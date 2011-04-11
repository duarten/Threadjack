#define CONTEXT_AMD64   0x100000

// end_wx86

#define CONTEXT_CONTROL (CONTEXT_AMD64 | 0x1L)
#define CONTEXT_INTEGER (CONTEXT_AMD64 | 0x2L)
#define CONTEXT_SEGMENTS (CONTEXT_AMD64 | 0x4L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_AMD64 | 0x8L)
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_AMD64 | 0x10L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_FLOATING_POINT)

#define CONTEXT_ALL (CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS | CONTEXT_FLOATING_POINT | CONTEXT_DEBUG_REGISTERS)

#define CONTEXT_XSTATE (CONTEXT_AMD64 | 0x20L)

#define CONTEXT_EXCEPTION_ACTIVE 0x8000000
#define CONTEXT_SERVICE_ACTIVE 0x10000000
#define CONTEXT_EXCEPTION_REQUEST 0x40000000
#define CONTEXT_EXCEPTION_REPORTING 0x80000000

// begin_wx86

#endif // !defined(RC_INVOKED)

//
// Define initial MxCsr and FpCsr control.
//

#define INITIAL_MXCSR 0x1f80            // initial MXCSR value
#define INITIAL_FPCSR 0x027f            // initial FPCSR value

// end_ntddk
// begin_wdm begin_ntosp

typedef XSAVE_FORMAT XMM_SAVE_AREA32, *PXMM_SAVE_AREA32;

// end_wdm end_ntosp
// begin_ntddk

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//
// The flags field within this record controls the contents of a CONTEXT
// record.
//
// If the context record is used as an input parameter, then for each
// portion of the context record controlled by a flag whose value is
// set, it is assumed that that portion of the context record contains
// valid context. If the context record is being used to modify a threads
// context, then only that portion of the threads context is modified.
//
// If the context record is used as an output parameter to capture the
// context of a thread, then only those portions of the thread's context
// corresponding to set flags will be returned.
//
// CONTEXT_CONTROL specifies SegSs, Rsp, SegCs, Rip, and EFlags.
//
// CONTEXT_INTEGER specifies Rax, Rcx, Rdx, Rbx, Rbp, Rsi, Rdi, and R8-R15.
//
// CONTEXT_SEGMENTS specifies SegDs, SegEs, SegFs, and SegGs.
//
// CONTEXT_FLOATING_POINT specifies Xmm0-Xmm15.
//
// CONTEXT_DEBUG_REGISTERS specifies Dr0-Dr3 and Dr6-Dr7.
//

typedef struct DECLSPEC_ALIGN(16) _CONTEXT {

    //
    // Register parameter home addresses.
    //
    // N.B. These fields are for convience - they could be used to extend the
    //      context record in the future.
    //

    DWORD64 P1Home;
    DWORD64 P2Home;
    DWORD64 P3Home;
    DWORD64 P4Home;
    DWORD64 P5Home;
    DWORD64 P6Home;

    //
    // Control flags.
    //

    DWORD ContextFlags;
    DWORD MxCsr;

    //
    // Segment Registers and processor flags.
    //

    WORD   SegCs;
    WORD   SegDs;
    WORD   SegEs;
    WORD   SegFs;
    WORD   SegGs;
    WORD   SegSs;
    DWORD EFlags;

    //
    // Debug registers
    //

    DWORD64 Dr0;
    DWORD64 Dr1;
    DWORD64 Dr2;
    DWORD64 Dr3;
    DWORD64 Dr6;
    DWORD64 Dr7;

    //
    // Integer registers.
    //

    DWORD64 Rax;
    DWORD64 Rcx;
    DWORD64 Rdx;
    DWORD64 Rbx;
    DWORD64 Rsp;
    DWORD64 Rbp;
    DWORD64 Rsi;
    DWORD64 Rdi;
    DWORD64 R8;
    DWORD64 R9;
    DWORD64 R10;
    DWORD64 R11;
    DWORD64 R12;
    DWORD64 R13;
    DWORD64 R14;
    DWORD64 R15;

    //
    // Program counter.
    //

    DWORD64 Rip;

    //
    // Floating point state.
    //

    union {
        XMM_SAVE_AREA32 FltSave;
        struct {
            M128A Header[2];
            M128A Legacy[8];
            M128A Xmm0;
            M128A Xmm1;
            M128A Xmm2;
            M128A Xmm3;
            M128A Xmm4;
            M128A Xmm5;
            M128A Xmm6;
            M128A Xmm7;
            M128A Xmm8;
            M128A Xmm9;
            M128A Xmm10;
            M128A Xmm11;
            M128A Xmm12;
            M128A Xmm13;
            M128A Xmm14;
            M128A Xmm15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    //
    // Vector registers.
    //

    M128A VectorRegister[26];
    DWORD64 VectorControl;

    //
    // Special debug control registers.
    //

    DWORD64 DebugControl;
    DWORD64 LastBranchToRip;
    DWORD64 LastBranchFromRip;
    DWORD64 LastExceptionToRip;
    DWORD64 LastExceptionFromRip;
} CONTEXT, *PCONTEXT;
