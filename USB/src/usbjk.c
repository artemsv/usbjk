#include "usbjk.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, USBJKEvtDeviceAdd)
#pragma alloc_text(PAGE, USBJKEvtDevicePrepareHardware)
#pragma alloc_text(PAGE, USBJKEvtDeviceD0Entry)
#pragma alloc_text(PAGE, USBJKEvtDeviceD0Exit)
#endif

DEFINE_GUID(GUID_DEVINTERFACE_USBJK, // Generated using guidgen.exe
   0x573e8c73, 0xcb4, 0x4471, 0xa1, 0xbf, 0xfa, 0xb2, 0x6c, 0x31, 0xd3, 0x84);
// {573E8C73-0CB4-4471-A1BF-FAB26C31D384}

static const PCHAR IrpMjFuncDesc[] =
{
	"IRP_MJ_CREATE",					//0
	"IRP_MJ_CREATE_NAMED_PIPE",
	"IRP_MJ_CLOSE",
	"IRP_MJ_READ",
	"IRP_MJ_WRITE",
	"IRP_MJ_QUERY_INFORMATION",
	"IRP_MJ_SET_INFORMATION",
	"IRP_MJ_QUERY_EA",
	"IRP_MJ_SET_EA",
	"IRP_MJ_FLUSH_BUFFERS",
	"IRP_MJ_QUERY_VOLUME_INFORMATION",	//10
	"IRP_MJ_SET_VOLUME_INFORMATION",
	"IRP_MJ_DIRECTORY_CONTROL",
	"IRP_MJ_FILE_SYSTEM_CONTROL",
	"IRP_MJ_DEVICE_CONTROL",
	"IRP_MJ_INTERNAL_DEVICE_CONTROL",
	"IRP_MJ_SHUTDOWN",
	"IRP_MJ_LOCK_CONTROL",
	"IRP_MJ_CLEANUP",
	"IRP_MJ_CREATE_MAILSLOT",
	"IRP_MJ_QUERY_SECURITY",			//20
	"IRP_MJ_SET_SECURITY",
	"IRP_MJ_POWER",                    //0x16
	"IRP_MJ_SYSTEM_CONTROL",           //0x17
	"IRP_MJ_DEVICE_CHANGE",            //0x18
	"IRP_MJ_QUERY_QUOTA",              //0x19
	"IRP_MJ_SET_QUOTA",                //0x1a
	"IRP_MJ_PNP",                      //0x1b
	"IRP_MJ_PNP_POWER",                //IRP_MJ_PNP      // Obsolete....
	"IRP_MJ_MAXIMUM_FUNCTION",         //0x1b
};

NTSTATUS
DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG       config;
    NTSTATUS                status;

    DbgPrint(("DriverEntry\n"));

    WDF_DRIVER_CONFIG_INIT(&config, USBJKEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,RegistryPath,WDF_NO_OBJECT_ATTRIBUTES, 
                        &config, WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) 
        DbgPrint("WdfDriverCreate failed 0x%x\n", status);

    return status;
}

#define MJ_MAS_SIZE	10

UCHAR mjMas[MJ_MAS_SIZE] = {IRP_MJ_PNP, IRP_MJ_CREATE, IRP_MJ_CLOSE, IRP_MJ_READ,
					IRP_MJ_WRITE, IRP_MJ_QUERY_INFORMATION, 
					IRP_MJ_DEVICE_CONTROL, IRP_MJ_INTERNAL_DEVICE_CONTROL,
					IRP_MJ_POWER, IRP_MJ_SYSTEM_CONTROL};
