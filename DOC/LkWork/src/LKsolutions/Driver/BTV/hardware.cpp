// LKsolutions company
// coded by Jongwhan Lee (leejw51@hitel.net)

// hardware related..


extern "C"
{
#include <ntddk.h>


}

#include "driver.h"
#include "ioctl.h"
#include "hardware.h"
#include <engine/LkBinutil/BinUtil.h>

void WriteReg(PBT878_VIDEOCHIP pChip,ULONG Reg, ULONG Data)
{
	PULONG r=(PULONG)(pChip->pMem+ Reg);
	WRITE_REGISTER_ULONG(r, Data);
}   

// READ_PORT_ULONG: for io ports
// READ_REGISTER_ULONG: for memorymapped registers

ULONG ReadReg(PBT878_VIDEOCHIP pChip, ULONG Reg	)
{
	PULONG r=(PULONG)(pChip->pMem+ Reg);
	ULONG status=READ_REGISTER_ULONG(r);
	return status;
}

////////////////////////////////////////////////////////////////////////////////////////
void HwSoftwareReset(PBT878_VIDEOCHIP pChip)
{
	// write any data will reset video decoder
	WriteReg(pChip, REG_SRESET, 0); 
}

void HwInterruptMask(PBT878_VIDEOCHIP pChip)
{
	ULONG a=ReadReg(pChip, REG_INT_MASK) & 0xFFFFFFF8;

	WriteReg(pChip, REG_INT_MASK, a);
}

void HwDisableInterrupt(PBT878_VIDEOCHIP pChip)
{
	ULONG a=0;
	WriteReg(pChip, REG_INT_MASK, a);
}

// for risc testing purpose
void HwInitProgram2(PBT878_SCREEN scr)
{
	
	
	ULONG a=0;
	int l=0;
	PULONG code=(PULONG)scr->pCode;
	PULONG start=(PULONG)scr->pCodePhy;
	PHYSICAL_ADDRESS code_physical=MmGetPhysicalAddress(scr->pCode);
	ULONG start2=code_physical.u.LowPart;
	
	
	int i;
	int h=scr->ulHeight;
	int w=scr->ulWidth;	

	PHYSICAL_ADDRESS frame_physical=MmGetPhysicalAddress(scr->pBuffer);
	ULONG frame=frame_physical.u.LowPart;
  


	// post interrupt
	code[l++]= RISC_JUMP |
		SetBin(24, "1") ; // IRQL
	code[l++]=(ULONG)start2;
	code[l++]=0;      
	code[l++]=0;
	code[l++]=0;  

	
}
  

// YUY2 format
void HwInitProgram(PBT878_SCREEN scr)
{
	
	  
	ULONG a=0;
	int l=0;
	PULONG code=(PULONG)scr->pCode;
	PULONG start=(PULONG)scr->pCodePhy;
	PHYSICAL_ADDRESS code_physical=MmGetPhysicalAddress(scr->pCode);
	ULONG start2=code_physical.u.LowPart;
	
	  
	int i;
	int h=scr->ulHeight;
	int w=scr->ulWidth;	

	PHYSICAL_ADDRESS frame_physical=MmGetPhysicalAddress(scr->pBuffer);
	ULONG frame=frame_physical.u.LowPart;
  


	//////////////////////////////////////////////////////////////////////
	// packed data
	code[l++]=RISC_SYNC | 
		SetBin(15, "1") |// RESYNC
		BIT_VRE; // even field to follow
	code[l++]=0;
		  
	code[l++]=RISC_SYNC | 
			BIT_FM1 // packed data			
		;  
	code[l++]=0;
	// post interrupt

	for (i=0;i<h;i+=2)
	{
		code[l++]= RISC_WRITE |
		SetBin(26, "1") | // SOL
		SetBin(27, "1")  | // EOL
		SetBin(11,0, w*2)  // width
		;

		a=frame+w*i*2; // address
		code[l++]=a;


		  
	}


	code[l++]=RISC_SYNC | 
	SetBin(15, "1") |// RESYNC
	BIT_VRO; // even field to follow
	code[l++]=0;

	/////////////////////////////////////////////////////////////////////		
	code[l++]=RISC_SYNC | 
			BIT_FM1 // packed data			
		;  
	code[l++]=0;
	// post interrupt

	for (i=1;i<h;i+=2)
	{
		code[l++]= RISC_WRITE |
		SetBin(26, "1") | // this instruction is first on this scan line
		SetBin(27, "1")  | // this instruction is last on this scan line
		SetBin(11,0, w*2)  // width
		;

		a=frame+w*i*2; // address
		code[l++]=a;


	}
  
  
	//////////////////////////////////////////////////////////////////////
  	code[l++]= RISC_JUMP | 
	SetBin(24, "1") ; // IRQL
	code[l++]=(ULONG)start2;
	code[l++]=0;
	code[l++]=0;    
	code[l++]=0;     
	code[l++]=0; 



	// packed data
//	code[l++]= RISC_SYNC |BIT_VRO;
//	code[l++]=0;



	
}

