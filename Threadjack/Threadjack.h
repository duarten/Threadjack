#define _AMD64_

#include <windows.h>

typedef VOID (__stdcall *PCALLBACK) (
	__in LPVOID Parameter
);

FORCEINLINE
BOOL
WINAPI
IsThreadInUserMode (
	__in LONG ContextFlags
	)
{
	const LONG Mask = CONTEXT_EXCEPTION_REPORTING 
					| CONTEXT_EXCEPTION_ACTIVE 
					| CONTEXT_SERVICE_ACTIVE;
	
	return (Mask & ContextFlags) == CONTEXT_EXCEPTION_REPORTING;
}

//
// Hijacks the specified thread if it's in user-mode.
//

BOOL
WINAPI
HijackThread (
	__in HANDLE Thread,
	__in PCALLBACK Function,
	__in_opt PVOID Argument
	);
