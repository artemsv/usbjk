#include "stdafx.h"
#include "BinUtil.h"

char* Lkstrcopy(char* out, char* in, int s, int e)
{
	int i;
	int n=e-s+1;
	for (i=0;i<n;i++)
	{
		*(out+i)=*(in+i+s);
	}
	*(out+i)=0;
	return out;
}

ULONG Lkstrlen(char* s)
{
	if (s==NULL) return 0;

	
	ULONG i;
	for (i=0;i<500;i++)
	{
		if (s[i]==0) break;
	}
	return i;
}
////////////////////////////////////////////////////////////////////////////////////////
ULONG Bin2Dec(char* a)
{
	char a2[255];

	ULONG b=0;
	int n=Lkstrlen(a);
	int i=0,j=0;
	if (n<=0) return b;

	for (i=0;i<n;i++)
	{
		if (a[i]!=' ') a2[j++]=a[i];
	}
	a2[j]=0;

	n=Lkstrlen(a2);

	
	for (i=0;i<n;i++)
	{
		if (a2[n-i-1]=='1') 
			b+= 1<<i;
	}


	return b;
}

char* Dec2Bin(ULONG n, char s[],ULONG d)
{
	s[0]=0;

	if (n==0) {
		return s;
	}


	ULONG j;
	ULONG i=0;
	ULONG v;
	for (j=0;j<n;j++)
	{
		v=1<<(n-j-1);
		s[i++]= (v & d )  ? '1' : '0';
		
	}

	s[i]=0;
	return s;

}


char* Dec2Bin(char s[],ULONG d)
{
	return Dec2Bin(32, s, d);
}
///////////////////////////////////////////////////////////////////////////////////
char* GetBin(char* out, char* in, int start, int end)
{
	int n=Lkstrlen(in);
	return Lkstrcopy(out, in, n-start-1, n-end-1);
}

//////////////////////////////////////////////////////////////////////////////////////
char* SetBin(char* out, int start, char* in)
{
	int i;
	int n=Lkstrlen(in);

	int s=Lkstrlen(out)-1-start;
	for (i=0;i<n;i++)
	{
		*(out+i+s)=*(in+i);
	}
	
	return out;
}

void SetBin(ULONG& num, int start, int end, ULONG a)
{
	int n=start-end+1;
	if (n<=0) return;

	ULONG max_value= (1<<n)-1;
	if (a>max_value) return; // overflow
	
	num |= a<<end;

}

ULONG SetBin( int start, int end, ULONG a)
{
	ULONG num=0;
	SetBin(num, start, end, a);
	return num;

}




void SetBin(ULONG& num, int start, char* s)
{
	
	int leng=Lkstrlen(s);
	int v= Bin2Dec(s);
	int finish= start - leng+1;
	if (start < finish) return;
	if (finish<0) return;

	num = num | v<<finish;

}
ULONG SetBin(int start, char* s)
{
	ULONG num=0;
	
	SetBin(num, start, s);
	return num;
}



void SetBin(ULONG& num, int start, char* s, int s_start, int s_end)
{
	char buf[256];
	int n=Lkstrlen(s);
	Lkstrcopy(buf, s, n-s_start-1, n-s_end-1);
	
	SetBin(num, start, buf);	
}
ULONG SetBin(int start, char* s, int s_start, int s_end)
{
	ULONG num=0;
	char buf[256];
	int n=Lkstrlen(s);
	Lkstrcopy(buf, s, n-s_start-1, n-s_end-1);
	
	SetBin(num, start, buf);
	return num;
}
