#ifndef LK_SOLUTIONS_HARDWARE_FOR_BT878_VIDEO_DRIVER
#define LK_SOLUTIONS_HARDWARE_FOR_BT878_VIDEO_DRIVER
// risc instructions
#define RISC_WRITE			SetBin(31, "0001")
#define RISC_WRITE123		SetBin(31,"1001" )
#define RISC_WRITE1S23		SetBin(31,"1011" )
#define RISC_WRITEC			SetBin(31,"0101" )
#define RISC_SKIP			SetBin(31,"0010" )
#define RISC_SKIP123		SetBin(31,"1010" )
#define RISC_JUMP			SetBin(31,"0111" )
#define RISC_SYNC			SetBin(31,"1000" )



// registers
#define REG_DSTATUS					0x000 // device status 
#define REG_IFORM					0x004 // input format 
#define REG_TDEC					0x008 // temporal decimation 
#define REG_E_CROP					0x00c // MSB cropping  - even field
#define REG_O_CROP					0x08c // " - odd field
#define REG_E_VDELAY_LO				0x090 // vertical delay  - even field
#define REG_O_VDELAY_LO				0x010 // " - odd field
#define REG_E_VACTIVE_LO			0x014 // vertical active  - even
#define REG_O_VACTIVE_LO			0x094 // " - odd
#define REG_E_HDELAY_LO				0x018 //horizontal delay  - even field
#define REG_O_HDELAY_LO				0x098 // " - odd field
#define REG_E_HACTIVE_LO			0x01c // horizontal active  - even
#define REG_O_HACTIVE_LO			0x09c // " - odd
#define REG_E_HSCALE_HI				0x020 // horizontal scaling upper byte - even
#define REG_O_HSCALE_HI				0x0a0 // " - odd
#define REG_E_HSCALE_LO				0x024 // horizontal scaling, lower byte
#define REG_O_HSCALE_LO				0x0a4
#define REG_BRIGHT					0x028 // bright
#define REG_E_CONTROL				0x02c // miscellaneous control 
#define REG_O_CONTROL				0x0ac
#define REG_CONTRAST_LO				0x030 // luma gain , lower byte
#define REG_SAT_U_LO				0x034 // chroma(U) gain , lower byte
#define REG_SAT_V_LO				0x038 // chroma(V) gain, lower byte
#define REG_HUE						0x03c // hue
#define REG_E_SCLOOP				0x040 // SC loop control 
#define REG_O_SCLOOP				0x0c0 
#define REG_WC_UP					0x044 // white crush up register
#define REG_OFORM					0x048 // output format 
#define REG_E_VSCALE_HI				0x04c // vertical scaling
#define REG_O_VSCALE_HI				0x0cc
#define REG_E_VSCALE_LO				0x050 // vertical scaling, lower byte
#define REG_O_VSCALE_LO				0x0d0
#define REG_TEST					0x054 // test control
#define REG_ADELAY					0x060 // AGC delay
#define REG_BDELAY					0x064 // burst delay
#define REG_ADC						0x068 // ADC interface
#define REG_E_VTC					0x06c // video timing control
#define REG_O_VTC					0x0ec
#define REG_SRESET					0x07c // software reset
#define REG_WC_DN					0x078 // white crush down
#define REG_TGLB					0x080 // timing generator load byte
#define REG_TGCTRL					0x084 // timing generator control
#define REG_VTOTAL_LO				0x0b0 // total line count
#define REG_VTOTAL_HI				0x0b4 
#define REG_COLOR_FMT				0x0d4 // color format 
#define REG_COLOR_CTL				0x0d8 // color control
#define REG_CAP_CTL					0x0dc // capture control
#define REG_VBI_PACK_SIZE			0x0e0 // VBI packet size
#define REG_VBI_PACK_DEL			0x0e4 // VBI packet size / delay
#define REG_FCAP					0x0e8 // field capture counter
#define REG_PLL_F_LO				0x0f0 // PLL reference multiplier
#define REG_PLL_F_HI				0x0f4 
#define REG_PLL_XCI					0x0f8 // integer pll-xcl
#define REG_DVSIF					0x0fc // digital video signal interface
#define REG_INT_STAT				0x100 // interrupt status
#define REG_INT_MASK				0x104 // interrupt mask
#define REG_GPIO_DMA_CTL			0x10c // GPIO and DMA control
#define REG_I2C						0x110 // I2C Data/Control
#define REG_RISC_STRT_ADD			0x114 // risc program address
#define REG_GPIO_OUT_EN				0x118 // GPIO output enable control
#define REG_RISC_COUNT				0x120 // risc program counter
#define REG_GPIO_DATA				0x200