/*
NTSTATUS RegisterPreprocessCallbacks(IN PWDFDEVICE_INIT  DeviceInit)
{
	NTSTATUS	status;
	ULONG		k;

	for (k = 0; k < MJ_MAS_SIZE; k++)
	{
		status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
			DeviceInit,
			EvtDeviceMyIrpPreprocess,
			mjMas[k],
			NULL, // pointer minor function table
			0); // number of entries in the table

		if (!NT_SUCCESS(status)) {
			DbgPrint("WdfDeviceInitAssignWdmIrpPreprocessCallback failed %!STATUS!\n",
				status);
			return status;
		}
	}

	return status;
}
*/
NTSTATUS CreateQueues(WDFDEVICE device)
{
	WDF_IO_QUEUE_CONFIG                 ioQueueConfig;
	WDFQUEUE                            queue;
	NTSTATUS                            status;

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
		WdfIoQueueDispatchParallel);

	ioQueueConfig.EvtIoDeviceControl = USBJKEvtIoDeviceControl;

	status = WdfIoQueueCreate(device,&ioQueueConfig,
		WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);

	if (!NT_SUCCESS(status)) {
		DbgPrint("WdfIoQueueCreate failed  %!STATUS!\n", status);
		return status;
	}

	//  READ QUEUE
	WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig, WdfIoQueueDispatchSequential);

	ioQueueConfig.EvtIoRead = USBJKEvtIoRead;
	ioQueueConfig.EvtIoStop = USBJKEvtIoStop;

	status = WdfIoQueueCreate(device, &ioQueueConfig,
		WDF_NO_OBJECT_ATTRIBUTES, &queue);

	if (!NT_SUCCESS (status)) 
	{
		DbgPrint("WdfIoQueueCreate failed 0x%x\n", status);
		return status;
	}

	status = WdfDeviceConfigureRequestDispatching(device,queue,
		WdfRequestTypeRead);

	if(!NT_SUCCESS (status))
	{
		ASSERT(NT_SUCCESS(status));
		DbgPrint("WdfDeviceConfigureRequestDispatching failed 0x%x\n", status);
		return status;
	}

	//  WRITE QUEUE
	WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig, WdfIoQueueDispatchSequential);

	ioQueueConfig.EvtIoWrite = USBJKEvtIoWrite;
	ioQueueConfig.EvtIoStop = USBJKEvtIoStop;

	status = WdfIoQueueCreate(device, &ioQueueConfig,
		WDF_NO_OBJECT_ATTRIBUTES, &queue);

	if (!NT_SUCCESS (status)) 
	{
		DbgPrint("WdfIoQueueCreate failed 0x%x\n", status);
		return status;
	}

	status = WdfDeviceConfigureRequestDispatching(device,queue,
		WdfRequestTypeWrite);

	if(!NT_SUCCESS (status))
	{
		ASSERT(NT_SUCCESS(status));
		DbgPrint("WdfDeviceConfigureRequestDispatching failed 0x%x\n", status);
		return status;
	}

	return status;
}

NTSTATUS USBJKEvtDeviceAdd(IN WDFDRIVER Driver,IN PWDFDEVICE_INIT DeviceInit)
{
    WDF_OBJECT_ATTRIBUTES               attributes;
    NTSTATUS                            status;
    WDFDEVICE                           device;
    PDEVICE_CONTEXT                     pDevContext;
    WDF_PNPPOWER_EVENT_CALLBACKS        pnpPowerCallbacks;
	WDF_FILEOBJECT_CONFIG				fileConfig;
    
    UNREFERENCED_PARAMETER(Driver);
	
	//RegisterPreprocessCallbacks(DeviceInit);

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = USBJKEvtDevicePrepareHardware;
	pnpPowerCallbacks.EvtDeviceD0Entry = USBJKEvtDeviceD0Entry;
	pnpPowerCallbacks.EvtDeviceD0Exit = USBJKEvtDeviceD0Exit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

	WDF_FILEOBJECT_CONFIG_INIT(
		&fileConfig,
		USBJKEvtDeviceFileCreate,
		USBJKEvtFileClose,
		WDF_NO_EVENT_CALLBACK // not interested in Cleanup
		);

	WdfDeviceInitSetFileObjectConfig(DeviceInit,
		&fileConfig,
		WDF_NO_OBJECT_ATTRIBUTES);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        DbgPrint("WdfDeviceCreate failed 0x%x\n", status);
        return status;
    }

    pDevContext = GetDeviceContext(device);

    status = WdfDeviceCreateDeviceInterface(device,
                                (LPGUID) &GUID_DEVINTERFACE_USBJK,
                                NULL);// Reference String
    if (!NT_SUCCESS(status)) {
        DbgPrint("WdfDeviceCreateDeviceInterface failed 0x%x\n", status);
        return status;
    }

	CreateQueues(device);

	DbgPrint("EvtDeviceAdd - good\n");
 
	return status;
}


