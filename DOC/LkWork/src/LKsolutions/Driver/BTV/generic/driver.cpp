// COMPANY:   LKsolutions Inc.
//
// the	enterprise	for		humanbeings
// the	technology	for		humanbeings
// the	hope		for		humanbeings

//
// products line up:
// Multimedia Communication with 3D technology

// 
// code by : Jongwhan Lee (leejw51@hitel.net) ; Lead Programmer 
// website: superfrontier.com

// 2002. 8. 7   : basic frame
//       8.8    : plug & play, INF file


#include "driver.h"

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

NTSTATUS DispatchPnp(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS PassdownPnp(IN PDEVICE_OBJECT , IN PIRP);
NTSTATUS HandleStartDevice(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS HandleStopDevice(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS HandleRemoveDevice(IN PDEVICE_OBJECT , IN PIRP );

NTSTATUS DispatchPower(IN PDEVICE_OBJECT , IN PIRP );
NTSTATUS DispatchShutdown(IN PDEVICE_OBJECT , IN PIRP );



//Global Variables
int g_ulDeviceNumber=0; // number of devices



extern "C" NTSTATUS DriverEntry(
			IN PDRIVER_OBJECT pDriverObject,
			IN PUNICODE_STRING pRegistryPath)
{

#ifdef _DEBUG
	_asm int 3	
#endif
	NTSTATUS status=STATUS_SUCCESS;

	// call-back functions
	pDriverObject->DriverUnload= DriverUnload;
	pDriverObject->DriverExtension->AddDevice= AddDevice;

	pDriverObject->MajorFunction[IRP_MJ_CREATE]= DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE]=DispatchClose;
	pDriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnp;
	pDriverObject->MajorFunction[IRP_MJ_POWER] = DispatchPower;
	pDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = DispatchShutdown;


	return status;
}


////////////////////////////////////////////////////////////////////////////////////////////////
VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
#ifdef _DEBUG
	//_asm int 3
#endif
	
	PDEVICE_OBJECT pNextObj;
	pNextObj=pDriverObject->DeviceObject;

	while(pNextObj!=NULL) {
		PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pNextObj->DeviceExtension;
		

		UNICODE_STRING pLinkName=pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);


		pNextObj=pNextObj->NextDevice;
		IoDeleteDevice(pDevExt->pDevice);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	pIrp->IoStatus.Information=0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
NTSTATUS DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PDEVICE_EXTENSION pDevExt= (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	

	pIrp->IoStatus.Status=STATUS_SUCCESS;
	pIrp->IoStatus.Information= 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////

// FDO : Functional Device Object
// PDO : Physical Device Object
NTSTATUS AddDevice (IN PDRIVER_OBJECT pDriverObject, IN PDEVICE_OBJECT pPhysicalDeviceObject)
{
#ifdef _DEBUG
	//_asm int 3
#endif
	NTSTATUS status;
	PDEVICE_OBJECT pfdo;
	PDEVICE_EXTENSION pDevExt;
	CUString devName("\\Device\\LKbtv");
	devName+=CUString(g_ulDeviceNumber); // zero base number

	// CREATE FUNCTIONAL DEVICE OBJECT
	status=IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
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


	// make a notice that FDO was created outside of DriverEntry
	pfdo->Flags &= ~DO_DEVICE_INITIALIZING;

	// increate device nunmber
	g_ulDeviceNumber++;


	
	return STATUS_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Plug & Play
NTSTATUS DispatchPnp(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	PIO_STACK_LOCATION pIrpStack;
	pIrpStack=IoGetCurrentIrpStackLocation(pIrp);
	switch(pIrpStack->MinorFunction)
	{
		case IRP_MN_START_DEVICE:
		return HandleStartDevice(pDevObj, pIrp);
		
		case IRP_MN_STOP_DEVICE:
		return HandleStopDevice(pDevObj, pIrp);

		case IRP_MN_REMOVE_DEVICE:
		return HandleRemoveDevice(pDevObj, pIrp);
		
		default:
		return PassdownPnp(pDevObj, pIrp);

	}
}

NTSTATUS PassdownPnp(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	IoSkipCurrentIrpStackLocation(pIrp);
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	return IoCallDriver(pDevExt->pLowerDevice, pIrp);
}

NTSTATUS HandleStartDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	#ifdef _DEBUG
	//_asm int 3
	#endif

	PassdownPnp(pDevObj, pIrp);
	return STATUS_SUCCESS;
}

NTSTATUS HandleStopDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	
	PassdownPnp(pDevObj, pIrp);
	return STATUS_SUCCESS;
}

NTSTATUS HandleRemoveDevice(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	#ifdef _DEBUG
	//_asm int 3
	#endif

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

NTSTATUS DispatchPower(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	#ifdef _DEBUG
	_asm int 3
	#endif
	
	PDEVICE_EXTENSION pDevExt=(PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	NTSTATUS status=STATUS_SUCCESS;
	
	PIO_STACK_LOCATION stack=IoGetCurrentIrpStackLocation(pIrp);
	ULONG fcn=stack->MinorFunction;

	if (fcn==IRP_MN_SET_POWER ||
		fcn==IRP_MN_QUERY_POWER) {

	}
	else {
		if (fcn==IRP_MN_WAIT_WAKE) 
		{
			IoCopyCurrentIrpStackLocationToNext(pIrp);
			IoSetCompletionRoutine(
				pIrp,(PIO_COMPLETION_ROUTINE)WaitWakeCompletionRoutine,
				pDevObj, TRUE, TRUE, TRUE);
			PoStartNextPowerIrp(pIrp);
			status=PoCallDriver(pDevExt->pLowerDevice, pIrp);
		}
		else {
			// default power handler			
			PoStartNextPowerIrp(pIrp);
			IoSkipCurrentIrpStackLocation(pIrp);
			return PoCallDriver(pDevExt->pLowerDevice, pIrp);
		}
	}

	
	return status;
}