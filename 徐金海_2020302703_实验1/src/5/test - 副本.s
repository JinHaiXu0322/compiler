	.file	"test.c"
	.text
	.globl	g2
	.bss
	.align 4
g2:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	call	__main
	movl	$10, -12(%rbp)
	movl	$1, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	$1, -8(%rbp)
	jmp	.L2
.L4:
	movl	-8(%rbp), %eax
	movl	%eax, %ecx
	call	factorial
	movl	%eax, -16(%rbp)
	addl	$1, -8(%rbp)
.L2:
	cmpl	$5, -8(%rbp)
	jle	.L7
	cmpl	$9, -8(%rbp)
	jle	.L4
.L7:
	movl	-12(%rbp), %eax
	movl	$10, %edx
	movl	%eax, %ecx
	call	add
	movl	-4(%rbp), %edx
	imull	%edx, %eax
	movl	%eax, -20(%rbp)
	addl	$1, -4(%rbp)
	cmpl	$100, -20(%rbp)
	jg	.L10
	jmp	.L7
.L10:
	nop
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	call	fibonacci
	movl	%eax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	$10, %edx
	movl	%eax, %ecx
	call	add
	movl	%eax, -28(%rbp)
	movl	g2(%rip), %eax
	addl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev10, Built by MSYS2 project) 12.2.0"
	.def	factorial;	.scl	2;	.type	32;	.endef
	.def	add;	.scl	2;	.type	32;	.endef
	.def	fibonacci;	.scl	2;	.type	32;	.endef
