#include "stdafx.h"

#include <winioctl.h>
#include "..\BTV\ioctl.h"
using namespace std;

// global variables
HANDLE g_hCaptureEvent=NULL;

int g_bFinishThread=false;
HANDLE g_hFinishEvent=NULL;


// functions
void StartThread(void*);
void CallEvent(HANDLE hDevice);

  
// entry function
void Start();
void Finish();
void main()
{
	Start();

	HANDLE hDevice=CreateFile("\\\\.\\LKbtv1",
		GENERIC_READ  | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, 0, NULL);

	DWORD data[3];
	DWORD result;
	

	// give event handle
	DeviceIoControl(
		hDevice, IOCTL_TEST_REGISTEREVENT,
		(PVOID)&g_hCaptureEvent, sizeof(HANDLE),
		NULL, 0, &result, NULL);

	

	DWORD vaddr=0;
	DeviceIoControl(hDevice,
		IOCTL_TEST_GET_ADDR, 
		NULL, 0,
		(PVOID)&vaddr, sizeof(vaddr),
		&result, NULL);
	printf("starting address 0x%x\n", vaddr);

	// start thread
	_beginthread(StartThread, 0, NULL);


	ULONG ch=1;
	DeviceIoControl(
		hDevice, IOCTL_TEST_START_CAPTURE,
		&ch, sizeof(ch), NULL, 0, &result, NULL);

	string read;
	while(1)
	{
		cin>>read;
		if (read=="q") break;
		if (read=="a") CallEvent(hDevice);
	}


	g_bFinishThread=true;
	WaitForSingleObject(g_hFinishEvent, 60000);

	DeviceIoControl(
		hDevice, IOCTL_TEST_FINISH_CAPTURE,
		NULL, 0, NULL, 0, &result, NULL);

	CloseHandle(hDevice);

	
	Finish();
	cout<<"program finished"<<endl;
}  


  
  
void StartThread(void* pParam)
{
	int num=0;
	ULONG old_time=timeGetTime();

	cout<<"Start Thread"<<endl;
	while(!g_bFinishThread)
	{
		int obj=WaitForSingleObject(g_hCaptureEvent, 1000);
		if (obj==WAIT_OBJECT_0) {
			num++;
			ULONG new_time=timeGetTime();
			if (new_time-old_time>3000) {
				float t=(float)(new_time-old_time) / 1000.0;
				cout<<
					"NUMBER = "<<num<<"    "<<
					"TIME = "<<t<<"    "<<
					"fps  "<<(float)num / t<<endl;
				old_time=new_time;
				num=0;
			}
		}
		else 	cout<<"Waiting"<<endl;
	}
	cout<<"Finish Thread"<<endl;
	SetEvent(g_hFinishEvent);
}
void Start()
{
	g_hCaptureEvent=CreateEvent(NULL, FALSE, FALSE, NULL); // automatic
	g_hFinishEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
}
void Finish()
{
	CloseHandle(g_hCaptureEvent);
	CloseHandle(g_hFinishEvent);
}
void CallEvent(HANDLE hDevice)
{
	
	DWORD result;
	DeviceIoControl(
		hDevice, IOCTL_TEST_CALLEVENT,
		NULL, 0, // in
		NULL, 0, // out
		&result, NULL);
}