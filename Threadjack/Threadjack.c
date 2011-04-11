#include "Threadjack.h"
#include <stdio.h>

//
// The size of all integer registers excluding Rsp.
//

#define INTEGER_REG_SIZE (FIELD_OFFSET(CONTEXT, R15) - FIELD_OFFSET(CONTEXT, Rax))

#define HOME_SPACE_SIZE (sizeof(((PCONTEXT)0)->R8) + sizeof(((PCONTEXT)0)->Rcx) + \
						 sizeof(((PCONTEXT)0)->R9) + sizeof(((PCONTEXT)0)->Rdx))

#define CONTROL_REG_SIZE (sizeof(((PCONTEXT)0)->Rip) + sizeof(((PCONTEXT)0)->EFlags))

extern 
VOID 
Restore (
	__in PDWORD64 Rsp
	);

static
__declspec(noreturn)
VOID
WINAPI
Trampoline (
	__in PCONTEXT PointerToOldContext,
	__inout HANDLE Event,
	__in PCALLBACK Function,
	__in_opt PVOID Argument
	)
{
	CONTEXT OldContext;
	PDWORD64 Rsp;

	OldContext = *PointerToOldContext;
	SetEvent(Event);

	//
	// Copy the control and integer registers to 
	// the reserved space on the stack.
	//

	Rsp = (PDWORD64) OldContext.Rsp;
	*(--Rsp) = OldContext.Rip;
	*(--Rsp) = OldContext.EFlags;
	*(--Rsp) = OldContext.Rax;
	*(--Rsp) = OldContext.Rcx;
	*(--Rsp) = OldContext.Rdx;
	*(--Rsp) = OldContext.Rbx;
	*(--Rsp) = OldContext.Rbp;
	*(--Rsp) = OldContext.Rsi;
	*(--Rsp) = OldContext.Rdi;
	*(--Rsp) = OldContext.R8;
	*(--Rsp) = OldContext.R9;
	*(--Rsp) = OldContext.R10;    
	*(--Rsp) = OldContext.R11;
	*(--Rsp) = OldContext.R12;
	*(--Rsp) = OldContext.R13;
	*(--Rsp) = OldContext.R14;
	*(--Rsp) = OldContext.R15;
	
	Function(Argument);

	//
	// Restore the integer and contol registers.
	//

	Restore(Rsp);
}

FORCEINLINE
BOOL
GetThreadContext (
	__in HANDLE Thread,
	__out PCONTEXT OldContext
	)
{
	RtlZeroMemory(OldContext, sizeof(*OldContext));

	//
	// Try to get the thread's context and ensure it is running in user-mode.
	//

	OldContext->ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_EXCEPTION_REQUEST;
	return GetThreadContext(Thread, OldContext) && IsThreadInUserMode(OldContext->ContextFlags);
}

FORCEINLINE
BOOL
SetNewContext (
	__in HANDLE Thread,    
	__in PCONTEXT OldContext,
	__in HANDLE Event,
	__in PCALLBACK Function,
	__in_opt PVOID Argument
	)
{
	CONTEXT NewContext = *OldContext;
	NewContext.Rip = (LONGLONG) Trampoline;

	//
	// Reserve space on the stack for the integer and control registers + 
	// home space for Trampoline (R9, R8, Rdx and Rcx) + return address.
	//

	NewContext.Rsp -= INTEGER_REG_SIZE + CONTROL_REG_SIZE + 
										HOME_SPACE_SIZE + sizeof(((PCONTEXT)0)->Rip);

	//
	// Arguments.
	//

	NewContext.Rcx = (LONGLONG) OldContext;
	NewContext.Rdx = (LONGLONG) Event;
	NewContext.R8 = (LONGLONG) Function;
	NewContext.R9 = (LONGLONG) Argument;

	return SetThreadContext(Thread, &NewContext);
}

BOOL
WINAPI
HijackThread (
	__in HANDLE Thread,
	__in PCALLBACK Function,
	__in_opt PVOID Argument
	)
{
	HANDLE Event;
	CONTEXT OldContext;
	BOOL Success;
	LONG SuspendCount;

	if (GetThreadId(Thread) == GetCurrentThreadId()) {
		return FALSE;
	}

	//
	// Try to suspend the thread. 
	//

	SuspendCount = SuspendThread(Thread);
	if (SuspendCount == -1) {
		return FALSE;
	}

	Event = NULL;

	//
	// Try to set the new context and wait until the thread no longer
	// needs the old one.
	//

	Success = SuspendCount == 0
				 && GetThreadContext(Thread, &OldContext)
				 && (Event = CreateEvent(NULL, TRUE, FALSE, NULL)) != NULL 
				 && SetNewContext(Thread, &OldContext, Event, Function, Argument);

	ResumeThread(Thread);

	Success = Success && WaitForSingleObject(Event, INFINITE) == WAIT_OBJECT_0;

	if (Event != NULL) {
		CloseHandle(Event);
	}

	return Success;
}
