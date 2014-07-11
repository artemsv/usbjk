#include "ntddk.h"
#include "wdf.h"
#include "prototypes.h"
#pragma warning(disable:4200)  // suppress nameless struct/union warning
#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4214)  // suppress bit field types other than int warning
#include "usbdi.h"
#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#include "wdfusb.h"
#include "initguid.h"

typedef struct _DEVICE_CONTEXT {

	WDFUSBDEVICE		UsbDevice;
	WDFUSBPIPE          BulkReadPipe;
	WDFUSBPIPE          BulkWritePipe;
	WDFUSBINTERFACE		UsbInterface;
	WDFUSBPIPE			InterruptPipe;
	UCHAR				Flag;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

NTSTATUS USBJKEvtDeviceAdd(IN WDFDRIVER	Driver, IN PWDFDEVICE_INIT DeviceInit);

NTSTATUS USBJKEvtDevicePrepareHardware(IN WDFDEVICE Device, 
		IN WDFCMRESLIST ResourceList,IN WDFCMRESLIST ResourceListTranslated);

VOID USBJKEvtIoDeviceControl(IN WDFQUEUE Queue,IN WDFREQUEST Request,
			IN size_t OutputBufferLength,IN size_t InputBufferLength,
			IN ULONG IoControlCode);

NTSTATUS USBJKConfigContReaderForInterruptEndPoint(
		PDEVICE_CONTEXT DeviceContext);

VOID USBJKEvtUsbInterruptPipeReadComplete(WDFUSBPIPE Pipe, WDFMEMORY Buffer,
							size_t NumBytesTransferred,WDFCONTEXT  Context);

NTSTATUS USBJKEvtDeviceD0Entry(IN  WDFDEVICE Device, 
							   IN  WDF_POWER_DEVICE_STATE PreviousState);

NTSTATUS USBJKEvtDeviceD0Exit(IN  WDFDEVICE Device,
							  IN  WDF_POWER_DEVICE_STATE TargetState);

PCHAR DbgDevicePowerString(IN WDF_POWER_DEVICE_STATE Type);

NTSTATUS EvtDeviceMyIrpPreprocess (IN WDFDEVICE Device, IN OUT PIRP Irp);

PCHAR PnPMinorFunctionString(UCHAR MinorFunction);

VOID USBJKEvtUsbInterruptPipeReadComplete(WDFUSBPIPE  Pipe, WDFMEMORY   Buffer,
						size_t NumBytesTransferred, WDFCONTEXT  Context);
NTSTATUS GetUSBInfo(PDEVICE_CONTEXT pDevContext, WDFUSBDEVICE usbdev, 
					WDFREQUEST Request, PULONG bytesTransferred);

NTSTATUS USBJKConfigContReaderForInterruptEndPoint(
	PDEVICE_CONTEXT DeviceContext);

NTSTATUS USBJKEvtDeviceD0Entry(WDFDEVICE Device,IN  
							   WDF_POWER_DEVICE_STATE PreviousState);

PCHAR DbgDevicePowerString(WDF_POWER_DEVICE_STATE Type);

NTSTATUS USBJKEvtDeviceD0Exit(WDFDEVICE Device,IN  
							  WDF_POWER_DEVICE_STATE TargetState);

VOID USBJKEvtIoRead(IN WDFQUEUE Queue, IN WDFREQUEST Request, IN size_t Length);   
VOID
EvtRequestReadCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target,
		PWDF_REQUEST_COMPLETION_PARAMS CompletionParams, IN WDFCONTEXT Context);
VOID USBJKEvtIoWrite(IN WDFQUEUE Queue,IN WDFREQUEST Request,IN size_t Length);
VOID
EvtRequestWriteCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target,
		PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,IN WDFCONTEXT Context);

VOID USBJKEvtIoStop(IN WDFQUEUE Queue, IN WDFREQUEST Request,
					IN ULONG ActionFlags);
VOID USBJKEvtDeviceFileCreate ( IN WDFDEVICE Device,
								 IN WDFREQUEST Request, IN WDFFILEOBJECT FileObject);

VOID USBJKEvtFileClose (IN WDFFILEOBJECT FileObject);

#define IOCTL_INDEX                0x800
#define FILE_DEVICE_USBJK          65500
#define IOCTL_USBJK_SET_BAR_GRAPH_DISPLAY CTL_CODE(FILE_DEVICE_USBJK,\
	IOCTL_INDEX + 2, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_USBJK_SAYHELLO				CTL_CODE(FILE_DEVICE_USBJK,\
	IOCTL_INDEX, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_USBJK_SAYGOODBAY				CTL_CODE(FILE_DEVICE_USBJK,\
	IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_USBJK_GETUSBINFO				CTL_CODE(FILE_DEVICE_USBJK,\
	IOCTL_INDEX + 3, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_USBJK_DEBUGBREAK				CTL_CODE(FILE_DEVICE_USBJK,\
	IOCTL_INDEX + 4, METHOD_BUFFERED, FILE_WRITE_ACCESS)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

#define PARAM_HELLO						0
#define PARAM_ECHO						1
#define PARAM_GET_DEVICEDESCRIPTOR		2
#define PARAM_GET_STRING				3
#define PARAM_GET_CONFIG				4
#define PARAM_GET_FLAG					5
#define USBJK_TRANSFER_BUFFER_SIZE		10