NTSTATUS USBJKEvtDevicePrepareHardware(IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList, IN WDFCMRESLIST ResourceListTranslated)
{
    USB_DEVICE_DESCRIPTOR				udd;
    WDF_USB_DEVICE_INFORMATION			wudi;
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     pDeviceContext;
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
    USHORT								len;
    PVOID								pv;
    WDFUSBPIPE                          pipe;
    WDF_USB_PIPE_INFORMATION            pipeInfo;
    UCHAR                               index;
    UCHAR                               numberConfiguredPipes;
 
    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    pDeviceContext = GetDeviceContext(Device);

    status = WdfUsbTargetDeviceCreate(Device,
                                WDF_NO_OBJECT_ATTRIBUTES,
                                &pDeviceContext->UsbDevice);
    if (!NT_SUCCESS(status)) {
        DbgPrint("WdfUsbTargetDeviceCreate failed 0x%x\n", status);        
        return status;
    }

    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

    status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->UsbDevice,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &configParams);
    if(!NT_SUCCESS(status)) {
        DbgPrint("WdfUsbTargetDeviceSelectConfig failed 0x%x\n", status);
        return status;
    }

    pDeviceContext->UsbInterface =  
                configParams.Types.SingleInterface.ConfiguredUsbInterface;        

    DbgPrint("EvtDevicePrepareHardware\n");
    
    DbgPrint("%d interfaces\n",
    	WdfUsbTargetDeviceGetNumInterfaces(pDeviceContext->UsbDevice));
    
	numberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;
	DbgPrint("numberConfiguredPipes = %d\n", numberConfiguredPipes);

	for(index=0; index < numberConfiguredPipes; index++) 
	{
		WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
        
		pipe = WdfUsbInterfaceGetConfiguredPipe(pDeviceContext->UsbInterface,
            index, &pipeInfo);

		WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

        if(WdfUsbPipeTypeInterrupt == pipeInfo.PipeType) 
		{    
            DbgPrint("Interrupt Pipe is 0x%p\n", pipe);        
            pDeviceContext->InterruptPipe = pipe;
        }else
		{
			DbgPrint("pipeInfo.PipeType = %d", pipeInfo.PipeType);
			status = STATUS_INVALID_DEVICE_STATE;
			return status;
		}
 
		if(!(pDeviceContext->InterruptPipe)) 
		{
			status = STATUS_INVALID_DEVICE_STATE;
			DbgPrint("Device is not configured properly %!STATUS!\n", status);
	        return status;
	    }
	}

    udd.bLength = sizeof(udd);
    
   // if (STATUS_SUCCESS == 
    WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->UsbDevice,
		NULL, &len);
    {
    	DbgPrint("len = %d\n", len);
    	pv = ExAllocatePoolWithTag(NonPagedPool, len, 'KOSA');
		if (pv)
	    if (STATUS_SUCCESS == 
    	WdfUsbTargetDeviceRetrieveConfigDescriptor(pDeviceContext->UsbDevice, pv, &len))
    	{/*
	   		DbgPrint("bDescriptorType = %d, wTotalLength = %d, bNumInterfaces = %d\n,\
			bConfigurationValue = %d, iConfiguration = %d, bmAttributes = %d, \n\
			MaxPower = %d\n",
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->bDescriptorType,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->wTotalLength,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->bNumInterfaces,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->bConfigurationValue,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->iConfiguration,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->bmAttributes,
    			((PUSB_CONFIGURATION_DESCRIPTOR)pv)->MaxPower);
		*/
		}
    }

    WDF_USB_DEVICE_INFORMATION_INIT(&wudi);
    WdfUsbTargetDeviceRetrieveInformation(pDeviceContext->UsbDevice, &wudi);
    /*
    DbgPrint("USBDI_Version = %X, Supported_USB_Version = %X\n", 
    	wudi.UsbdVersionInformation.USBDI_Version,
    	wudi.UsbdVersionInformation.Supported_USB_Version);
    */	
    status = USBJKConfigContReaderForInterruptEndPoint(pDeviceContext);
    
    if(!NT_SUCCESS(status)) {
        DbgPrint("USBJKConfigContReaderForInterruptEndPoint failed 0x%x\n", status);
        return status;
    }

	return status;
}