// planar mode  YV12
void HwInitProgramYV12(PBT878_SCREEN scr)
{
	
	
	ULONG a=0;
	int l=0;
	PULONG code=(PULONG)scr->pCode;
	PULONG start=(PULONG)scr->pCodePhy;
	PHYSICAL_ADDRESS code_physical=MmGetPhysicalAddress(scr->pCode);
	ULONG start2=code_physical.u.LowPart;
	
	  
	int i;
	ULONG h=scr->ulHeight;
	ULONG w=scr->ulWidth;	

	PHYSICAL_ADDRESS frame_physical=MmGetPhysicalAddress(scr->pBuffer);
	ULONG frame=frame_physical.u.LowPart;
  


	//////////////////////////////////////////////////////////////////////
	// packed data
	code[l++]=RISC_SYNC | 
		SetBin(15, "1") |// RESYNC
		BIT_VRE; // even field to follow
	code[l++]=0;
		  
	code[l++]=RISC_SYNC | 
			BIT_FM3 // planar
		;  
	code[l++]=0;
	// post interrupt

	ULONG cw=w/2;
	ULONG ch=h/2;
	int j=0;
	for (i=0;i<h;i+=2,j++)
	{
		
		code[l++]= RISC_WRITE123 |
		SetBin(26, "1") | // SOL
		SetBin(27, "1")  | // EOL
		SetBin(11,0, w)  // width, fifo 1 count
		;

		
		code[l++]=  
			SetBin(11,0, cw) | // fifo2 count
			SetBin(27,16, cw) ; // fifo3 count

		a=(frame)+w*i; // address for Y
		code[l++]=a;  
  
		
		a=(frame+ w*w + cw* cw) + cw*j; // address for Cr
		code[l++]=a;

		
		a=(frame+ w*w) + cw*j; // address for Cb
		code[l++]=a;
    
	
		  
	}
   

	code[l++]=RISC_SYNC | 
	SetBin(15, "1") |// RESYNC
	BIT_VRO; // even field to follow
	code[l++]=0;

	/////////////////////////////////////////////////////////////////////		
	code[l++]=RISC_SYNC | 
			BIT_FM3 // planar
		;  
	code[l++]=0;
	// post interrupt
	
	
	for (i=1;i<h;i+=2)
	{
		code[l++]= RISC_WRITE1S23 |
		SetBin(26, "1") | // SOL
		SetBin(27, "1")  | // EOL
		SetBin(11,0, w)  // width
		;
  
		
		code[l++]=
			SetBin(11,0, cw) |
			SetBin(27,16, cw) ;

		a=(frame) + w*i; // address for Y
		code[l++]=a;
         
	
		
	}
  
  
	//////////////////////////////////////////////////////////////////////
  	code[l++]= RISC_JUMP | 
	SetBin(24, "1") ; // IRQL
	code[l++]=(ULONG)start2;
	code[l++]=0;
	code[l++]=0;    
	code[l++]=0;     
	code[l++]=0; 



	// packed data
//	code[l++]= RISC_SYNC |BIT_VRO;
//	code[l++]=0;



	
}


