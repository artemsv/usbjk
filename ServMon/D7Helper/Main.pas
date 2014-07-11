{$O-}
unit Main;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes;

function InitHelper():  Integer; stdcall;
function OpenDevice():  Integer; stdcall;
function CloseDevice(): Integer; stdcall;
function GetIntState:   Integer; stdcall;

implementation

uses
	JwaWinType, SetupApi, JwaWinNT, JwaWinIoctl, USB100;

const
  FILE_DEVICE_USBJK          =  65500;
	IOCTL_INDEX                =  $800;

const
	GETUSBINFO									=		3;

const
	PARAM_HELLO									=		0;
	PARAM_ECHO									=		1;
	PARAM_GET_DEVICEDESCRIPTOR	=		2;
	PARAM_GET_STRING						=		3;
  PARAM_GET_CONFIG            =   4;
  PARAM_GET_FLAG              =   5;

var
	guid: TGUID = '{573E8C73-0CB4-4471-A1BF-FAB26C31D384}';
	hAVRDev: THandle;
  isHelperInitialized: boolean = false;


function CheckHelperInitialization(): boolean;
begin
  if not isHelperInitialized then
    OutputDebugString('Helper: DLL wasn''t initialized\d');

  Result := isHelperInitialized;
end;

function GetCTLCode(ioctlindex: integer): integer;
begin
	Result := CTL_CODE(FILE_DEVICE_USBJK, IOCTL_INDEX + ioctlindex,
		METHOD_BUFFERED, FILE_WRITE_ACCESS);
end;

function GetDevicePath(InterfaceGuid:
							PGUID; DevicePath: PCHAR; BufLen: DWORD): boolean;
var
	HardwareDeviceInfo: HDEVINFO;
	DeviceInterfaceData: SP_DEVICE_INTERFACE_DATA;
	DeviceInterfaceDetailData: PSPDeviceInterfaceDetailData;
	Length: DWORD;
	RequiredLength: DWORD;
	bResult: boolean;
	hr: HRESULT;
	lpMsgBuf : LPVOID;
	p, p1:pointer;
begin
	RequiredLength := 0;
	HardwareDeviceInfo := SetupDiGetClassDevs(
													 InterfaceGuid,
													 nil,
													 0{Application.MainForm.Handle},
													 (DIGCF_PRESENT or DIGCF_DEVICEINTERFACE));

	if HardwareDeviceInfo = Pointer(INVALID_HANDLE_VALUE) then
	begin
			//ShowMessage('SetupDiGetClassDevs failed!');
			result := FALSE;
			Exit;
	end;

	DeviceInterfaceData.cbSize := sizeof(SP_DEVICE_INTERFACE_DATA);

	bResult := SetupDiEnumDeviceInterfaces(HardwareDeviceInfo,
																						0,
																						InterfaceGuid^,
																						0,
																						DeviceInterfaceData);

	if bResult = FALSE then
	begin
			if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER or
												FORMAT_MESSAGE_FROM_SYSTEM or
												FORMAT_MESSAGE_IGNORE_INSERTS,
												nil,
												GetLastError(),
												MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
												@lpMsgBuf,
												0,
												nil
												) <> 0)  then
			begin
          OutputDebugString('SetupDiEnumDeviceInterfaces failed');
          OutputDebugString(PChar(lpMsgBuf));
					LocalFree(DWORD(lpMsgBuf));
			end;

			SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
			Result := FALSE;
			Exit;
	end;

	SetupDiGetDeviceInterfaceDetail(
			HardwareDeviceInfo,
			@DeviceInterfaceData,
			nil,
			0,
			RequiredLength,
			nil
			);

	DeviceInterfaceDetailData := PSPDeviceInterfaceDetailData(
											LocalAlloc(LMEM_FIXED, RequiredLength));

	if DeviceInterfaceDetailData = nil then
	begin
			SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
			//ShowMessage('Failed to allocate memory');
			Result := FALSE;
			Exit;
	end;

	DeviceInterfaceDetailData.cbSize := sizeof(TSPDeviceInterfaceDetailData);

	Length := RequiredLength;

	bResult := SetupDiGetDeviceInterfaceDetail(
								HardwareDeviceInfo,
								@DeviceInterfaceData,
								DeviceInterfaceDetailData,
								Length,
								RequiredLength,
								nil);

	if bResult = FALSE then
	begin
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER or
										FORMAT_MESSAGE_FROM_SYSTEM or
										FORMAT_MESSAGE_IGNORE_INSERTS,
										nil,
										GetLastError(),
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
										lpMsgBuf,
										0,
										nil
										);

			LocalFree(DWORD(lpMsgBuf));
			SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