VOID USBJKEvtIoDeviceControl(IN WDFQUEUE Queue, IN WDFREQUEST Request,
    IN size_t     OutputBufferLength, IN size_t     InputBufferLength,
    IN ULONG      IoControlCode )
{
    WDFDEVICE                           device;
    PDEVICE_CONTEXT                     pDevContext;
    size_t                              bytesTransferred = 0;
    NTSTATUS                            status;
    WDF_USB_CONTROL_SETUP_PACKET        controlSetupPacket;
    WDF_MEMORY_DESCRIPTOR               memDesc;
    WDFMEMORY                           memory;
	PVOID								ee;
	UCHAR								tt;
   
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    
    device = WdfIoQueueGetDevice(Queue);
    pDevContext = GetDeviceContext(device);

    switch(IoControlCode) {

    case IOCTL_USBJK_SET_BAR_GRAPH_DISPLAY:

        if(InputBufferLength < sizeof(UCHAR)) {
            status = STATUS_BUFFER_OVERFLOW;
            bytesTransferred = sizeof(UCHAR);
            break;
        } 

        status = WdfRequestRetrieveInputMemory(Request, &memory);
        if (!NT_SUCCESS(status)) {
            DbgPrint("WdfRequestRetrieveMemory failed 0x%x\n", status);
            break;
        }

		ee = WdfMemoryGetBuffer(memory, NULL);
		if (ee)
		{
			tt = ((UCHAR*)ee)[0];
			DbgPrint("Param = %d\n", tt);
		}
		
			
        WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
                                        BmRequestHostToDevice,
                                        BmRequestToDevice,
                                        tt, // Request
                                        55, // Value
                                        0); // Index                                                        

        WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(&memDesc, memory, NULL);
    
		 
        status = WdfUsbTargetDeviceSendControlTransferSynchronously(
                                        pDevContext->UsbDevice, 
                                        NULL, // Optional WDFREQUEST
                                        NULL, // PWDF_REQUEST_SEND_OPTIONS
                                        &controlSetupPacket,

//A pointer to a caller-allocated WDF_MEMORY_DESCRIPTOR structure that 
//describes either an input or an output buffer, depending on the 
//device-specific command. This pointer is optional and can be NULL. 
//For more information, see the following Comments section. 

                                        &memDesc,	
                                        (PULONG)&bytesTransferred);
        if (!NT_SUCCESS(status)) {
            DbgPrint("SendControlTransfer failed 0x%x\n", status);
            break;
        }
		
		DbgPrint("PLAY_BAR\n");
        break;
	case IOCTL_USBJK_SAYHELLO:
		DbgPrint("Hello from Kernel mode!!!\n");
		break;
	case IOCTL_USBJK_SAYGOODBAY:
		DbgPrint("Bay-bay!!!\n");
		break;

	case IOCTL_USBJK_GETUSBINFO:
		status = GetUSBInfo(pDevContext, pDevContext->UsbDevice, 
			Request, &bytesTransferred);
		//DbgPrint("bytesTransferred = %d\n", bytesTransferred);
		break;
	case IOCTL_USBJK_DEBUGBREAK:
		DbgBreakPoint();
		break;
    default:
    	DbgPrint("EvtIoDeviceControl other control code %X\n", IoControlCode);
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, bytesTransferred);

    return;
}

