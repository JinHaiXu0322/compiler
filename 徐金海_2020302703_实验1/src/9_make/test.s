	.file	"test.c"
	.comm	g2,4,4
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$10, -12(%rbp)
	movl	$1, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	$1, -8(%rbp)
	jmp	.L2
.L4:
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	factorial
	movl	%eax, -16(%rbp)
	addl	$1, -8(%rbp)
.L2:
	cmpl	$5, -8(%rbp)
	jle	.L3
	cmpl	$9, -8(%rbp)
	jle	.L4
.L3:
	movl	-12(%rbp), %eax
	movl	$10, %esi
	movl	%eax, %edi
	call	add
	imull	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	addl	$1, -4(%rbp)
	cmpl	$100, -20(%rbp)
	jle	.L5
	jmp	.L8
.L5:
	jmp	.L3
.L8:
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	fibonacci
	movl	%eax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	$10, %esi
	movl	%eax, %edi
	call	add
	movl	%eax, -28(%rbp)
	movl	g2(%rip), %eax
	addl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
