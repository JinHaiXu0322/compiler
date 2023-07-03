	.file	"fibonacci.c"
	.text
	.globl	fibonacci
	.def	fibonacci;	.scl	2;	.type	32;	.endef
	.seh_proc	fibonacci
fibonacci:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	leaq	32(%rsp), %rbp
	.seh_setframe	%rbp, 32
	.seh_endprologue
	movl	%ecx, 32(%rbp)
	cmpl	$1, 32(%rbp)
	jg	.L2
	movl	32(%rbp), %eax
	jmp	.L3
.L2:
	movl	32(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %ecx
	call	fibonacci
	movl	%eax, %ebx
	movl	32(%rbp), %eax
	subl	$2, %eax
	movl	%eax, %ecx
	call	fibonacci
	addl	%ebx, %eax
.L3:
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev10, Built by MSYS2 project) 12.2.0"