NTSTATUS GetUSBInfo(PDEVICE_CONTEXT pDevContext, WDFUSBDEVICE usbdev, 
					WDFREQUEST Request, PULONG bytesTransferred)
{
	USB_DEVICE_DESCRIPTOR				udd;
	USB_CONFIGURATION_DESCRIPTOR		ucd;
	PVOID								ee, ee2;
    NTSTATUS                            status;
	ULONG								param, param2;
	USHORT								str[100];
	UCHAR								ucha;
	USHORT								len = sizeof(str);

	WdfUsbTargetDeviceGetDeviceDescriptor(usbdev, &udd);
/*
	DbgPrint("bDescriptorType = %d, bcdUSB = %d, bDeviceClass = %d,\n \
		bDeviceSubClass = %d, bDeviceProtocol = %d, bMaxPacketSize0 = %d,\n\
		idVendor = %X, idProduct = %X, bcdDevice = %d, iManufacturer = %d, \n\
        iProduct = %d, iSerialNumber = %d\n, bNumConfigurations = %d",

		udd.bDescriptorType, udd.bcdUSB, udd.bDeviceClass, 
		udd.bDeviceSubClass, udd.bDeviceProtocol, udd.bMaxPacketSize0, 
		udd.idVendor, udd.idProduct, udd.bcdDevice, udd.iManufacturer, 
		udd.iProduct, udd.iSerialNumber, udd.bNumConfigurations);		

	DbgPrint("NumInterfaces = %d", 
		WdfUsbTargetDeviceGetNumInterfaces(usbdev));
*/
	status = WdfRequestRetrieveInputBuffer(Request, 10, &ee, NULL);
	if (status != STATUS_SUCCESS)
		DbgPrint("Status = %d\n", status);

	if (ee)
	{
		param = ((ULONG*)ee)[0];
		param2 = ((ULONG*)ee)[1];
	}

	status = WdfRequestRetrieveOutputBuffer(Request, 255, &ee2, NULL);
	if ((NT_SUCCESS(status)) && (ee2))
	{
		switch (param)
		{
			case PARAM_HELLO:
				strcpy(ee2, "Hello\n");
				*bytesTransferred = 6;
				break;
			case PARAM_ECHO:
				((UCHAR*)ee2)[0] = 'A';		
				((UCHAR*)ee2)[1] = 0;
				*bytesTransferred = 2;
				break;
			case PARAM_GET_DEVICEDESCRIPTOR:
				WdfUsbTargetDeviceGetDeviceDescriptor(usbdev, &udd);
				memcpy(ee2, &udd, sizeof(udd));
				*bytesTransferred = sizeof(udd);
				break;
			case PARAM_GET_STRING:
				status = WdfUsbTargetDeviceQueryString(usbdev, Request, NULL, 
					(PUSHORT)&str, &len, (UCHAR)param2, 0);
				DbgPrint("Query String Status = %d\n", status);
				if (NT_SUCCESS(status))
				{
					memcpy(ee2, &str, len*2);
					*bytesTransferred = len*2;
					DbgPrint("String len = %d\n", len*2);
				}
				break;
			case PARAM_GET_CONFIG:
				len = 512;
				status = WdfUsbTargetDeviceRetrieveConfigDescriptor(usbdev, ee2, &len);
				if (NT_SUCCESS(status))
				{
					*bytesTransferred = len;
					DbgPrint("Config len = %d\n", len);
				}
				break;
			case PARAM_GET_FLAG:
				((UCHAR*)ee2)[0] = pDevContext->Flag;	
				DbgPrint("Flag2 = %d\n", pDevContext->Flag);
				*bytesTransferred = 1;
				break;
			default:
				break;
		}
	}

	return status;
}

