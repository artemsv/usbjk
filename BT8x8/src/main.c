/*++

Step1: This step shows how to create a simplest functional driver. 
       It only registers DriverEntry and EvtDeviceAdd callback.
       Framework provides default behaviour for everything else.
       This allows you to install and uninstall this driver.
--*/

#include "ntddk.h"
#include "wdf.h"
//#include "prototypes.h"

#include <initguid.h> // required for GUID definitions
#include <wdmguid.h> // required for WMILIB_CONTEXT
#include <wmistr.h>
#include <wmilib.h>
#include <ntintsafe.h>

#include "main.h"

//#include "main.tmh"

// {2A70561E-5ED6-4e7d-ADAF-7ACFC4F97F10}
static const GUID GUID_DEVINTERFACE_BT8x8 = 
	{0x2a70561e,0x5ed6, 0x4e7d, {0xad,0xaf,0x7a, 0xcf, 0xc4, 0xf9, 0x7f, 0x10}};

/*
// {85F8FFE3-E517-4c83-877C-9000142CB694}
DEFINE_GUID(GUID_DEVINTERFACE_BT8x8, 
	0x85f8ffe3, 0xe517, 0x4c83, 0x87, 0x7c, 0x90, 0x0, 0x14, 0x2c, 0xb6, 0x94);
*/

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG       config;
    NTSTATUS                status;

    DbgPrint(("DriverEntry of Step1\n"));

    WDF_DRIVER_CONFIG_INIT(&config, Bt8xxEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                        RegistryPath,
                        WDF_NO_OBJECT_ATTRIBUTES, 
                        &config,     
                        WDF_NO_HANDLE 
                        );

    if (!NT_SUCCESS(status)) {
		DbgPrint("DriverEntry WdfDriverCreate ERROR");

       // KdPrint(("WdfDriverCreate failed 0x%x\n", status));
    }

	DbgPrint("DriverEntry WdfDriverCreate OK");

    return status;
}

NTSTATUS Bt8xxEvtDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT  DeviceInit)
{
    NTSTATUS                            status;
    WDFDEVICE                           device;
	WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;
	PFDO_DATA                       fdoData = NULL;
	WDF_OBJECT_ATTRIBUTES           fdoAttributes;
    
    UNREFERENCED_PARAMETER(Driver);

	//
	// I/O type is Buffered by default. If required to use something else,
	// call WdfDeviceInitSetIoType with the appropriate type.
	//
	WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoDirect);

	//
	// Zero out the PnpPowerCallbacks structure.
	//
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

	pnpPowerCallbacks.EvtDevicePrepareHardware = Bt8xxEvtDevicePrepareHardware;
	pnpPowerCallbacks.EvtDeviceReleaseHardware = Bt8xxEvtDeviceReleaseHardware;

	pnpPowerCallbacks.EvtDeviceD0Entry = Bt8xxEvtDeviceD0Entry;
	pnpPowerCallbacks.EvtDeviceD0Exit  = Bt8xxEvtDeviceD0Exit;

	//
	// These next two callbacks are for doing work at PASSIVE_LEVEL (low IRQL)
	// after all the interrupts are connected and before they are disconnected.
	//
	// Some drivers need to do device initialization and tear-down while the
	// interrupt is connected.  (This is a problem for these devices, since
	// it opens them up to taking interrupts before they are actually ready
	// to handle them, or to taking them after they have torn down too much
	// to be able to handle them.)  While this hardware design pattern is to
	// be discouraged, it is possible to handle it by doing device init and
	// tear down in these routines rather than in EvtDeviceD0Entry and
	// EvtDeviceD0Exit.
	//
	// In this sample these callbacks don't do anything.
	//

	//pnpPowerCallbacks.EvtDeviceD0EntryPostInterruptsEnabled = NICEvtDeviceD0EntryPostInterruptsEnabled;
	//pnpPowerCallbacks.EvtDeviceD0ExitPreInterruptsDisabled = NICEvtDeviceD0ExitPreInterruptsDisabled;

	//
	// This next group of five callbacks allow a driver to become involved in
	// starting and stopping operations within a driver as the driver moves
	// through various PnP/Power states.  These functions are not necessary
	// if the Framework is managing all the device's queues and there is no
	// activity going on that isn't queue-based.  This sample provides these
	// callbacks because it uses watchdog timer to monitor whether the device
	// is working or not and it needs to start and stop the timer when the device
	// is started or removed. It cannot start and stop the timers in the D0Entry
	// and D0Exit callbacks because if the device is surprise-removed, D0Exit
	// will not be called.
	//
	//pnpPowerCallbacks.EvtDeviceSelfManagedIoInit    = PciDrvEvtDeviceSelfManagedIoInit;
	//pnpPowerCallbacks.EvtDeviceSelfManagedIoCleanup = PciDrvEvtDeviceSelfManagedIoCleanup;
	//pnpPowerCallbacks.EvtDeviceSelfManagedIoSuspend = PciDrvEvtDeviceSelfManagedIoSuspend;
	//pnpPowerCallbacks.EvtDeviceSelfManagedIoRestart = PciDrvEvtDeviceSelfManagedIoRestart;

	//
	// Register the PnP and power callbacks. Power policy related callbacks will be registered
	// later.
	//
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, FDO_DATA);

	fdoAttributes.EvtCleanupCallback = Bt8xxEvtDeviceContextCleanup;

	status = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &device);
	
	if (!NT_SUCCESS(status))
	{
		DbgPrint("EvtDeviceAdd failed 0x%x\n", status);

        return status;
    }

	fdoData = FdoGetData(device);
	fdoData->WdfDevice = device;

	DbgPrint("PDO(0x%p) FDO(0x%p), Lower(0x%p) DevExt (0x%p)\n",
		WdfDeviceWdmGetPhysicalDevice (device),
		WdfDeviceWdmGetDeviceObject (device),
		WdfDeviceWdmGetAttachedDevice(device), fdoData);

	status = Bt8xxAllocateSoftwareResources(fdoData);

	status = WdfDeviceCreateDeviceInterface(device,
		(LPGUID) &GUID_DEVINTERFACE_BT8x8,	NULL);

	if (!NT_SUCCESS (status)) 
	{
		DbgPrint("WdfDeviceCreateDeviceInterface failed %!STATUS!\n", status);
		return status;
	}

	//status = PciDrvWmiRegistration(device);

	DbgPrint("EvtDeviceAdd");

    return status;
}


