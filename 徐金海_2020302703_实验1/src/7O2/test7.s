	.file	"test7.c"
	.text
	.p2align 4
	.globl	add
	.def	add;	.scl	2;	.type	32;	.endef
	.seh_proc	add
add:
	.seh_endprologue
	addl	%edx, %ecx
	addl	%r8d, %ecx
	leal	(%rcx,%r9), %eax
	addl	40(%rsp), %eax
	addl	48(%rsp), %eax
	addl	56(%rsp), %eax
	addl	64(%rsp), %eax
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	call	__main
	xorl	%eax, %eax
	addq	$40, %rsp
	ret
	.seh_endproc
	.ident	"GCC: (Rev10, Built by MSYS2 project) 12.2.0"