// total resolution 780 x 525
// output resolution   640 x 480 
void SetCropScale(PBT878_VIDEOCHIP pChip)
{
	ULONG a;
	// set scale
	// 640 x 480
	char hscale[256];
	char hdelay[256];
	char hactive[256];
	char vscale[256];
	char vdelay[256];
	char vactive[256];
	
	/*
	Dec2Bin(16, hscale, 682); // 16 bits
	Dec2Bin(13, vscale, 0); // 13 bits

	Dec2Bin(10, hdelay, 134);
	Dec2Bin(10, hactive, 640);    

	Dec2Bin(10,	vdelay, 32);      
	Dec2Bin(10, vactive, 480);    
  */

	Dec2Bin(16, hscale, 732); // 16 bits
	Dec2Bin(13, vscale, 0); // 13 bits

	Dec2Bin(10, hdelay, 114);
	Dec2Bin(10, hactive, 640);  
  
	Dec2Bin(10,	vdelay, 26);  
	Dec2Bin(10, vactive, 480);  
  	    
    
  
	  
	// msb..
	ULONG crop=
	SetBin(7, vdelay, 9, 8) |
	SetBin(5, vactive, 9, 8) |
	SetBin(3, hdelay, 9, 8) |
	SetBin(1, hactive, 9, 8) ;
	WriteReg(pChip, REG_E_CROP, crop);
	WriteReg(pChip, REG_O_CROP, crop);
	
	ULONG vdelay_lo= SetBin(7, vdelay, 7, 0);
	WriteReg(pChip, REG_E_VDELAY_LO, vdelay_lo);
	WriteReg(pChip, REG_O_VDELAY_LO, vdelay_lo);

	ULONG vactive_lo= SetBin(7, vactive, 7, 0);
	WriteReg(pChip, REG_E_VACTIVE_LO, vactive_lo);
	WriteReg(pChip, REG_O_VACTIVE_LO, vactive_lo);  

	ULONG hdelay_lo= SetBin(7, hdelay, 7, 0);
	WriteReg(pChip, REG_E_HDELAY_LO, hdelay_lo);
	WriteReg(pChip, REG_O_HDELAY_LO, hdelay_lo);

	ULONG hactive_lo= SetBin(7, hactive, 7, 0);  
	WriteReg(pChip, REG_E_HACTIVE_LO, hactive_lo);
	WriteReg(pChip, REG_O_HACTIVE_LO, hactive_lo);

	// horizontal scale    
	WriteReg(pChip, REG_E_HSCALE_LO, SetBin(7, hscale, 7, 0));
	WriteReg(pChip, REG_O_HSCALE_LO, SetBin(7, hscale, 7,0));
	WriteReg(pChip, REG_E_HSCALE_HI, SetBin(7, hscale, 15,8));
	WriteReg(pChip, REG_O_HSCALE_HI, SetBin(7, hscale, 15,8));

	// vertical scale upper bytes
	a=ReadReg(pChip, REG_E_VSCALE_HI);
	SetBin(a, 4, vscale, 12, 8);
	WriteReg(pChip, REG_E_VSCALE_HI, a);

	a=ReadReg(pChip, REG_O_VSCALE_HI);
	SetBin(a, 4, vscale, 12, 8);
	WriteReg(pChip, REG_O_VSCALE_HI, a);

	// vertical scale low bytes
	WriteReg(pChip, REG_E_VSCALE_LO, SetBin(7, vscale, 7, 0));
	WriteReg(pChip, REG_O_VSCALE_LO, SetBin(7, vscale, 7, 0));  


	
}

// 30 frames
// 60 fields
// see HwInitProgram
// EVEN
// ODD

void SetEtc(PBT878_VIDEOCHIP pChip)
{

}  
  
void SetVideo(PBT878_VIDEOCHIP pChip)
{
	ULONG a=0;


	
	
	// "1000" YCrCb 4:2:2 planar YV12
	// "0100" YUY2
	WriteReg(pChip, REG_COLOR_FMT, SetBin(7,"0100") | SetBin(3, "0100") ); // odd, even field
    
        
	char mux[4][10] = {"10", "11","01", "00"};
	// set ntsc
	WriteReg(pChip, REG_IFORM, 
		SetBin(7,  "0") |  // reserved
		SetBin( 6, mux[pChip->nMuxCh%4])| // 10: mux0, 11:mux1, 01:mux2, 00:mux3
		SetBin(4,  "11") | // reserved
		SetBin(2, "001")
	);    
	// "000" AUTOFORMAT
	// "001" NTSC(M)   
	// "010" NTSC JAPAN
     
      
  
	SetCropScale(pChip);
	SetEtc(pChip);   
	
}


void HwStopRisc(PBT878_VIDEOCHIP pChip)
{
	

	// interrupt mask disable
	WriteReg(pChip, REG_INT_MASK, 0x0);

	ULONG dma=ReadReg(pChip, REG_GPIO_DMA_CTL);
	char dma_bits[16+100]; Dec2Bin(16, dma_bits, dma);

		
	SetBin(dma_bits, 1, "0"); // risc disable
	SetBin(dma_bits, 0, "0"); // fifo disable
	WriteReg(pChip, REG_GPIO_DMA_CTL, Bin2Dec(dma_bits));

	
	// disable capture
	WriteReg(pChip, REG_CAP_CTL, 0 ); 
	

	

}

void HwRunRisc(PBT878_VIDEOCHIP pChip)
{
	SetVideo(pChip);

	ULONG dma=ReadReg(pChip, REG_GPIO_DMA_CTL);
	char dma_bits[16+100]; Dec2Bin(16, dma_bits, dma);

	
	// set start address
	WriteReg(pChip, REG_RISC_STRT_ADD, pChip->Scr.pCodePhy);	
	
	// risc enable, fifo enable
	SetBin(dma_bits, 1, "1"); // risc enable
	SetBin(dma_bits, 0, "1"); // fifo enable
	WriteReg(pChip, REG_GPIO_DMA_CTL, Bin2Dec(dma_bits));

	// capture control
	WriteReg(pChip, REG_CAP_CTL, 
		SetBin( 1, "1")  |// capture odd
		SetBin( 0, "1")   // capture even
		);

	// interrupt mask
	WriteReg(pChip,REG_INT_MASK,   
	BIT_RISCI		| // RISCI
	BIT_VSYNC 
	); 
	  
    
	

	
}
