// COMPANY:   LKsolutions Inc.
//
// the	enterprise	for		humanbeings
// the	technology	for		humanbeings
// the	hope		for		humanbeings

//
// products line up:
// Multimedia Communication with 3D technology
// founded in 2002. 7. 23 

// 
// code by : Jongwhan Lee (leejw51@hitel.net) ; Lead Programmer 
// website: superfrontier.com

// 2002. 8. 7   : basic frame
//       8.8    : plug & play, INF file
//
// credits:
// 
// generic device driver : Art Baker, Jerry Lozano from The Windows 2000 Device Driver Book


extern "C"
{
#include <ntddk.h>
}
#include <engine/LkBinutil/binutil.h>
#include "driver.h"
#include "ioctl.h"
#include "hardware.h"



// IRQL (interrupt request level ) EXPLANATION
//-------------------------------------------------
// I/O Request : PASSIVE_LEVEL   ; DispatchCreate, DispatchClose
// Start I/O Routine : DISPATCH_LEVEL 
// ISR(interrupt service routine) : DIRQL LEVEL ; device interrupt, can be many...
// DpcForIsr : DISPATCH_LEVEL

// interrupt types---
// Latched : interrupt can be false
// Level Sensitive: correct interrupt

// memory access --------------------------
// we can use paged memory only in
// APC_LEVEL(1), PASSIVE_LEVEL (0)



//////////////////////////////////////////////////////////////////////////////////////////////
//  DRIVER ELEMENTS
//////////////////////////////////////////////////////////////////////////////////////////////
//  PDRIVER_EXTENSION DriverExtension;
//  PDRIVER_INITIALIZE DriverInit;
//  PDRIVER_STARTIO DriverStartIo;
//  PDRIVER_UNLOAD DriverUnload;
//  PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
//////////////////////////////////////////////////////////////////////////////////////////////
VOID DriverUnload(IN PDRIVER_OBJECT);
NTSTATUS AddDevice (IN PDRIVER_OBJECT , IN PDEVICE_OBJECT ); // driver extension function

//////////////////////////////////////////////////////////////////////////////////////////////
// Dispatch Functions
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT , IN PIRP);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT, IN PIRP);
NTSTATUS DispatchControl(IN PDEVICE_OBJECT , IN PIRP );