NTSTATUS USBJKConfigContReaderForInterruptEndPoint(PDEVICE_CONTEXT DeviceContext)
{
    WDF_USB_CONTINUOUS_READER_CONFIG contReaderConfig;
    NTSTATUS status;

    WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&contReaderConfig,
                                          USBJKEvtUsbInterruptPipeReadComplete,
                                          DeviceContext,    // Context
                                          sizeof(UCHAR));   // TransferLength
    //
    // Reader requests are not posted to the target automatically.
    // Driver must explicitly call WdfIoTargetStart to kick start the
    // reader.  In this sample, it's done in D0Entry.
    // By default, framework queues two requests to the target
    // endpoint. Driver can configure up to 10 requests with CONFIG macro.
    //
    status = WdfUsbTargetPipeConfigContinuousReader(DeviceContext->InterruptPipe,
                                                    &contReaderConfig);

	DbgPrint("WdfUsbTargetPipeConfigContinuousReader = %d\n", status);

    if (!NT_SUCCESS(status)) {
        DbgPrint("USBJKConfigContReaderForInterruptEndPoint failed %x\n", status);
        return status;
    }

    return status;
}

VOID USBJKEvtUsbInterruptPipeReadComplete(WDFUSBPIPE  Pipe, WDFMEMORY   Buffer,
    size_t      NumBytesTransferred, WDFCONTEXT  Context)
{
    PUCHAR          flag = NULL;
    WDFDEVICE       device;
    PDEVICE_CONTEXT pDeviceContext = Context;

    UNREFERENCED_PARAMETER(NumBytesTransferred);
    UNREFERENCED_PARAMETER(Pipe);

    device = WdfObjectContextGetObject(pDeviceContext);

    ASSERT(NumBytesTransferred == sizeof(UCHAR));

    flag = WdfMemoryGetBuffer(Buffer, NULL);

//    DbgPrint("USBJKEvtUsbInterruptPipeReadComplete flag %x\n",
//                *flag);

    pDeviceContext->Flag = *flag;

}

NTSTATUS USBJKEvtDeviceD0Entry(WDFDEVICE Device,IN  
							   WDF_POWER_DEVICE_STATE PreviousState)
{
    PDEVICE_CONTEXT         pDeviceContext;
    NTSTATUS                status;

    PAGED_CODE();

    pDeviceContext = GetDeviceContext(Device);

    DbgPrint("-->USBJKEvtEvtDeviceD0Entry - coming from %s\n",
              DbgDevicePowerString(PreviousState));

    status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe));        

    DbgPrint("<--USBJKEvtEvtDeviceD0Entry\n");

    return status;
}

NTSTATUS USBJKEvtDeviceD0Exit(WDFDEVICE Device,IN  WDF_POWER_DEVICE_STATE TargetState)
{
    PDEVICE_CONTEXT         pDeviceContext;
    
    PAGED_CODE();

//    DbgPrint("-->USBJKEvtDeviceD0Exit - moving to %s\n", 
//        DbgDevicePowerString(TargetState));

    pDeviceContext = GetDeviceContext(Device);

    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe),   WdfIoTargetCancelSentIo);        

    DbgPrint("<--USBJKEvtDeviceD0Exit\n");

    return STATUS_SUCCESS;
}