NTSTATUS Bt8xxEvtDevicePrepareHardware (WDFDEVICE Device,
				WDFCMRESLIST Resources, WDFCMRESLIST ResourcesTranslated)
{
	PFDO_DATA               fdoData = NULL;
	NTSTATUS                status;

	fdoData = FdoGetData(Device);
	
	DbgPrint("Bt8xxEvtDevicePrepareHardware");

	Bt8xxMapHWResources(fdoData, ResourcesTranslated);

	return STATUS_SUCCESS;
}

NTSTATUS Bt8xxEvtDeviceReleaseHardware(IN  WDFDEVICE Device,
									   IN  WDFCMRESLIST ResourcesTranslated)
{
	DbgPrint("Bt8xxEvtDeviceReleaseHardware");

	return STATUS_SUCCESS;
}

NTSTATUS Bt8xxEvtDeviceD0Entry(IN  WDFDEVICE Device,
							   IN  WDF_POWER_DEVICE_STATE PreviousState)
{
	DbgPrint("Bt8xxEvtDeviceD0Entry");

	return STATUS_SUCCESS;
}

NTSTATUS Bt8xxEvtDeviceD0Exit(IN  WDFDEVICE Device,
							  IN  WDF_POWER_DEVICE_STATE TargetState)
{	
	DbgPrint("Bt8xxEvtDeviceD0Exit");

	return STATUS_SUCCESS;
}

VOID Bt8xxEvtDeviceContextCleanup(WDFDEVICE Device)
{
	PFDO_DATA               fdoData = NULL;
	NTSTATUS                status;

	PAGED_CODE();

	fdoData = FdoGetData(Device);

	DbgPrint("--> Bt8xxEvtDeviceContextCleanup\n");

	//status = NICFreeSoftwareResources(fdoData);

	DbgPrint("<-- Bt8xxEvtDeviceContextCleanup\n");

} 

