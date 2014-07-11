#define BT8XX_PCI_HDR_LENGTH	4

typedef struct _FDO_DATA
{
	ULONG                   Signature;       // must be PCIDRV_FDO_INSTANCE_SIGNATURE
	// beneath this device object.
	WDFDEVICE               WdfDevice;

	// HW Resources
	ULONG                   CacheFillSize;
	PULONG                  IoBaseAddress;
	ULONG                   IoRange;
	PHYSICAL_ADDRESS        MemPhysAddress;

	WDFINTERRUPT            WdfInterrupt;

	BOOLEAN                 MappedPorts;
//	PHW_CSR                 CSRAddress;
	BUS_INTERFACE_STANDARD  BusInterface;
	//PREAD_PORT              ReadPort;
	//PWRITE_PORT             WritePort;
	WDFDMAENABLER           WdfDmaEnabler;
	// spin locks for protecting misc variables
	WDFSPINLOCK         Lock;
	WDFQUEUE			IoctlQueue;


} FDO_DATA, *PFDO_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FDO_DATA, FdoGetData)

NTSTATUS Bt8xxEvtDevicePrepareHardware (WDFDEVICE Device,
		WDFCMRESLIST Resources, WDFCMRESLIST ResourcesTranslated);

NTSTATUS Bt8xxEvtDeviceReleaseHardware(IN  WDFDEVICE    Device,
		IN  WDFCMRESLIST ResourcesTranslated);

NTSTATUS Bt8xxEvtDeviceD0Entry(IN  WDFDEVICE Device,
					   IN  WDF_POWER_DEVICE_STATE PreviousState);

NTSTATUS Bt8xxEvtDeviceD0Exit(IN  WDFDEVICE Device,
					   IN  WDF_POWER_DEVICE_STATE TargetState);

NTSTATUS Bt8xxEvtDeviceAdd(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit);
VOID Bt8xxEvtDeviceContextCleanup(WDFDEVICE Device);
NTSTATUS Bt8xxAllocateSoftwareResources(PFDO_DATA fdoData);
NTSTATUS Bt8xxGetDeviceInfo(PFDO_DATA fdoData);
BOOLEAN Bt8xxInterruptHandler(IN WDFINTERRUPT Interrupt, IN ULONG MessageID);
VOID Bt8xxDpcForIsr(IN WDFINTERRUPT Interrupt, IN WDFOBJECT AssociatedObject);
VOID Bt8xxEvtIoDeviceControl(IN WDFQUEUE Queue,  IN WDFREQUEST Request,  
							 IN size_t OutputBufferLength, 
							 IN size_t InputBufferLength,  
							 IN ULONG IoControlCode );
NTSTATUS Bt8xxMapHWResources(IN OUT PFDO_DATA FdoData,   
							 WDFCMRESLIST ResourcesTranslated);
NTSTATUS Bt8xxEvtInterruptEnable(IN WDFINTERRUPT Interrupt,   
								 IN WDFDEVICE AssociatedDevice);
NTSTATUS Bt8xxEvtInterruptDisable(IN WDFINTERRUPT Interrupt,   
								  IN WDFDEVICE AssociatedDevice);