NTSTATUS MyIrpCompletionRoutine(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,
								IN PVOID  Context)
{
	//UNREFERENCED_PARAMETER(Device);
	UNREFERENCED_PARAMETER(Irp);

	return STATUS_SUCCESS;
}
/*
NTSTATUS EvtDeviceMyIrpPreprocess (IN WDFDEVICE Device, IN OUT PIRP Irp)
{
	PIO_STACK_LOCATION   irpStack;	
 
	irpStack = IoGetCurrentIrpStackLocation(Irp);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	
	if (29 > irpStack->MajorFunction)

	DbgPrint("%s - %s\n", IrpMjFuncDesc[irpStack->MajorFunction],
		PnPMinorFunctionString(irpStack->MinorFunction));

	IoSkipCurrentIrpStackLocation(Irp);

	
//	IoSetCompletionRoutine(
//
//		Irp,
//		MyIrpCompletionRoutine,
//		NULL,
//		TRUE,
//		TRUE,
//		TRUE
//		);
	

	return WdfDeviceWdmDispatchPreprocessedIrp(Device, Irp);
}
*/
VOID USBJKEvtIoRead(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length)
{

	WDFUSBPIPE                  pipe;
	NTSTATUS                    status;
	WDFMEMORY                   reqMemory;
	PDEVICE_CONTEXT             pDeviceContext;
	PVOID						mem;
	size_t						len		

	UNREFERENCED_PARAMETER(Queue);

	DbgPrint("-->USBJKEvtIoRead\n");

	//
	// First validate input parameters.
	//
	if (Length > USBJK_TRANSFER_BUFFER_SIZE)
	{
		DbgPrint("Transfer exceeds %d\n",USBJK_TRANSFER_BUFFER_SIZE);
		status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}

	pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));

	pipe = pDeviceContext->BulkReadPipe;

	status = WdfRequestRetrieveOutputMemory(Request, &reqMemory);

	mem = WdfMemoryGetBuffer(reqMemory, NULL);
	((UCHAR*)mem)[0] = '3';
	WdfRequestCompleteWithInformation(Request, status, 1);
	return;

	if(!NT_SUCCESS(status))
	{
		DbgPrint("WdfRequestRetrieveOutputMemory failed %!STATUS!\n", status);
		goto Exit;
	}

	//
	// The format call validates to make sure that you are reading or
	// writing to the right pipe type, sets the appropriate transfer flags,
	// creates an URB and initializes the request.
	//
	status = WdfUsbTargetPipeFormatRequestForRead(pipe,Request,reqMemory,NULL); 
	if (!NT_SUCCESS(status)) 
	{
		DbgPrint("WdfUsbTargetPipeFormatRequestForRead failed 0x%x\n", status);
		goto Exit;
	}

	WdfRequestSetCompletionRoutine(Request,
		EvtRequestReadCompletionRoutine,pipe);
	//
	// Send the request asynchronously.
	//
	if (WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS) == FALSE) {
		//
		// Framework couldn't send the request for some reason.
		//
		DbgPrint("WdfRequestSend failed\n");
		status = WdfRequestGetStatus(Request);
		goto Exit;
	}


Exit:
	if (!NT_SUCCESS(status)) 
		WdfRequestCompleteWithInformation(Request, status, 0);

	DbgPrint("<-- USBJKEvtIoRead\n");

	return;
}

VOID
EvtRequestReadCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target,
								PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
								IN WDFCONTEXT Context)
{    
	NTSTATUS    status;
	size_t      bytesRead = 0;
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;

	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Context);

	status = CompletionParams->IoStatus.Status;

	usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

	bytesRead =  usbCompletionParams->Parameters.PipeRead.Length;

	if (NT_SUCCESS(status)){
		DbgPrint("Number of bytes read: %I64d\n", (INT64)bytesRead);        
	} else 
	{
		DbgPrint("Read failed - request status 0x%x UsbdStatus 0x%x\n", 
			status, usbCompletionParams->UsbdStatus);

	}

	WdfRequestCompleteWithInformation(Request, status, bytesRead);

	return;
}

