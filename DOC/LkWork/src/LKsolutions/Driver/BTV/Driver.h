// LK Solutions
// Coder: Jongwhan Lee
// Email: leejw51@hitel.net

#pragma once
extern "C" {
#include <ntddk.h>
}
#include "unicode.h"
#include "generic/generic.h"

#define BT_MAX_CHIP 4

typedef struct {

	// virtual address
	PUCHAR pCode; // for risc code memory
	PUCHAR pBuffer; // for screen frame buffer

	ULONG pCodePhy; // physical addresses
	ULONG pBufferPhy;

	ULONG ulWidth;
	ULONG ulHeight;

	// user address
	ULONG pUserBuffer;
}
BT878_SCREEN, * PBT878_SCREEN;

typedef
struct {
	UCHAR ucChipId;
	UCHAR ucChNum;

	BT878_SCREEN Scr; 	

	// Hardware information
	PUCHAR pMem; // memory of this chip
	ULONG ulMemLeng;  // lengh

	// Interrupt
	KIRQL IntIrql; // interrutp irql
	ULONG IntVec; // interrupt vector
	KAFFINITY IntAff; // interrupt affinity
	KINTERRUPT_MODE IntMode; // Latched, Level Sensitive
	
	PKINTERRUPT pIntObj; // interrupt object

	// data
	ULONG nMuxCh; // mux channel : 0, 1, 2, 3
}
BT878_VIDEOCHIP, * PBT878_VIDEOCHIP;


typedef
struct {
	BT878_VIDEOCHIP pChips[BT_MAX_CHIP];   

	ULONG ulChipNumber; // total chip number <= BT_MAX_CHIP


	// for file handling
	PFILE_OBJECT pFileObject;
	PVOID pHandleObject;
	PKEVENT pEvent;
}
BT878_VIDEO, * PBT878_VIDEO;

typedef   
struct {
	PBT878_VIDEOCHIP pVideo;   	


	PDEVICE_OBJECT pDevice; // functional device object
	PDEVICE_OBJECT pLowerDevice; // lower device at the stack

	ULONG DeviceNumber; // device number
	CUString ustrDeviceName; // device name
	CUString ustrSymLinkName; // symbolic name

	IO_REMOVE_LOCK RemoveLock;				// removal control locking structure
	PGENERIC_EXTENSION pgx;
}
DEVICE_EXTENSION, * PDEVICE_EXTENSION;

    
typedef
struct {
	PFILE_OBJECT pFileObject;
}
HANDLE_OBJECT, *PHANDLE_OBJECT;
