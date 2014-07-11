// LKsolutions company
//
// binary utility
//
// coded by Jongwhan Lee (leejw51@hitel.net)
//
// date: 2002. 8. 15 
//
// used pure c language without c-running time library
// for device driver development


#ifndef LK_SOLUTIONS_BINARY_UTILITY_FOR_DRIVER_DEVELOPMENT
#define LK_SOLUTIONS_BINARY_UTILITY_FOR_DRIVER_DEVELOPMENT

#ifndef BASETYPES
#include <ntddk.h>
#endif

// decimal -> binary
char* Dec2Bin(char s[],ULONG d);
char* Dec2Bin(ULONG n, char s[],ULONG d);

// binary -> decimal
ULONG Bin2Dec(char* a);


// set binary
void SetBin(ULONG& num, int start, char* s);
void SetBin(ULONG& num, int start, int end, ULONG a);
void SetBin(ULONG& num, int start, char* s, int s_start, int s_end);

ULONG SetBin(int start, char* s);
ULONG SetBin( int start, int end, ULONG a);
ULONG SetBin(int start, char* s, int s_start, int s_end);

char* SetBin(char* out, int start, char* in);

// get binary by cutting
char* GetBin(char* out, char* in, int start, int end);

ULONG Lkstrlen(char* s);


#endif