VOID USBJKEvtIoWrite(IN WDFQUEUE Queue,IN WDFREQUEST Request,IN size_t Length)
{
	NTSTATUS                    status;
	WDFUSBPIPE                  pipe;
	WDFMEMORY                   reqMemory;
	PDEVICE_CONTEXT             pDeviceContext;
	PCHAR						pWriteBuf;

	UNREFERENCED_PARAMETER(Queue);

	DbgPrint("-->USBJKEvtIoWrite\n");

	//
	// First validate input parameters.
	//
	if (Length > USBJK_TRANSFER_BUFFER_SIZE) 
	{
		DbgPrint("Transfer exceeds %d\n", USBJK_TRANSFER_BUFFER_SIZE);
		status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}

	pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));

	pipe = pDeviceContext->BulkWritePipe;

	status = WdfRequestRetrieveInputMemory(Request, &reqMemory);
	if(!NT_SUCCESS(status))
	{
		DbgPrint("WdfRequestRetrieveInputBuffer failed\n");
		goto Exit;
	}

	status = WdfRequestRetrieveInputBuffer(Request, 5, &pWriteBuf, NULL);
	DbgPrint("Write = %s", pWriteBuf);
	WdfRequestCompleteWithInformation(Request, status, Length);
	return;

	status = WdfUsbTargetPipeFormatRequestForWrite(pipe,Request,reqMemory,NULL); // Offset

	if (!NT_SUCCESS(status)) 
	{
		DbgPrint("WdfUsbTargetPipeFormatRequest failed 0x%x\n", status);
		goto Exit;
	}

	WdfRequestSetCompletionRoutine(Request,
		EvtRequestWriteCompletionRoutine, pipe);

	//
	// Send the request asynchronously.
	//
	if (WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS) == FALSE) {
		//
		// Framework couldn't send the request for some reason.
		//
		DbgPrint("WdfRequestSend failed\n");
		status = WdfRequestGetStatus(Request);
		goto Exit;
	}

Exit:

	if (!NT_SUCCESS(status)) 
		WdfRequestCompleteWithInformation(Request, status, 0);

	DbgPrint("<-- USBJKEvtIoWrite\n");

	return;
}


VOID
EvtRequestWriteCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target,
		PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,IN WDFCONTEXT Context)
{
	NTSTATUS    status;
	size_t      bytesWritten = 0;
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;

	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Context);

	status = CompletionParams->IoStatus.Status;

	//
	// For usb devices, we should look at the Usb.Completion param.
	//
	usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

	bytesWritten =  usbCompletionParams->Parameters.PipeWrite.Length;

	if (NT_SUCCESS(status))
		DbgPrint("Number of bytes written: %I64d\n", (INT64)bytesWritten);        
	else 
		DbgPrint("Write failed: request Status 0x%x UsbdStatus 0x%x\n", 
			status, usbCompletionParams->UsbdStatus);
	
	WdfRequestCompleteWithInformation(Request, status, bytesWritten);

	return;
}

VOID USBJKEvtIoStop(IN WDFQUEUE Queue, IN WDFREQUEST Request, 
					IN ULONG ActionFlags)
/*++

Routine Description:

This callback is invoked on every in flight request when the device
is suspended or removed. Since our in flight read and write requests
are actually pending in the target device, we will just acknowledge
its presence. Until we acknowledge, complete, or re queue the requests
framework will wait before allowing the device suspend or remove to
proceed. When the underlying USB stack gets the request to suspend or
remove, it will fail all the pending requests.

Arguments:

Return Value:
None

--*/
{ 
	UNREFERENCED_PARAMETER(Queue);
	UNREFERENCED_PARAMETER(ActionFlags);

	if (ActionFlags == WdfRequestStopActionSuspend ) {
		WdfRequestStopAcknowledge(Request, FALSE); // Don't requeue
	} else if(ActionFlags == WdfRequestStopActionPurge) {
		WdfRequestCancelSentRequest(Request);
	}
	return;
}

VOID USBJKEvtDeviceFileCreate ( IN WDFDEVICE Device,
								 IN WDFREQUEST Request, IN WDFFILEOBJECT FileObject)
{
	NTSTATUS                    status = 0;

	DbgPrint("Hello from Create\n");

	WdfRequestComplete(Request, status);

	return;
}

VOID USBJKEvtFileClose (IN WDFFILEOBJECT FileObject)
{
	PAGED_CODE ();

	DbgPrint("Hello from Close\n");

	return;
}


