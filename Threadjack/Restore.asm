_TEXT	SEGMENT

PUBLIC	Restore

Restore PROC
	mov	rsp, rcx
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rdi
	pop	rsi
	pop	rbp
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
	popfq
	ret
Restore ENDP

_TEXT ENDS

END