NTSTATUS DispatchPnp(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS PassdownPnp(IN PDEVICE_OBJECT , IN PIRP);
NTSTATUS HandleStartDevice(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS HandleStopDevice(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS HandleRemoveDevice(IN PDEVICE_OBJECT , IN PIRP );

NTSTATUS DispatchPower(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS DispatchShutdown(IN PDEVICE_OBJECT , IN PIRP );

// interrupt routine

// work which takes much time should be done in DPC
void DpcforIsr(IN PKDPC , IN PDEVICE_OBJECT ,  IN PIRP , IN PVOID );
BOOLEAN Isr (IN PKINTERRUPT , IN PVOID );

// for generic library
NTSTATUS StartDevice(PDEVICE_OBJECT fdo, PCM_PARTIAL_RESOURCE_LIST raw, PCM_PARTIAL_RESOURCE_LIST translated);
VOID StopDevice(IN PDEVICE_OBJECT fdo, BOOLEAN oktouch /* = FALSE */);
VOID RemoveDevice(IN PDEVICE_OBJECT fdo);

VOID GetMemory();
VOID ReleaseMemory();
void CloseCaptureEvent();
void GiveSignal();
void InitHardware(PBT878_VIDEOCHIP );
void MapMdl(PBT878_VIDEOCHIP );
NTSTATUS CompleteRequest(IN PIRP pIrp, IN NTSTATUS status, IN ULONG_PTR info);


void TestScreen();

void StartThread();


//Global Variables
#pragma data_seg ()
int g_ulDeviceNumber=0; // number of devices
PBT878_VIDEO g_Video=NULL;

////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code_seg("init")
extern "C" NTSTATUS DriverEntry(
			IN PDRIVER_OBJECT pDriverObject,
			IN PUNICODE_STRING pRegistryPath)
{
	//BREAK  
	
	int a=Bin2Dec("1111");

	char* notice="okay";
	NTSTATUS status=STATUS_SUCCESS;


	// call-back functions
	pDriverObject->DriverUnload= DriverUnload;
	pDriverObject->DriverExtension->AddDevice= AddDevice;

	pDriverObject->MajorFunction[IRP_MJ_CREATE]= DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE]=DispatchClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;

	pDriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
	pDriverObject->MajorFunction[IRP_MJ_POWER] = DispatchPower;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = DispatchShutdown;


	GetMemory();
	//StartThread();

	return status;     
}


////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code_seg()
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	
	PDEVICE_OBJECT pNextObj;
	pNextObj=pDriverObject->DeviceObject;

	while(pNextObj!=NULL) {
		PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pNextObj->DeviceExtension;
		

		UNICODE_STRING pLinkName=pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		


		pNextObj=pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}

	ReleaseMemory();
}
////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);
	g_Video->pFileObject=stack->FileObject;

	PHANDLE_OBJECT pObj= (PHANDLE_OBJECT)ExAllocatePool(NonPagedPool, sizeof(HANDLE_OBJECT));
	pObj->pFileObject= stack->FileObject;
	stack->FileObject->FsContext=pObj;
	g_Video->pFileObject= stack->FileObject;
	g_Video->pHandleObject=(PVOID) pObj;

	/////////////////////////////////////////////////////////////////////////////
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	pIrp->IoStatus.Information=0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);


	return STATUS_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	
	PDEVICE_EXTENSION pDevExt= (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	PBT878_VIDEOCHIP pChip=pDevExt->pVideo;

	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);

	// stop risc
	HwStopRisc(pChip);

	// disable interrupt
	HwDisableInterrupt(pChip);  

	// close
	CloseCaptureEvent();
	
	PHANDLE_OBJECT pObj=(PHANDLE_OBJECT)stack->FileObject->FsContext;
	ExFreePool(pObj);
		g_Video->pFileObject= NULL;
	g_Video->pHandleObject=NULL;
	

	pIrp->IoStatus.Status=STATUS_SUCCESS;
	pIrp->IoStatus.Information= 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// run at : PASSIVE_LEVEL
NTSTATUS DispatchControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	
	int irql=KeGetCurrentIrql();

	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	PBT878_VIDEOCHIP pChip=pDevExt->pVideo;

	NTSTATUS status=STATUS_SUCCESS;
	ULONG info=0;

	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);
	ULONG cbin=		stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG cbout=	stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code=		stack->Parameters.DeviceIoControl.IoControlCode;

	switch(code)
	{
		case IOCTL_TEST_DEVICE:
		{
			PULONG write=(PULONG) pIrp->AssociatedIrp.SystemBuffer;
			*write=100;
			*(write+1)=200;
			*(write+2)=300;
			info=cbout;
			TestScreen();
			break;
		}

		case IOCTL_TEST_REGISTEREVENT:
		{
			
			int irql=KeGetCurrentIrql();

			if (cbin<sizeof(HANDLE)) {
				status=STATUS_INVALID_PARAMETER;
			
				break;
			}  
			// register event
			HANDLE hEvent=*(PHANDLE)pIrp->AssociatedIrp.SystemBuffer;
			//if (!RegisterEvent(pdx, stack->FileObject, *(PHANDLE) Irp->AssociatedIrp.SystemBuffer, Irp->RequestorMode))
			ObReferenceObjectByHandle(
				hEvent,
				EVENT_MODIFY_STATE,  // access mask
				*ExEventObjectType, // optional, 
				pIrp->RequestorMode, // object type
				 (PVOID*)&g_Video->pEvent, 
				NULL);
   
  
			break;  
		}  

		case IOCTL_TEST_CALLEVENT:
		{
			GiveSignal();
			break;
		}

		case IOCTL_TEST_START_CAPTURE:
		{
			if (cbin==sizeof(ULONG)) {
				pChip->nMuxCh= * (PULONG)pIrp->AssociatedIrp.SystemBuffer;
				HwRunRisc(pChip);
			}
			else {
				status=STATUS_INVALID_PARAMETER;
			}
		}
		break;

		case IOCTL_TEST_FINISH_CAPTURE:
		
		HwStopRisc(pChip);		
		break;


		case IOCTL_TEST_GET_ADDR:
		{
			
			PULONG write=(PULONG) pIrp->AssociatedIrp.SystemBuffer;
			MapMdl(pChip);
			*write=pChip->Scr.pUserBuffer;
			info=cbout;
		}
		break;

		case IOCTL_TEST_SET_CHANNEL:
		{
		}

		break;

		default:
		status=STATUS_INVALID_DEVICE_REQUEST;
		break;
	}


	// report
	return CompleteRequest(pIrp, status, info);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////

