#include <windows.h>
#include <stdio.h>
#include "Threadjack.h"

DWORD 
WINAPI 
HappyThread (
	__in PVOID Argument 
	) 
{
	volatile PBOOL Stop = (PBOOL) Argument;

	do { 
		YieldProcessor();
	} while (!*Stop);

	return 0;
}

VOID 
WINAPI 
PanickingThread (
	__in PVOID Argument 
	) 
{
	LONG Loops = (LONG) Argument;

	do {
		printf("+", Loops);
	} while (--Loops > 0);
}

VOID
__cdecl
main (
	__in ULONG argc,
	__in_ecount(argc) PCHAR argv[]
	)
{	
	BOOL Stop;
	HANDLE Thread;

	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
		
	Stop = FALSE;

	Thread = CreateThread(NULL, 0, HappyThread, &Stop, 0, NULL);

	getchar();

	//
	// Try to hijack the thread. Spin until the thread leaves kernel-mode.
	//

	while (!HijackThread(Thread, PanickingThread, (PVOID) 500)) {
		YieldProcessor();
	}

	getchar();

	//
	// Stop.
	//

	Stop = TRUE;
	WaitForSingleObject(Thread, INFINITE);
}