//BITS
// device status  REG_STATUS
#define BIT_PRES		SetBin(7, "1") // video present? // 0 : not present, 1: present
#define BIT_HLOC		SetBin(6, "1") // device in hlock
#define BIT_STATUS_FIELD		SetBin(5, "1") // field status, odd, even 0: odd, 1:even
#define BIT_NUML		SetBin(4, "1") // number of lines , 0:525lines(NTSC), 1: 625 lines(PAL)
#define BIT_PLOCK		SetBin(2, "1") // 1: PLL is out of lock
#define BIT_LOF			SetBin(1, "1") // luma adc overflow
#define BIT_COF			SetBin(0, "1") // chroma adc overfloow
// input format
#define BIT_MUXSEL_0		SetBin(6, "10")
#define BIT_MUXSEL_1		SetBin(6, "11")
#define BIT_MUXSEL_2		SetBin(6, "01")
#define BIT_MUXSEL_3		SetBin(6, "00")
#define BIT_FORMAT_AUTO		SetBin(2, "000") // auto format detect
#define BIT_FORMAT_NTSC		SetBin(2, "001") // NTSC(M)
#define BIT_FORMAT_NTSCW	SetBin(2, "010") // NTSC w/o
// temporal decimation 
#define BIT_DEC_FIELD		SetBin(7, "1") // 0: decimate frames, 1: decimate fields
#define BIT_FLDALGIN		SetBin(6, "1"); // 0: start decimation on the odd field, 1: start decimation on the even field
#define BIT_DEC_RAT			SetBin(5, "000000"); // 0x00 : disable decimation
// msb cropping register
#define BIT_VDELAY_MSB		SetBin(7,"00"); // most significant two bits 
#define BIT_VACTIVE_MSB		SetBin(5,"01");
#define BIT_HDELAY_MSB		SetBin(3,"00");
#define BIT_HACTIVE_MSB		SetBin(1,"10");
// vertical delay , lower byte
#define BIT_VACTIVE_LO		SetBin(7, "11100000")
#define BIT_HDELAY_LO		SetBin(7, "01111000")
#define BIT_HACTIVE_LO		SetBin(7, "10000000")
#define BIT_HSCALE_HI		SetBin(7, "00000010")
#define BIT_HSCALE_LO		SetBin(7, "10101100")
#define BIT_BRIGHT			SetBin(7, "00000000")
#define BIT_LNOTCH			SetBin(7, "1") // 0: enable luman notch filter, 1: disable luma lotch filter
#define BIT_COMP			SetBin(6, "1") // 0: composite video, 1: y/c component video
#define BIT_LDEC			SetBin(5, "1") // 0: enable luma decimation , 1:  disable luma decimation
#define BIT_CBSENSE			SetBin(4, "1") // 0: normal Cb,Cr order, 1 : invert Cb,Cr
#define BIT_CON_MSB			SetBin(2, "1") // msb of luma gain value
#define BIT_SAT_U_MSB		SetBin(1, "1") // msb of chroma u gain value
#define BIT_SAT_V_MSB		SetBIn(0, "1") // msb of chroma v gain value
#define BIT_CONTRAST_LO		SetBin(7, "11011000")
#define BIT_SAT_U_LO		SetBin(7, "11111110")
#define BIT_SAT_V_LO		SetBin(7, "10110100")
#define BIT_HUE				SetBin(7, "00000000")
#define BIT_PEAK			SetBIn(7, "1") // 0: normal luma low pass filtering, 1: use luma peaking filters
#define BIT_CAGC			SetBin(6, "1") // 0: chroma agc disabled, 1: chroma agc enabled
#define BIT_CKILL			SetBin(5, "1") // 0: low color detection and removal disabeld , 1: enabled
#define BIT_HFILT			SetBin(4, "00") // auto
#define BIT_MAJS			SetBin(7, "11") // 3: automatic
#define BIT_UPCNT			SetBin(5, "1111") 
#define BIT_RANGE			SetBin(7, "1") // 0: normal operation, 1: full-range output
#define BIT_CORE			SetBin(6,"00") // no coring
#define BIT_VSFLDALIGN		SetBin(7, "1") 
#define BIT_COMB			SetBin(6, "1") //0: chroma comb disabled
#define BIT_INT				SetBin(5, "1")
#define BIT_VSCALE_HI		SetBin(4, "0000")
#define BIT_VSCALE_LO		SetBin(7, "00000000")
#define BIT_ADELAY			SetBin(7, "01110000")
#define BIT_BDELAY			SetBin(7, "01011101")
#define BIT_AGC_EN			SetBin(4, "1") // 0: agc enabled, 1: agc disabled
#define BIT_CLK_SLEEP		SetBin(3, "1") // 0: normal clock operation, 1: shut down the system clock
#define BIT_Y_SLEEP			SetBin(2, "1") // 0: normal y adc operation, 1: sleep y adc operation
#define BIT_C_SLEEP			SetBin(1, "1") // 0: normal c adc operation, 1: sleep c adc operation
#define BIT_CRUSH			SetBin(0, "1") // 0: non adaptive agc, 1: adaptive agc
#define BIT_HSFMT			SetBin(7, "1") // 0: HRESET is 64 clkx1 cycles wide, 1: 32 clkx1 cycles wide
#define BIT_VFILT			SetBin(2, "000")
#define BIT_VERTEN			SetBin(7, "1") // 0: normal operation, 1: enable vertical sync detection
#define BIT_WCFRAME			SetBin(6, "1") // 0: once per field , 1: once per frame
#define BIT_DNCNT			SetBin(5, "000000")
#define BIT_TGLB			SetBin(7, "00000000")
#define BIT_TGCKO			SetBin(6, "00")
#define BIT_TGCKI			SetBin(4, "00")
#define BIT_TGC_AI			SetBin(2, "1")
#define BIT_GPC_AR			SetBin(1, "1")
#define BIT_TGC_VM			SetBin(0, "1")
#define BIT_VTOTAL			SetBin(7, "00000000")
#define BIT_VTOTAL_HI		SetBin(1, "00")
#define BIT_COLOR_ODD_YUY2	SetBin(7, "0100")
#define BIT_COLOR_ODD_YV12	SetBin(7, "1000")
#define BIT_COLOR_EVEN_YUY2	SetBin(3, "0100")
#define BIT_COLOR_EVEN_YV12	SetBin(3, "1000")
#define BIT_EXT_FRMRATE		SetBin(7, "1") 
#define BIT_COLOR_BARS		SetBin(6, "1") // 1: enable color pattern
#define BIT_RGB_DED			SetBin(5, "1") // 0 : enable error diffucion for rgb 16/rgb 15
#define BIT_GAMMA			SetBin(4, "1")
#define BIT_WSWAP_ODD		SetBin(3, "1")
#define BIT_WSWAP_EVEN		SetBin(2, "1")
#define BIT_BSWAP_ODD		SetBin(1, "1")
#define BIT_BSWAP_EVEN		SetBin(0, "1")
#define BIT_DITH_FRAME		SetBin(4, "1")
#define BIT_CAPTURE_VBI_ODD	SetBin(3, "1")
#define BIT_CAPTURE_VBI_EVEN	SetBin(2, "1")
#define BIT_CAPTURE_ODD		SetBin(1, "1")
#define BIT_CAPTURE_EVEN	SetBin(0, "1")
#define BIT_VBI_PKT_LO		SetBin(7, "00000000")
#define BIT_VBI_HDELAY		SetBin(7, "000000")
#define BIT_EXT_FRAME		SetBin(1, "1") 
#define BIT_VBI_PKT_HI		SetBin(0, "1")
#define BIT_FCNTR			SetBin(7, "00000000")
#define BIT_PLL_F_LO		SetBin(7, "00000000")
#define BIT_PLL_F_HI		SetBin(7, "00000000")
#define BIT_PLL_X			SetBin(7, "1") // 0: use 1 for pre-divider, 1: use 2 for pre-divider
#define BIT_PLL_C			SetBin(6, "1") // 0: use 6 for post-divider, 1: use 4 for post divider
#define BIT_PLL_I			SetBin(5, "000000")
#define BIT_VSIF_BCF		SetBin(6, "1") // 1: bypass chroma filters, 0: use chroma filters
#define BIT_VSIF_ESO		SetBin(5, "1") // 1: syncs are outputs, 0: syncs are inputs
#define BIT_SVREF			SetBin(4, "00")
#define BIT_VSFMT			SetBin(3, "00")
#define BIT_RISCS			SetBin(31, "0000")
#define BIT_RISC_EN			SetBin(27, "1") // 0: dma controller is disabled
#define BIT_RACK			SetBin(25, "1") // 1: when i2c operation is complete successfully
#define BIT_FIELD			SetBin(24, "1") // 0: odd field, 1: even field
#define BIT_SCERR			SetBin(19, "1") // set when dma eol sync counter overflows
#define BIT_OCERR			SetBin(18, "1") // set when dma controller detects a reserved op-code
#define BIT_PABORT			SetBin(17, "1") // set whenever the initiaor receives a master or target abort
#define BIT_RIPERR			SetBin(16, "1") // data parity error
#define BIT_PPERR			SetBin(15, "1") // pci bus error
#define BIT_FDSR			SetBin(14, "1")
#define BIT_FTRGT			SetBin(13, "1")
#define BIT_FBUS			SetBin(12, "1")
#define BIT_RISCI			SetBin(11, "1")
#define BIT_GPINT			SetBin(9, "1")
#define BIT_I2CDONE			SetBin(8, "1")
#define BIT_VPRES			SetBin(5, "1")
#define BIT_HLOCK			SetBin(4, "1")
#define BIT_OFLOW			SetBin(3, "1")
#define BIT_HSYNC			SetBin(2, "1")
#define BIT_VSYNC			SetBin(1, "1")
#define BIT_FMTCHG			SetBin(0, "1")
#define BIT_GPINTC			SetBin(15, "1")
#define BIT_GPINTI			SetBin(14, "1")
#define BIT_GPIOMODE		SetBin(12, "00")
#define BIT_GPCLKMODE		SetBin(10, "1")
#define BIT_PLTP23			SetBin(7, "00")
#define BIT_PLTP1			SetBin(5, "00")
#define BIT_PKTP			SetBin(3, "00")
#define BIT_RISC_ENABLE		SetBin(1, "1")
#define BIT_FIFO_ENABLE		SetBin(0, "1")
#define BIT_I2CMODE			SetBin(7, "1")
#define BIT_I2CRATE			SetBin(6, "1")
#define BIT_I2CNOSTOP		SetBin(5, "1")
#define BIT_I2CNOS1B		SetBin(4, "1")
#define BIT_I2CSYNC			SetBin(3, "1")
#define BIT_I2CW3BRA		SetBin(2, "1")
#define BIT_I2CSCL			SetBin(1, "1")
#define BIT_I2CSDA			SetBin(0, "1")