// FDO : Functional Device Object
// PDO : Physical Device Object
NTSTATUS AddDevice (IN PDRIVER_OBJECT pDriverObject, IN PDEVICE_OBJECT pPhysicalDeviceObject)
{
		char* test="AddDevice2";
	NTSTATUS status;
	PDEVICE_OBJECT pfdo;
	PDEVICE_EXTENSION pDevExt;

	// calculate size
	ULONG dxsize = (sizeof(DEVICE_EXTENSION) + 7) & ~7;
	ULONG xsize = dxsize + GetSizeofGenericExtension();
	

	CUString devName("\\Device\\LKbtv");
	devName+=CUString(g_ulDeviceNumber); // zero base number

	// CREATE FUNCTIONAL DEVICE OBJECT
	status=IoCreateDevice(pDriverObject,
		xsize,
		&(UNICODE_STRING)devName,
		FILE_DEVICE_UNKNOWN, // device type
		0,  // device characteristics
		FALSE,  //	exclusive : this device driver is not exclusive device
		&pfdo);
	if (!NT_SUCCESS(status)) return status;

	// Buffered IO
	pfdo->Flags |= DO_BUFFERED_IO;

	// initialize device extension
	pDevExt = (PDEVICE_EXTENSION) pfdo->DeviceExtension;
	pDevExt->pDevice= pfdo;
	pDevExt->DeviceNumber= g_ulDeviceNumber;
	pDevExt->ustrDeviceName= devName;
	// put this device on the device stack
	pDevExt->pLowerDevice= IoAttachDeviceToDeviceStack(pfdo, pPhysicalDeviceObject);

	// connect
	pDevExt->pVideo=  g_Video->pChips+ g_ulDeviceNumber;
	


	// create symblic link
	CUString symLinkName("\\??\\LKbtv");
	symLinkName+=CUString(g_ulDeviceNumber+1); // 1 base number
	pDevExt->ustrSymLinkName= symLinkName;
	status= IoCreateSymbolicLink(&(UNICODE_STRING)symLinkName, &(UNICODE_STRING)
		devName);
	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(pfdo);
		return status;
	}


	// Initialize to use the GENERIC.SYS library
	pDevExt->pgx = (PGENERIC_EXTENSION) ((PUCHAR) pDevExt + dxsize);
	GENERIC_INIT_STRUCT gis = {sizeof(GENERIC_INIT_STRUCT)};
	gis.DeviceObject = pfdo;
	gis.Pdo = pPhysicalDeviceObject;
	gis.Ldo = pDevExt->pLowerDevice;
	gis.RemoveLock = &pDevExt->RemoveLock;
	gis.StartDevice = StartDevice;
	gis.StopDevice = StopDevice;
	gis.RemoveDevice = RemoveDevice;
	RtlInitUnicodeString(&gis.DebugName, devName);
	status = InitializeGenericExtension(pDevExt->pgx, &gis);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pfdo);
		return status;
	}  

	// make a notice that FDO was created outside of DriverEntry
	pfdo->Flags &= ~DO_DEVICE_INITIALIZING;

	// increate device nunmber
	
	g_ulDeviceNumber++;
	g_Video->ulChipNumber++;
  

	
	return STATUS_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Plug & Play