//			MessageBox(Application.MainForm.Handle,
	//				'Error in SetupDiGetDeviceInterfaceDetail',PChar(lpMsgBuf),16);
			LocalFree(DWORD(DeviceInterfaceDetailData));
			Result := FALSE;
			Exit;
	end;

	Move(DeviceInterfaceDetailData.DevicePath, DevicePath^, 100);
	{
	if FAILED(hr) then
	begin
			SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
			LocalFree(DeviceInterfaceDetailData);
	end;
	}
	LocalFree(DWORD(DeviceInterfaceDetailData));

	Result := TRUE;

end;

function OpenDevice_(Synchronous: boolean): THandle;
var
	hDev: THandle;
	completeDeviceName: array[0..260] of char;
	g: TGUID;
begin
	g := guid;
	if not GetDevicePath(@g, @completeDeviceName[0], sizeof(completeDeviceName)) then
	begin
    OutputDebugString('Helper: OpenDevice_: GetDevicePath returns false');
		Result := INVALID_HANDLE_VALUE;
		Exit;
	end;

	//MessageBox(Application.MainForm.Handle, completeDeviceName, nil, 0);

	if Synchronous then

			hDev := CreateFile(completeDeviceName,
							GENERIC_WRITE or GENERIC_READ,
							FILE_SHARE_WRITE or FILE_SHARE_READ,
							nil, // default security
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL)
	 else

			hDev := CreateFile(completeDeviceName,
							GENERIC_WRITE or GENERIC_READ,
							FILE_SHARE_WRITE or FILE_SHARE_READ,
							nil, // default security
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL or FILE_FLAG_OVERLAPPED,
							NULL);
	

	if hDev = INVALID_HANDLE_VALUE then
  begin
    OutputDebugString('Helper: OpenDevice_: Failed to open the device, error - ');
    OutpuTDebugString(PCHar(IntToStr(GetLastError)));
  end;

	Result := hDev;

  hAVRDev := hDev;
end;

function OpenDevice(): Integer;stdcall;
begin
  Result := 0;

  OutputDebugString('Helper: OpenDevice: IN');

  if not CheckHelperInitialization() then
    Exit;

  if INVALID_HANDLE_VALUE = OpenDevice_(true) then
    Result := 0
  else
    Result := 1;

  OutputDebugString(PChar(('Helper: OpenDevice: OpenDevice_ returns ' + IntToStr(Result))));
end;
{
procedure Button1Click(Sender: TObject);
begin
	hAVRDev := OpenDevice_(true);
  if INVALID_HANDLE_VALUE <> hAVRDev then
  begin
    //evice connected';
  end;
end;
 }
function InitHelper(): Integer;stdcall;
begin
  if LoadSetupApi then
  begin
    Result := 1;
    isHelperInitialized := true;
  end else
    Result := 0;

//	LoadConfigManagerApi;
end;

procedure Button2Click(Sender: TObject);
var
	ctl: DWORD;
	inbuf, outbuf: array [0..512] of char;
begin
	if (hAVRDev = 0) then
	begin
		//ShowMessage('hAVRDev not valid');
		exit;
	end;

	ctl :=  GetCTLCode(0{cbxIoctlCode.ItemIndex});

	PDWORD(@inbuf)^ := 0{heParam.Value};
	PDWORD(integer(@inbuf) + 4)^ := 0{heParam2.Value};

	if DeviceIoControl(hAVRDev,
											 ctl,
											 @inbuf,           // Ptr to InBuffer
											 512,  // Length of InBuffer
											 @outbuf,         // Ptr to OutBuffer
											 512,            // Length of OutBuffer
											 ctl,       // BytesReturned
											 nil) = false then
	begin         // Ptr to Overlapped structure

			ctl := GetLastError();
			//printf("DeviceIoControl failed with error 0x%x\n", code);
			Exit;
	end;

//  if (heParam.Value = 5) and (cbxIoctlCode.ItemIndex = GETUSBINFO) then
//    Memo1.Lines.Add(IntToStr(byte(outbuf[0])));

  if outbuf[0] = #45 then Halt;
end;

function MakeDeviceIoControl(code,param1,param2:integer;outbuf:PChar):integer;
var
	ctl: DWORD;
	inbuf, outb: array [0..512] of char;