// fifo status bits
#define BIT_FM1				SetBin(3, "0110") // fifo mode: packdata to follow
#define BIT_FM3				SetBin(3, "1110") // fifo mode:planar to follow
#define BIT_SOL				SetBin(3, "0010") // first active pixel of scan line
#define BIT_EOL4			SetBin(3, "0001") // last active pixel of scan line, 4valid bytes
#define BIT_EOL3			SetBin(3, "1101") // 3 valid bytes
#define BIT_EOL2			SetBin(3, "1001") // 2 valid bytes
#define BIT_EOL1			SetBin(3, "0101") // 1 valid bytes
#define BIT_VRE				SetBin(3, "0100") // VRESET following an even field
#define BIT_VRO				SetBin(3, "1100") // VRESET following an odd field
#define BIT_PXV				SetBin(3, "0000") // valid pixel/data dword



												

// hardware control
void HwSoftwareReset(PBT878_VIDEOCHIP ); // software initialize
void HwInterruptMask(PBT878_VIDEOCHIP );
void HwInitProgram(PBT878_SCREEN scr); // make risc program which will run in video chip
void HwRunRisc(PBT878_VIDEOCHIP pChip); // risc program
void HwStopRisc(PBT878_VIDEOCHIP pChip);
void HwDisableInterrupt(PBT878_VIDEOCHIP);

  

// debugging tool
#define BREAK _asm int 1

// hardware related 
void WriteReg(PBT878_VIDEOCHIP pChip,ULONG Reg, ULONG Data);
ULONG ReadReg(PBT878_VIDEOCHIP pChip, ULONG Reg	);

#endif