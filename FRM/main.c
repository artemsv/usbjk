/* Name: main.c
 * Project: PowerSwitch based on AVR USB driver
 * Author: Christian Starkjohann
 * Creation Date: 2005-01-16
 * Tabsize: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: Proprietary, free under certain conditions. See Documentation.
 * This Revision: $Id: main.c 183 2006-03-26 19:07:22Z cs $
 */

#define __AVR_ATtiny2313__ 1

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>


#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"

#include "usbconfig.h"

/*
This module implements an 8 bit parallel output controlled via USB. It is
intended to switch the power supply to computers and/or other electronic
devices.

Application examples:
- Rebooting computers located at the provider's site
- Remotely switch on/off of rarely used computers
- Rebooting other electronic equipment which is left unattended
- Control room heating from remote
*/

//#define TEST_DRIVER_SIZE 1
#ifndef TEST_DRIVER_SIZE    /* define this to check out size of pure driver */

#define EEPROM_LOCATION 37

static uchar    actionTimers[8];
unsigned char flag;

/* This is the AT90S2313 version of the routine. Change for others. */
/*
static void outputByte(uchar b)
{
    //PORTB = (PORTB & ~0xf8) | (b & 0xf8);
    PORTD = (PORTD & ~0x30) | ((b << 4) & 0x30);
}
*/
#define    EEMWE   2
#define    EEWE    1

static void eepromWrite(unsigned char addr, unsigned char val)
{
    while(EECR & (1 << EEWE));
    EEARL = addr;
    EEDR = val;
    cli();
    EECR |= 1 << EEMWE;
    EECR |= 1 << EEWE;  /* must follow within a couple of cycles -- therefore cli() */
    sei();
}

static uchar    eepromRead(uchar addr)
{
    while(EECR & (1 << EEWE));
    EEARL = addr;
    EECR |= 1 << EERE;
    return EEDR;
}

static uchar    computeTemporaryChanges(void)
{
uchar   i, status = 0, mask = 1;

    for(i=0;i<8;i++){
        if(actionTimers[i])
            status |= mask;
        mask <<= 1;
    }
    return status;
}
/*
static void computeOutputStatus(void)
{
uchar   status = eepromRead(EEPROM_LOCATION) ^ computeTemporaryChanges();

    outputByte(status);
}
*/
/* We poll for the timer interrupt instead of declaring an interrupt handler
 * with global interrupts enabled. This saves recursion depth. Our timer does
 * not need high precision and does not run at a high rate anyway.
 */
static void timerInterrupt(void)
{
static uchar    prescaler;
uchar           i;

    if(!prescaler--){
        prescaler = 8;  /* rate = 12M / 1024 * 256 * 9 */
        for(i=0;i<8;i++){
            if(actionTimers[i])
                actionTimers[i]--;
        }
        //computeOutputStatus();
    }
}

uchar   usbFunctionSetup(uchar data[8])
{
	if (data[1]  == 0xA1)
		PORTC |= _BV(PC5);
	else
		PORTC &= ~_BV(PC5);

    //flag = data[1];

uchar           status = eepromRead(EEPROM_LOCATION);
static uchar    replyBuf[2];

    usbMsgPtr = replyBuf;
    if(data[1] == 0){       /* ECHO */
        replyBuf[0] = data[2];
        replyBuf[1] = data[3];
        return 2;
    }
    if(data[1] == 1){       /* GET_STATUS -> result = 2 bytes */
        replyBuf[0] = status;
        replyBuf[1] = computeTemporaryChanges();
        return 2;
    }
    if(data[1] == 2 || data[1] == 3){   /* SWITCH_ON or SWITCH_OFF, index = bit number */
        uchar bit = data[4] & 7;
        uchar mask = 1 << bit;
        uchar needChange, isOn = status & mask;
        if(data[1] == 2){   /* SWITCH_ON */
            status |= mask;
            needChange = !isOn;
        }else{              /* SWITCH_OFF */
            status &= ~mask;
            needChange = isOn;
        }
        if(data[2] == 0){       /* duration == 0 -> permanent switch */
            actionTimers[bit] = 0;
            eepromWrite(EEPROM_LOCATION, status);
        }else if(needChange){   /* temporary switch: value = duration in 200ms units */
            actionTimers[bit] = data[2];
        }
    }
   // computeOutputStatus();

	return 0;
}

/* allow some inter-device compatibility */
#if !defined TCCR0 && defined TCCR0B
#define TCCR0   TCCR0B
#endif
#if !defined TIFR && defined TIFR0
#define TIFR    TIFR0
#endif

SIGNAL(SIG_INTERRUPT1)
{
    //GIMSK &= ~_BV(INT1);
    //TCCR1A = _BV(CS02) | _BV(CS00);		//  делитель 1024
    //TCNT1 = 50000;

	if (flag)
	{
		flag = 0;
		PORTC &= ~_BV(PC4);
		//PORTC = 0xFF;
	}
	else
	{
		flag = 1;
		PORTC |= _BV(PC4);
		//PORTC = 0;
	}
}

int main(void)
{
	uchar   i, j;

	flag = 0;
    wdt_enable(WDTO_1S);
    odDebugInit();
   // DDRD = ~(1 << 2);   /* all outputs except PD2 = INT0 */
    PORTD = 0;
    PORTB = 0;          /* no pullups on USB pins */
    PORTC = 0;
   // PORTC |= _BV(PC5);

    DDRB = ~0;          /* output SE0 for USB reset */
    DDRC = 0xFF;
    PORTC |= _BV(PC5);
    PORTC |= _BV(PC4);
    //computeOutputStatus();  /* set output status before we do the delay */
    j = 0;
    while(--j){         /* USB Reset by device only required on Watchdog Reset */
        i = 0;
        while(--i);     /* delay >10ms for USB reset */
    }
    DDRB = ~USBMASK;    /* all outputs except USB data */
    TCCR0 = 5;          /* set prescaler to 1/1024 */
    usbInit();
    sei();

	//PORTB |= _BV(PB2);
	GIMSK |= _BV(INT1);

    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();
        if(TIFR & (1 << TOV0)){
            TIFR |= 1 << TOV0;  /* clear pending flag */
            timerInterrupt();
        }

        	if (usbInterruptIsReady())
                usbSetInterrupt(&flag, 1);

    }
    return 0;
}

#else   /* TEST_DRIVER_SIZE */

/* This is the minimum do-nothing function to determine driver size. The
 * resulting binary will consist of the C startup code, space for interrupt
 * vectors and our minimal initialization. The C startup code and vectors
 * (together ca. 70 bytes of flash) can not directly be considered a part
 * of this driver. The driver is therefore ca. 70 bytes smaller than the
 * resulting binary due to this overhead. The driver also contains strings
 * of arbitrary length. You can save another ca. 50 bytes if you don't
 * include a textual product and vendor description.
 */
uchar   usbFunctionSetup(uchar data[8])
{
    return 0;
}

int main(void)
{
#ifdef PORTD
    PORTD = 0;
    DDRD = ~(1 << 2);   /* all outputs except PD2 = INT0 */
#endif
    PORTB = 0;          /* no pullups on USB pins */
    DDRB = ~USBMASK;    /* all outputs except USB data */
    usbInit();
    sei();
    for(;;){    /* main event loop */
        usbPoll();
    }
    return 0;
}

#endif  /* TEST_DRIVER_SIZE */
