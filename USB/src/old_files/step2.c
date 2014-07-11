/*++

Step2: This steps shows:
       1) How to create a context with the WDFDEVICE object
       2) How to initialize the USB device.
       3) How to register an interface so that app can open 
          an handle to the device.
--*/

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

DEFINE_GUID(GUID_DEVINTERFACE_OSRUSBFX2, // Generated using guidgen.exe
   0x573e8c73, 0xcb4, 0x4471, 0xa1, 0xbf, 0xfa, 0xb2, 0x6c, 0x31, 0xd3, 0x84);
// {573E8C73-0CB4-4471-A1BF-FAB26C31D384}

typedef struct _DEVICE_CONTEXT {
  WDFUSBDEVICE      UsbDevice;
  WDFUSBINTERFACE   UsbInterface;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    WDF_DRIVER_CONFIG       config;
    NTSTATUS                status;

    DbgPrint(("DriverEntry of Step1\n"));

    WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                        RegistryPath,
                        WDF_NO_OBJECT_ATTRIBUTES, 
                        &config,     
                        WDF_NO_HANDLE 
                        );

    if (!NT_SUCCESS(status)) {
		DbgPrint("DriverEntry WdfDriverCreate ERROR");

        KdPrint(("WdfDriverCreate failed 0x%x\n", status));
    }
	DbgPrint("DriverEntry WdfDriverCreate OK");

    return status;
}

NTSTATUS
EvtDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    WDF_OBJECT_ATTRIBUTES               attributes;
    NTSTATUS                            status;
    WDFDEVICE                           device;
    PDEVICE_CONTEXT                     pDevContext;
    WDF_PNPPOWER_EVENT_CALLBACKS        pnpPowerCallbacks;
    
    UNREFERENCED_PARAMETER(Driver);

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = EvtDevicePrepareHardware;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceCreate failed 0x%x\n", status));
        DbgPrint("EvtDeviceAdd WdfDeviceCreate ERR");
        return status;
    }

    pDevContext = GetDeviceContext(device);

    status = WdfDeviceCreateDeviceInterface(device,
                                (LPGUID) &GUID_DEVINTERFACE_OSRUSBFX2,
                                NULL);// Reference String
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceCreateDeviceInterface failed 0x%x\n", status));
        DbgPrint("WdfDeviceCreateDeviceInterface failed 0x%x\n");
        return status;
    }

	DbgPrint("EvtDeviceAdd OK");
	
    return status;
}


NTSTATUS
EvtDevicePrepareHardware(
    IN WDFDEVICE    Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    )
{
    NTSTATUS                            status;
    PDEVICE_CONTEXT                     pDeviceContext;
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    pDeviceContext = GetDeviceContext(Device);

    status = WdfUsbTargetDeviceCreate(Device,
                                WDF_NO_OBJECT_ATTRIBUTES,
                                &pDeviceContext->UsbDevice);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfUsbTargetDeviceCreate failed 0x%x\n", status));        
		DbgPrint("WdfUsbTargetDeviceCreate ERR");                
        return status;
    }

    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

    status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->UsbDevice,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &configParams);
    if(!NT_SUCCESS(status)) {
        KdPrint(("WdfUsbTargetDeviceSelectConfig failed 0x%x\n", status));
        DbgPrint("WdfUsbTargetDeviceSelectConfig ERR");
        return status;
    }

    pDeviceContext->UsbInterface =  
                configParams.Types.SingleInterface.ConfiguredUsbInterface;

	DbgPrint("EvtDevicePrepareHardware OK");
    return status;
}

