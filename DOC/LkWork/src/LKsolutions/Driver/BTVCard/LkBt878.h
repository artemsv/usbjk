// LKsolutions
// company for humanbegings
// technology for humanbeings
//
// coded by Jongwhan Lee
// 2002. 8. 19
// this is for interface with bt878 kernel mode driver
// it's for function 0 for bt878 chip.
// this class can have state-machine.


#ifndef LK_SOLUTIONS_BT878_INTERFACE_CLASS_FOR_FUNCTION_0_VIDEO_PART
#define LK_SOLUTIONS_BT878_INTERFACE_CLASS_FOR_FUNCTION_0_VIDEO_PART


class LkBt878  
{
public:
	ULONG GetAddr();
	HANDLE m_hDevice; // device

	ULONG result;
	void RegisterEvent(HANDLE E);
	void FinishCapture();
	void StartCapture();
	LkBt878();
	virtual ~LkBt878();

};

#endif 