begin
	PDWORD(@inbuf)^ := param1;
	PDWORD(integer(@inbuf) + 4)^ := param2;

	if DeviceIoControl(hAVRDev,
											 code,
											 @inbuf,           // Ptr to InBuffer
											 512,  // Length of InBuffer
											 @outb,         // Ptr to OutBuffer
											 512,            // Length of OutBuffer
											 ctl,       // BytesReturned
											 nil) = false then
		Result := 0 else Result := ctl;

    Move(outb, outbuf^, 512);
end;
{
procedure TForm1.btnDevDescClick(Sender: TObject);
var
	outbuf: array[0..512] of char;
	udd: USB_DEVICE_DESCRIPTOR;
begin
	if 0 = MakeDeviceIoControl(GetCTLCode(GETUSBINFO),
		PARAM_GET_DEVICEDESCRIPTOR, 0, @outbuf) then Exit;

	Move(outbuf, udd, SizeOf(udd));

	Memo1.Lines.Add('Device descriptor');
	Memo1.Lines.Add(' ');
	Memo1.Lines.Add(
		Format('bDescriptorType = %d, ' + #13#10 +
					 'bcdUSB          = %d, ' + #13#10 +
					 'bDeviceClass    = %d, ' + #13#10 +
					 'bDeviceSubClass = %d, ' + #13#10 +
					 'bDeviceProtocol = %d, ' + #13#10 +
					 'bMaxPacketSize0 = %d, ' + #13#10 +
					 'dVendor         = 0x%X, ' + #13#10 +
					 'idProduct       = 0x%X, ' + #13#10 +
					 'bcdDevice       = %d, ' + #13#10 +
					 'iManufacturer   = %d, ' + #13#10 +
					 'iProduct        = %d, ' + #13#10 +
					 'iSerialNumber   = %d, ' + #13#10 +
					 'bNumConfigurations = %d',

		[udd.bDescriptorType, udd.bcdUSB, udd.bDeviceClass,
		udd.bDeviceSubClass, udd.bDeviceProtocol, udd.bMaxPacketSize0,
		udd.idVendor, udd.idProduct, udd.bcdDevice, udd.iManufacturer,
		udd.iProduct, udd.iSerialNumber, udd.bNumConfigurations]));

  Memo1.Lines.Add('------------------------------------------');

end;
}

{
procedure TForm1.btnDevStrClick(Sender: TObject);
var
	outbuf: array[0..512] of char;
  st: string;
  k, res: integer;
begin
  ZeroMemory(@outbuf, SizeOf(outbuf));
	res := MakeDeviceIoControl(GetCTLCode(GETUSBINFO),
		PARAM_GET_STRING, seStrIndex.Value, @outbuf);

  if res > 0 then
  begin
    outbuf[res] := #0;
    outbuf[res + 1] := #0;
  	Memo1.Lines.Add('Device string');

    st := WideCharToString(PWideChar(@outbuf));

  	Memo1.Lines.Add(st);
  end;
end;
}

