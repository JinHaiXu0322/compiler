	.file	"factorial.c"
	.text
	.globl	factorial
	.type	factorial, @function
factorial:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	count.1726(%rip), %eax
	addl	$1, %eax
	movl	%eax, count.1726(%rip)
	movl	$1, -4(%rbp)
	cmpl	$0, -20(%rbp)
	jne	.L2
	movl	$1, %eax
	jmp	.L3
.L2:
	cmpl	$0, -20(%rbp)
	jns	.L4
	movl	$0, %eax
	jmp	.L3
.L4:
	jmp	.L5
.L6:
	movl	-4(%rbp), %eax
	imull	-20(%rbp), %eax
	movl	%eax, -4(%rbp)
	subl	$1, -20(%rbp)
.L5:
	cmpl	$0, -20(%rbp)
	jne	.L6
	movl	-4(%rbp), %eax
.L3:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	factorial, .-factorial
	.local	count.1726
	.comm	count.1726,4,4
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