// work in PASSIVE_LEVEL
NTSTATUS PnpComplete(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp, IN PVOID pContext)
{
	

	NTSTATUS status=STATUS_SUCCESS;
	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);
	switch(stack->MajorFunction)
	{
		case IRP_MJ_PNP:
		return STATUS_MORE_PROCESSING_REQUIRED;
		break;

		case IRP_MJ_POWER:
		break;

	}

	return status;
}

NTSTATUS DispatchPnp(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	PIO_STACK_LOCATION pIrpStack;
	pIrpStack=IoGetCurrentIrpStackLocation(pIrp);
	switch(pIrpStack->MinorFunction)
	{
		case IRP_MN_START_DEVICE:
		{
			
			 
			
			KEVENT e;  
			KeInitializeEvent(&e, NotificationEvent, FALSE);
			IoCopyCurrentIrpStackLocationToNext(pIrp);
			IoSetCompletionRoutine(pIrp, PnpComplete, &e, TRUE, TRUE, TRUE);
			NTSTATUS status=IoCallDriver(pDevExt->pLowerDevice, pIrp);
			if (status==STATUS_PENDING) {
				
				KeWaitForSingleObject(&e, Executive, KernelMode, FALSE, NULL);
				status=pIrp->IoStatus.Status;
			}
			
			if (NT_SUCCESS(status)) status=HandleStartDevice(pDevObj, pIrp);
			pIrp->IoStatus.Status=status;
			pIrp->IoStatus.Information=0;
			IoCompleteRequest(pIrp, IO_NO_INCREMENT);

			return status; //bug check
			// if status is not STATUS_SUCCESS
			// device will stop
			
		
		
		}
		 
		case IRP_MN_STOP_DEVICE:
		return HandleStopDevice(pDevObj, pIrp);

		case IRP_MN_REMOVE_DEVICE:
		return HandleRemoveDevice(pDevObj, pIrp);
		
		default:
		return PassdownPnp(pDevObj, pIrp);

	}
	//PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	//return GenericDispatchPnp(pdx->pgx, pIrp);
}

NTSTATUS PassdownPnp(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	IoSkipCurrentIrpStackLocation(pIrp);
	
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	return IoCallDriver(pDevExt->pLowerDevice, pIrp);
}



