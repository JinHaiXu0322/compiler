	.file	"factorial.c"
	.text
	.globl	factorial
	.def	factorial;	.scl	2;	.type	32;	.endef
	.seh_proc	factorial
factorial:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	count.0(%rip), %eax
	addl	$1, %eax
	movl	%eax, count.0(%rip)
	movl	$1, -4(%rbp)
	cmpl	$0, 16(%rbp)
	jne	.L2
	movl	$1, %eax
	jmp	.L3
.L2:
	cmpl	$0, 16(%rbp)
	jns	.L5
	movl	$0, %eax
	jmp	.L3
.L6:
	movl	-4(%rbp), %eax
	imull	16(%rbp), %eax
	movl	%eax, -4(%rbp)
	subl	$1, 16(%rbp)
.L5:
	cmpl	$0, 16(%rbp)
	jne	.L6
	movl	-4(%rbp), %eax
.L3:
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
.lcomm count.0,4,4
	.ident	"GCC: (Rev10, Built by MSYS2 project) 12.2.0"
