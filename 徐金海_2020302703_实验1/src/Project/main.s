	.file	"main.c"
	.text
	.globl	g1
	.data
	.align 4
g1:
	.long	10
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
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	call	__main
	movl	$10, -4(%rbp)
	movl	g1(%rip), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	call	add
	movl	%eax, -8(%rbp)
	movl	g2(%rip), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (Rev10, Built by MSYS2 project) 12.2.0"
	.def	add;	.scl	2;	.type	32;	.endef