NTSTATUS HandleStartDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	PBT878_VIDEOCHIP pChip=pDevExt->pVideo;

	//  { ULONG Count, CM_FULL_RESOURCE_DESCRIPTOR List[1]}
	PCM_RESOURCE_LIST pResourceList=stack->Parameters.StartDevice.AllocatedResourcesTranslated;

	// {CM_PARTIAL_RESOURCE_LIST PartialResourceList}
	PCM_FULL_RESOURCE_DESCRIPTOR pFullDescriptor=pResourceList->List;

	// {ULONG Count, CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1] }
	PCM_PARTIAL_RESOURCE_LIST pPartialList=&pFullDescriptor->PartialResourceList;

	PCM_PARTIAL_RESOURCE_DESCRIPTOR pPartialDescriptor=NULL; // real data

	int i;  
	int n=pPartialList->Count;
	for (i=0;i<n;i++)
	{
		pPartialDescriptor=& pPartialList->PartialDescriptors[i];

		switch (pPartialDescriptor->Type) 
		{
			case CmResourceTypeInterrupt:
			{
				
				KIRQL IRQL=(KIRQL)pPartialDescriptor->u.Interrupt.Level;
				ULONG Vector=pPartialDescriptor->u.Interrupt.Vector;
				KAFFINITY Affinity=pPartialDescriptor->u.Interrupt.Affinity;
				KINTERRUPT_MODE Mode= pPartialDescriptor->Flags& CM_RESOURCE_INTERRUPT_LATCHED 
					? Latched : LevelSensitive;

				pChip->IntIrql= IRQL;
				pChip->IntVec= Vector;
				pChip->IntAff= Affinity;
				pChip->IntMode= Mode;
				
				
			
			}
			break;

			case CmResourceTypeDma:
			
			{
			int a=5;
			// got DMA
			}
			break;

			case CmResourceTypePort:
			{
				

				PUCHAR portBase=(PUCHAR)pPartialDescriptor->u.Port.Start.LowPart;
				ULONG portLength=pPartialDescriptor->u.Port.Length;
			}
			break;

			case CmResourceTypeMemory:
			{
				

				PHYSICAL_ADDRESS mem=pPartialDescriptor->u.Memory.Start;
				ULONG leng=pPartialDescriptor->u.Memory.Length;
				
				// MmMapIoSpace(PHYSICAL_ADDRESS, SIZE, MEMORY_CACHING_TYPE)
				PUCHAR addr=(PUCHAR)MmMapIoSpace(mem, leng, MmNonCached);
				ULONG si= leng;

				pChip->pMem=addr;
				pChip->ulMemLeng=si;

				
			}
			break;  
		}
	} // end of for

	// initialize hardware
	InitHardware(pChip);
  
	// interrupt
	IoInitializeDpcRequest(pDevExt->pDevice, DpcforIsr);
	// make dpc
	//KeInitializeDpc(&pDevExt->pDevice->Dpc, (PKDEFERRED_ROUTINE) DpcforIsr, pDevExt->pDevice);

	// connect hardware interrupt and ISR(interrupt service routine)
	// if (0) 
	
	NTSTATUS status=STATUS_SUCCESS;
	{
	
	status=IoConnectInterrupt(
		&pChip->pIntObj, // interrupt object
		Isr, // interrupt service routine
		pDevExt,  // service context
		NULL, // spinlock -- optional
		pChip->IntVec, // vector
		pChip->IntIrql, // irql
		pChip->IntIrql, // synchronize irql
		pChip->IntMode, //interrupt mode
		TRUE, // share vector
		pChip->IntAff, // processor enable mask
		FALSE); // floating save
	if (!NT_SUCCESS(status)) return status;
  
	}
	return STATUS_SUCCESS;
}

NTSTATUS HandleStopDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	PBT878_VIDEOCHIP pChip=pDevExt->pVideo;

	if (pChip->pIntObj)  { // disconnect interrupt
		IoDisconnectInterrupt(pChip->pIntObj);
		pChip->pIntObj=NULL;
	}
	
	PassdownPnp(pDevObj, pIrp);
	return STATUS_SUCCESS;
}