NTSTATUS Bt8xxAllocateSoftwareResources(PFDO_DATA fdoData)
{

	//TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "Failed to get lookaside buffer\n");
 
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_INTERRUPT_CONFIG wic;
	WDFINTERRUPT Interrupt;
	WDF_OBJECT_ATTRIBUTES  interruptAttrb;
	WDF_DMA_ENABLER_CONFIG dmaConfig;
	WDF_IO_QUEUE_CONFIG ioQueueConfig;
	WDF_INTERRUPT_INFO wii;

	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.ParentObject = fdoData->WdfDevice;
	status = WdfSpinLockCreate(&attributes,&fdoData->Lock);
	if(!NT_SUCCESS(status))
		return status;

	WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig, WdfIoQueueDispatchParallel);

	ioQueueConfig.EvtIoDeviceControl = Bt8xxEvtIoDeviceControl;

	status = WdfIoQueueCreate(fdoData->WdfDevice, &ioQueueConfig,
		WDF_NO_OBJECT_ATTRIBUTES, &fdoData->IoctlQueue);

	if(!NT_SUCCESS (status))
	{
		DbgPrint("Error Creating ioctl Queue 0x%x\n", status);
		return status;
	}

	status = WdfDeviceConfigureRequestDispatching(fdoData->WdfDevice,
		fdoData->IoctlQueue, WdfRequestTypeDeviceControl);

	if(!NT_SUCCESS (status)){
		ASSERT(NT_SUCCESS(status));
		DbgPrint("Error in config'ing ioctl Queue 0x%x\n", status);
		return status;
	}

	status = WdfFdoQueryForInterface(fdoData->WdfDevice,
		&GUID_BUS_INTERFACE_STANDARD, (PINTERFACE) &fdoData->BusInterface,
		sizeof(BUS_INTERFACE_STANDARD), 1, NULL); //InterfaceSpecificData

	if (!NT_SUCCESS(status))
		return status;

	WDF_INTERRUPT_CONFIG_INIT(&wic, Bt8xxInterruptHandler, Bt8xxDpcForIsr);
	//WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&interruptAttrb, INTERRUPT_DATA);

	status = WdfInterruptCreate(fdoData->WdfDevice, &wic, 
		WDF_NO_OBJECT_ATTRIBUTES, &fdoData->WdfInterrupt);
	if (!NT_SUCCESS(status))
		return status;

	WDF_INTERRUPT_INFO_INIT(&wii);
	WdfInterruptGetInfo(fdoData->WdfInterrupt, &wii);

	DbgPrint("wii.Irql = %X, wii.MessageAddress = %X, wii.MessageData = %X, \n\
			wii.MessageNumber = %X, wii.Mode = %X, wii.Polarity = %X, \n\
			wii.ShareDisposition = %X, wii.Size = %X, \n\
			wii.TargetProcessorSet = %X, wii.Vector = %X\n",
			wii.Irql, wii.MessageAddress, wii.MessageData, wii.MessageNumber,
			wii.Mode, wii.Polarity, wii.ShareDisposition, wii.Size, 
			wii.TargetProcessorSet, wii.Vector);

	return Bt8xxGetDeviceInfo(fdoData);
}

NTSTATUS Bt8xxGetDeviceInfo(PFDO_DATA fdoData)
{
	NTSTATUS            status = STATUS_SUCCESS;
	UCHAR               buffer[sizeof(PCI_COMMON_CONFIG)];
	PPCI_COMMON_CONFIG  pPciConfig = (PPCI_COMMON_CONFIG) buffer;
	USHORT              usPciCommand;
	ULONG               bytesRead =0;
	UCHAR				k;

	DbgPrint("---> Bt8xxGetDeviceInfo\n");

	bytesRead = fdoData->BusInterface.GetBusData(
		fdoData->BusInterface.Context,
		PCI_WHICHSPACE_CONFIG, //READ
		buffer,
		0,//FIELD_OFFSET(PCI_COMMON_CONFIG, VendorID),
		sizeof(PCI_COMMON_CONFIG));

	if (bytesRead != sizeof(PCI_COMMON_CONFIG)) {
		DbgPrint("GetBusData (BT8XX_PCI_HDR_LENGTH) failed =%d\n", bytesRead);
		return STATUS_INVALID_DEVICE_REQUEST;
	}

	//
	// Is this our device?
	//

	for (k =0; k < 50;k++)
		DbgPrint("b = %u", buffer[k]);

	usPciCommand = pPciConfig->Command;
	DbgPrint("VendorID = %X, DeviceID = %X, Command = %u, Status = %X\n\
			 RevisionID = %X, ProgIf = %X, SubClass = %X, BaseClass = %X\n\
			 CacheLineSize = %X, LatencyTimer = %X, HeaderType = %X, BIST = %X", 
		pPciConfig->VendorID, pPciConfig->DeviceID, pPciConfig->Command,
		pPciConfig->Status, pPciConfig->RevisionID, pPciConfig->ProgIf,
		pPciConfig->SubClass, pPciConfig->BaseClass, pPciConfig->CacheLineSize,
		pPciConfig->LatencyTimer, pPciConfig->HeaderType, pPciConfig->BIST);

	DbgPrint("usPciCommand = %u", usPciCommand);

	/*
	if (pPciConfig->VendorID != NIC_PCI_VENDOR_ID ||
		pPciConfig->DeviceID != NIC_PCI_DEVICE_ID)
	{
		TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT,
			"VendorID/DeviceID don't match - %x/%x\n",
			pPciConfig->VendorID, pPciConfig->DeviceID);
		//return STATUS_DEVICE_DOES_NOT_EXIST;

	}
	*/
	return STATUS_SUCCESS;
}

