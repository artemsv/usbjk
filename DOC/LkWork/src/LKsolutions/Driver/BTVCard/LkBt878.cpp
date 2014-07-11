// LkBt878.cpp: implementation of the LkBt878 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LkBt878.h"
#include "..\BTV\ioctl.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LkBt878::LkBt878()
{

	m_hDevice=CreateFile("\\\\.\\LKbtv1",
		GENERIC_READ  | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, 0, NULL);
}

LkBt878::~LkBt878()
{
	CloseHandle(m_hDevice);
}

void LkBt878::StartCapture()
{
	int id=1; 

	DeviceIoControl(
		m_hDevice, IOCTL_TEST_START_CAPTURE,
		&id, sizeof(id), NULL, 0, &result, NULL);
}

void LkBt878::FinishCapture()
{
	DeviceIoControl(
		m_hDevice, IOCTL_TEST_FINISH_CAPTURE,
		NULL, 0, NULL, 0, &result, NULL);

}

void LkBt878::RegisterEvent(HANDLE E)
{
	DeviceIoControl(
		m_hDevice, IOCTL_TEST_REGISTEREVENT,
		(PVOID)&E, sizeof(HANDLE),
		NULL, 0, &result, NULL);
}  

ULONG LkBt878::GetAddr()
{	
	ULONG vaddr=0;
		DeviceIoControl(m_hDevice,
		IOCTL_TEST_GET_ADDR, 
		NULL, 0,
		(PVOID)&vaddr, sizeof(vaddr),
		&result, NULL);
	return vaddr;
}