NTSTATUS HandleRemoveDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	

	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	
	// detach first
	if (pdx->pLowerDevice) IoDetachDevice(pdx->pLowerDevice); // corresponding
	// IoAttachDeviceToDeviceStack of AddDevice

	// delete 
	IoDeleteDevice(pDevObj); // corresponding IoCreateDevice of AddDevice

	// release memory
	pdx->ustrDeviceName.Free();
	pdx->ustrSymLinkName.Free();

	// success
	pIrp->IoStatus.Status=STATUS_SUCCESS;


	PassdownPnp(pDevObj, pIrp);
	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS DispatchShutdown(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{

	pIrp->IoStatus.Status=STATUS_SUCCESS;
	pIrp->IoStatus.Information=0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

////////////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS DispatchPower(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{	
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	return GenericDispatchPower(pdx->pgx, pIrp);	
}



////////////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS StartDevice(PDEVICE_OBJECT fdo, PCM_PARTIAL_RESOURCE_LIST raw, PCM_PARTIAL_RESOURCE_LIST translated)
{							// StartDevice
	return STATUS_SUCCESS;
}							// StartDevice

VOID StopDevice(IN PDEVICE_OBJECT fdo, BOOLEAN oktouch /* = FALSE */)
{							// StopDevice
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;
}							// StopDevice
VOID RemoveDevice(IN PDEVICE_OBJECT fdo)
{							// RemoveDevice
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;
//	NTSTATUS status;

	if (pdx->pLowerDevice)
		IoDetachDevice(pdx->pLowerDevice);

	IoDeleteDevice(fdo);  
}

////////////////////////////////////////////////////////////////////////////////////////////////
void TestScreen()
{


}

/*
void InitScr(PBT878_SCREEN scr)
{
	scr->ulWidth= 640;
	scr->ulHeight= 480;

	int n=640*480*4*2;  
	scr->pBuffer=(PUCHAR)ExAllocatePool(	NonPagedPoolCacheAligned, n);
	RtlZeroMemory(scr->pBuffer, n);
	scr->pBufferPhy=MmGetPhysicalAddress(scr->pBuffer).u.LowPart;

	n=4096*4;
	scr->pCode=(PUCHAR)ExAllocatePool(		NonPagedPoolCacheAligned, n);
	RtlZeroMemory(scr->pCode, n);
	scr->pCodePhy=MmGetPhysicalAddress(scr->pCode).u.LowPart;
  
	  

	HwInitProgram(scr);
}*/
void InitScr(PBT878_SCREEN scr)
{
	scr->ulWidth= 640;
	scr->ulHeight= 480;

	PHYSICAL_ADDRESS highest;
	highest.QuadPart=0xffffffffffffffff;
	int n=640*480*4*2;  
	scr->pBuffer=(PUCHAR)MmAllocateContiguousMemory(n,highest);
	RtlZeroMemory(scr->pBuffer, n);
	scr->pBufferPhy=MmGetPhysicalAddress(scr->pBuffer).u.LowPart;

	n=4096*4;
	//scr->pCode=(PUCHAR)ExAllocatePool(		NonPagedPoolCacheAligned, n);
	scr->pCode=(PUCHAR)MmAllocateContiguousMemory(n, highest);
	RtlZeroMemory(scr->pCode, n);
	scr->pCodePhy=MmGetPhysicalAddress(scr->pCode).u.LowPart;
  
	  

	HwInitProgram(scr);
}
// has four video sources
void InitVideochip(PBT878_VIDEOCHIP chip)
{
	chip->ucChNum=1; // by default, one channel
	InitScr(&chip->Scr);
}

// has many chips
void InitVideo(PBT878_VIDEO video)
{
	int i;
	for (i=0;i<BT_MAX_CHIP;i++)
	{
		PBT878_VIDEOCHIP chip=video->pChips+i;
		chip->ucChipId=i;
		InitVideochip(chip);		
	}
}
void ClearVideo(PBT878_VIDEO video)
{
	int i;
	for (i=0;i<BT_MAX_CHIP;i++)
	{
		PBT878_VIDEOCHIP chip=video->pChips+i;
		ExFreePool(chip->Scr.pCode); chip->Scr.pCode=NULL;
		ExFreePool(chip->Scr.pBuffer); chip->Scr.pBuffer=NULL;
	}
		
}

void GetMemory()
{
	
	int n=sizeof(BT878_VIDEO);
//	g_Video=(PBT878_VIDEO) ExAllocatePool(NonPagedPoolCacheAligned,
//		n);
	PHYSICAL_ADDRESS highest;
	highest.QuadPart=0xffffffffffffffff;
	g_Video=(PBT878_VIDEO) MmAllocateContiguousMemory(n,highest);
	RtlZeroMemory(g_Video, n); 

   
	InitVideo(g_Video);
}
void ReleaseMemory()
{
	ClearVideo(g_Video);

	ExFreePool(g_Video);

}
   


void CloseCaptureEvent()
{
	if (g_Video->pEvent) {
		ObDereferenceObject(g_Video->pEvent);
		g_Video->pEvent=NULL;
		
	}
}

// if application finishes, DispatchClose is called even though 
// application forgets calling "Close".
  
void GiveSignal()
{

	if (g_Video->pEvent==NULL) return;

	// send signal
	KeSetEvent(g_Video->pEvent, 0, FALSE);
}






// run at  dispatch level
void DpcforIsr(IN PKDPC pDpc, IN PDEVICE_OBJECT pDevObj,
			   IN PIRP pIrp, IN PVOID pContext)
{
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pContext;
	// send signal to application
	GiveSignal();
}



// run at DIRQL (device irql)
// contents of pServiceContext is up to IoConnectInterrupt
// we know which chip made this interrupt
// by pServiceContext value
BOOLEAN Isr (IN PKINTERRUPT pIntObj, IN PVOID pServiceContext)
{
	

	// all information we need
	// we know which device object , 
	PDEVICE_EXTENSION pDevExt= (PDEVICE_EXTENSION)pServiceContext; // device extension
	PDEVICE_OBJECT pDevObj= pDevExt->pDevice; // funtional device object
	PBT878_VIDEOCHIP pChip=pDevExt->pVideo;
	PIRP pIrp=pDevObj->CurrentIrp;

	bool queue_dpc=false;  
   
 	
	// process (quick routine)
	ULONG int_stat=ReadReg(pChip, REG_INT_STAT); // interrupt status
	ULONG int_mask=ReadReg(pChip, REG_INT_MASK); // interrupt mask

	bool risc_enabled = int_stat & BIT_RISC_EN;
	ULONG risc_pc = ReadReg(pChip, REG_RISC_COUNT);	

	bool found=false;
	if (int_mask & BIT_SCERR) {
		if (int_stat & BIT_SCERR)
		{
			WriteReg(pChip,REG_INT_STAT, BIT_SCERR);
			found=true;
		}
	}
	
	if (int_mask& BIT_FDSR) {
		if (int_stat& BIT_FDSR) {
			WriteReg(pChip,REG_INT_STAT, BIT_FDSR); // clear to zero
			found=true;
		}
	}
	
	if (int_mask& BIT_RISCI) {
		if (int_stat& BIT_RISCI) {
				queue_dpc=true;
				WriteReg(pChip,REG_INT_STAT, BIT_RISCI);
				found=true;
		}
	}
	
	if (int_mask& BIT_VSYNC) {
		if (int_stat & BIT_VSYNC) {
			WriteReg(pChip,REG_INT_STAT, BIT_VSYNC);			
			found=true;
		}
	}
	if (!found) return FALSE; // this interrupt does not belong to this device


	if (queue_dpc) {
		// dpc!! (work which needs much time should go to Dpc)
		// DPC routine is execute on DISPATCH_LEVEL
		// here is DIRQL (device irql)
		IoRequestDpc(pDevObj, pIrp, (PVOID) pDevExt);
		//KeInsertQueueDpc(& pDevObj->Dpc, pIrp, pDevExt);
	}

	

	return TRUE; // this interrupt is processed..
}





void InitHardware(PBT878_VIDEOCHIP pChip)
{
	
	HwSoftwareReset(pChip);
	HwInterruptMask(pChip);
}

void MapMdlScreen(PBT878_SCREEN Scr)
{
	

	PVOID addr=Scr->pBuffer;
	PMDL mdl=IoAllocateMdl(addr, 640*480*2, FALSE, FALSE, NULL);

	MmBuildMdlForNonPagedPool(mdl);
	PVOID UserVirtualAddress= MmMapLockedPages(mdl, UserMode);

	Scr->pUserBuffer=(ULONG)UserVirtualAddress;

}
void MapMdl(PBT878_VIDEOCHIP pChip)
{
	MapMdlScreen(&pChip->Scr);
	
}