/*
хорезмская академия мамуна
что открыла что достигла какой вклад внесен и что дошло и ценится
кто внес вклад в ее развитие и как создавалась абду-фадл аср ахрип
какие произв были напечатаны и какие дошли
арабский ренессанс сколькой лет она просуществовала
*/

BOOLEAN Bt8xxInterruptHandler(IN WDFINTERRUPT Interrupt, IN ULONG  MessageID)
{
	return TRUE;
}

VOID Bt8xxDpcForIsr(IN WDFINTERRUPT  Interrupt, IN WDFOBJECT  AssociatedObject)
{

}

VOID Bt8xxEvtIoDeviceControl(IN WDFQUEUE  Queue, IN WDFREQUEST  Request,
		IN size_t OutputBufferLength, IN size_t InputBufferLength, 
		IN ULONG  IoControlCode)
{

}

NTSTATUS Bt8xxMapHWResources(IN OUT PFDO_DATA FdoData, 
							 WDFCMRESLIST   ResourcesTranslated)
/*++
Routine Description:

Gets the HW resources assigned by the bus driver and maps them
to system address space. Called during EvtDevicePrepareHardware.

Three base address registers are supported by the 8255x:
1) CSR Memory Mapped Base Address Register (BAR 0 at offset 10)
2) CSR I/O Mapped Base Address Register (BAR 1 at offset 14)
3) Flash Memory Mapped Base Address Register (BAR 2 at offset 18)

The 8255x requires one BAR for I/O mapping and one BAR for memory
mapping of these registers anywhere within the 32-bit memory address space.
The driver determines which BAR (I/O or Memory) is used to access the
Control/Status Registers.

Just for illustration, this driver maps both memory and I/O registers and
shows how to use READ_PORT_xxx or READ_REGISTER_xxx functions to perform
I/O in a platform independent basis. On some platforms, the I/O registers
can get mapped into memory space and your driver should be able to handle
this transparently.

One BAR is also required to map the accesses to an optional Flash memory.
The 82557 implements this register regardless of the presence or absence
of a Flash chip on the adapter. The 82558 and 82559 implement this
register only if a bit is set in the EEPROM. The size of the space requested
by this register is 1Mbyte, and it is always mapped anywhere in the 32-bit
memory address space.

Note: Although the 82558 only supports up to 64 Kbytes of Flash memory
and the 82559 only supports 128 Kbytes of Flash memory, the driver
requests 1 Mbyte of address space. Software should not access Flash
addresses above 64 Kbytes for the 82558 or 128 Kbytes for the 82559
because Flash accesses above the limits are aliased to lower addresses.

Arguments:

FdoData     Pointer to our FdoData
ResourcesTranslated     Pointer to list of translated resources passed
to EvtDevicePrepareHardware callback

Return Value:


None

--*/
{
	PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
	ULONG       i;
	NTSTATUS    status = STATUS_SUCCESS;
	BOOLEAN     bResPort      = FALSE;
	BOOLEAN     bResInterrupt = FALSE;
	BOOLEAN     bResMemory    = FALSE;
	ULONG       numberOfBARs  = 0;

	PAGED_CODE();

	for (i=0; i<WdfCmResourceListGetCount(ResourcesTranslated); i++) {

		descriptor = WdfCmResourceListGetDescriptor(ResourcesTranslated, i);

		if(!descriptor)
		{
			DbgPrint("WdfResourceCmGetDescriptor");
			return STATUS_DEVICE_CONFIGURATION_ERROR;
		}

		switch (descriptor->Type) {

		case CmResourceTypePort:
			//
			// We will increment the BAR count only for valid resources. We will
			// not count the private device types added by the PCI bus driver.
			//
			numberOfBARs++;

			DbgPrint("I/O mapped CSR: (%x) Length: (%d)\n",
				descriptor->u.Port.Start.LowPart, descriptor->u.Port.Length);

			//
			// The resources are listed in the same order the as
			// BARs in the config space, so this should be the second one.
			//
			if(numberOfBARs != 2) 
			{
				DbgPrint("I/O mapped CSR is not in the right order\n");
				status = STATUS_DEVICE_CONFIGURATION_ERROR;
				return status;
			}

			//
			// The port is in I/O space on this machine.
			// We should use READ_PORT_Xxx, and WRITE_PORT_Xxx routines
			// to read or write to the port.
			//
/*
			FdoData->IoBaseAddress = ULongToPtr(descriptor->u.Port.Start.LowPart);
			FdoData->IoRange = descriptor->u.Port.Length;
			//
			// Since all our accesses are USHORT wide, we will create an accessor
			// table just for these two functions.
			//
			FdoData->ReadPort = NICReadPortUShort;
			FdoData->WritePort = NICWritePortUShort;

			bResPort = TRUE;
			FdoData->MappedPorts = FALSE;
*/
			break;

		case CmResourceTypeMemory:

			numberOfBARs++;

			if(numberOfBARs == 1) 
			{
				DbgPrint("Memory mapped CSR:(%x:%x) Length:(%d)\n",
					descriptor->u.Memory.Start.LowPart,
					descriptor->u.Memory.Start.HighPart,
					descriptor->u.Memory.Length);
				//
				// Our CSR memory space should be 0x1000 in size.
				//
/*				ASSERT(descriptor->u.Memory.Length == 0x1000);

				FdoData->MemPhysAddress = descriptor->u.Memory.Start;
				FdoData->CSRAddress = MmMapIoSpace(
					descriptor->u.Memory.Start,
					NIC_MAP_IOSPACE_LENGTH,
					MmNonCached);
*/
				//DbgPrint("CSRAddress=%p\n", FdoData->CSRAddress);

				bResMemory = TRUE;

			} else if(numberOfBARs == 2)
			{
				DbgPrint("I/O mapped CSR in Memory Space: (%x) Length: (%d)\n",
					descriptor->u.Memory.Start.LowPart,
					descriptor->u.Memory.Length);
				//
				// The port is in memory space on this machine.
				// We should call MmMapIoSpace to map the physical to virtual
				// address, and also use the READ/WRITE_REGISTER_xxx function
				// to read or write to the port.
				//
/*
				FdoData->IoBaseAddress = MmMapIoSpace(
					descriptor->u.Memory.Start,
					descriptor->u.Memory.Length,
					MmNonCached);

				FdoData->ReadPort = NICReadRegisterUShort;
				FdoData->WritePort = NICWriteRegisterUShort;
				FdoData->MappedPorts = TRUE;
*/
				bResPort = TRUE;

			} else if(numberOfBARs == 3)
			{
				DbgPrint("Flash memory:(%x:%x) Length:(%d)\n",
					descriptor->u.Memory.Start.LowPart,
					descriptor->u.Memory.Start.HighPart,
					descriptor->u.Memory.Length);
				//
				// Our flash memory should be 1MB in size. Since we don't
				// access the memory, let us not bother mapping it.
				//
				//ASSERT(descriptor->u.Memory.Length == 0x100000);
			} else 
			{
				DbgPrint("Memory Resources are not in the right order\n");
				status = STATUS_DEVICE_CONFIGURATION_ERROR;
				return status;
			}

			break;

		case CmResourceTypeInterrupt:

			ASSERT(!bResInterrupt);

			bResInterrupt = TRUE;

			DbgPrint("Interrupt level: 0x%0x, Vector: 0x%0x\n",
				descriptor->u.Interrupt.Level,
				descriptor->u.Interrupt.Vector);

			break;

		case CmResourceTypeDevicePrivate:
			DbgPrint("CmResourceTypeDevicePrivate = %X, %X, %X",
				descriptor->u.DevicePrivate.Data[0],
				descriptor->u.DevicePrivate.Data[1],
				descriptor->u.DevicePrivate.Data[2]);
			break;
		default:
			//
			// This could be device-private type added by the PCI bus driver. We
			// shouldn't filter this or change the information contained in it.
			//
			DbgPrint("Unhandled resource type (0x%x)\n", descriptor->Type);
			break;
		}

	}

	//
	// Make sure we got all the 3 resources to work with.
	//
	if (!(bResPort && bResInterrupt && bResMemory)) 
	{
		status = STATUS_DEVICE_CONFIGURATION_ERROR;
	}

	//
	// Read additional info from NIC such as MAC address
	//
	/*
	status = NICReadAdapterInfo(FdoData);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}

	//
	// Test our adapter hardware
	//
	status = NICSelfTest(FdoData);
	if (status != STATUS_SUCCESS)
	{
		return status;
	}
*/
	return status;

}

NTSTATUS Bt8xxEvtInterruptEnable(IN WDFINTERRUPT Interrupt,
								 IN WDFDEVICE AssociatedDevice)
{
	return STATUS_SUCCESS;
}

NTSTATUS Bt8xxEvtInterruptDisable(IN WDFINTERRUPT Interrupt,
								 IN WDFDEVICE AssociatedDevice)
{
	return STATUS_SUCCESS;
}