{
procedure TForm1.btnConfigDescrClick(Sender: TObject);
var
	outbuf: array[0..512] of char;
	ucd: USB_CONFIGURATION_DESCRIPTOR;
  uid: USB_INTERFACE_DESCRIPTOR;
begin
	if 0 = MakeDeviceIoControl(GetCTLCode(GETUSBINFO),
		PARAM_GET_CONFIG, 0, @outbuf) then Exit;

	Move(outbuf, ucd, SizeOf(ucd));
  Memo1.Lines.Add('Configuration descriptor');
  Memo1.Lines.Add(' ');
  Memo1.Lines.Add(Format(
           'bLength             = %d, ' + #13#10 +
					 'bDescriptorType     = %d, ' + #13#10 +
					 'wTotalLength        = %d, ' + #13#10 +
					 'bNumInterfaces      = %d, ' + #13#10 +
					 'bConfigurationValue = %d, ' + #13#10 +
					 'iConfiguration      = %d, ' + #13#10 +
					 'bmAttributes        = %d, ' + #13#10 +
					 'MaxPower            = %d',

      [ucd.bLength, ucd.bDescriptorType, ucd.wTotalLength, ucd.bNumInterfaces,
       ucd.bConfigurationValue, ucd.iConfiguration, ucd.bmAttributes,
       ucd.MaxPower]));
  Memo1.Lines.Add('------------------------------------------');

  Move(Pointer(integer(@outbuf) + SizeOf(ucd))^, uid, SizeOf(uid));
  Memo1.Lines.Add('Interface descriptor');
  Memo1.Lines.Add(' ');
  Memo1.Lines.Add(Format(
           'bLength             = %d, ' + #13#10 +
					 'bDescriptorType     = %d, ' + #13#10 +
					 'bInterfaceNumber    = %d, ' + #13#10 +
					 'bAlternateSetting   = %d, ' + #13#10 +
					 'bNumEndpoints       = %d, ' + #13#10 +
					 'bInterfaceClass     = %d, ' + #13#10 +
					 'bInterfaceSubClass  = %d, ' + #13#10 +
					 'bInterfaceProtocol  = %d, ' + #13#10 +
					 'iInterface          = %d',
      [uid.bLength, uid.bDescriptorType, uid.bInterfaceNumber,
       uid.bAlternateSetting, uid.bNumEndpoints, uid.bInterfaceClass,
       uid.bInterfaceSubClass, uid.bInterfaceProtocol, uid.iInterface]));

end;
}
{
procedure TForm1.btnReadClick(Sender: TObject);
var
  buf: array[0..10] of char;
  res: DWORD;
  b: boolean;
begin
	if (hAVRDev = 0) then
	begin
		ShowMessage('hAVRDev not valid');
		exit;
	end;

	b := ReadFile(hAVRDev, buf, 1, res, nil);

  if not b then
  begin
    res := GetLastError();

  end;

  if buf[0] <> '3' then
    ShowMessage('Error');
end;
}
{
procedure TForm1.Timer1Timer(Sender: TObject);
var
	ctl: DWORD;
	inbuf, outbuf: array [0..512] of char;
begin
  if Label4.Visible then
    Label4.Hide else Label4.Show;

	if (hAVRDev = 0) then
		Exit;

	ctl :=  GetCTLCode(3);

	PDWORD(@inbuf)^ := 1;
	PDWORD(integer(@inbuf) + 4)^ := 0;

	if DeviceIoControl(hAVRDev,
											 ctl,
											 @inbuf,           // Ptr to InBuffer
											 512,  // Length of InBuffer
											 @outbuf,         // Ptr to OutBuffer
											 512,            // Length of OutBuffer
											 ctl,       // BytesReturned
											 nil) = false then
	begin         // Ptr to Overlapped structure

    Label4.Caption := 'Device not connected';
    Label4.Font.Color := clRed;

    ctl := GetLastError();
		//printf("DeviceIoControl failed with error 0x%x\n", code);
		Exit;
	end;

  if outbuf[0] <> #65 then
  begin
    Label4.Caption := 'Device not connected';
    Label4.Font.Color := clRed;
  end;

  if cbIntControl.Checked then
    GetIntState();
end;
}

function GetIntState: Integer;stdcall;
var
	ctl: DWORD;
	inbuf, outbuf: array [0..512] of char;
begin
  if not CheckHelperInitialization() then
  begin
    OutputDebugString('Helper: GetIntState: CheckHelperInitialization returns zero');
    Exit;
  end;

	if (hAVRDev = 0) then
	begin
		OutputDebugString('Helper: GetIntState: hAVRDev not valid');
		exit;
	end;

	PDWORD(@inbuf)^ := 5;
	//PDWORD(integer(@inbuf) + 4)^ := heParam2.Value;

	if DeviceIoControl(hAVRDev,
											 GetCTLCode(GETUSBINFO),
											 @inbuf,           // Ptr to InBuffer
											 512,  // Length of InBuffer
											 @outbuf,         // Ptr to OutBuffer
											 512,            // Length of OutBuffer
											 ctl,       // BytesReturned
											 nil) = false then
	begin         // Ptr to Overlapped structure

			OutputDebugString('Helper: GetIntState: DeviceIoControl failed with error = ');
      OutputDebugString(PChar(IntToStr(GetLastError())));
			Exit;
	end;

  Result := byte(outbuf[0]);
end;


function CloseDevice(): integer;
begin
  Result := 0;

  if hAVRDev = 0 then
  begin
    OutputDebugString('Helper: CloseDevice: Device is already closed or was not opened');
    Exit;
  end;

  if not CheckHelperInitialization() then
    Exit;

  if CloseHandle(hAVRDev) then
    OutputDebugString('Helper: CloseDevice: Device closed')
  else
    OutputDebugString(PChar('Helper: CloseDevice: Error while closing GLE = ' + IntToStr(GetLastError())));

  hAVRDev := 0;

  Result := 1;
end;

end.
