	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test.c"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$72, %rsp
	.seh_stackalloc 72
	leaq	64(%rsp), %rbp
	.seh_setframe %rbp, 64
	.seh_endprologue
	callq	__main
	movl	$0, -24(%rbp)
	movl	$10, -8(%rbp)
	movl	$1, (%rbp)
	movl	$1, 4(%rbp)
	movl	$1, 4(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	xorl	%eax, %eax
	cmpl	$5, 4(%rbp)
	jle	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	cmpl	$10, 4(%rbp)
	setl	%al
.LBB0_3:                                #   in Loop: Header=BB0_1 Depth=1
	testb	$1, %al
	jne	.LBB0_4
	jmp	.LBB0_6
.LBB0_4:                                #   in Loop: Header=BB0_1 Depth=1
	movl	4(%rbp), %ecx
	callq	fibonacci
	movl	%eax, -16(%rbp)
# %bb.5:                                #   in Loop: Header=BB0_1 Depth=1
	movl	4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, 4(%rbp)
	jmp	.LBB0_1
.LBB0_6:
	jmp	.LBB0_7
.LBB0_7:                                # =>This Inner Loop Header: Depth=1
	movl	(%rbp), %esi
	movl	-8(%rbp), %ecx
	movl	$10, %edx
	callq	add
	imull	%eax, %esi
	movl	%esi, -12(%rbp)
	movl	(%rbp), %eax
	addl	$1, %eax
	movl	%eax, (%rbp)
	cmpl	$100, -12(%rbp)
	jle	.LBB0_9
# %bb.8:
	jmp	.LBB0_10
.LBB0_9:                                #   in Loop: Header=BB0_7 Depth=1
	jmp	.LBB0_7
.LBB0_10:
	movl	-8(%rbp), %ecx
	callq	fibonacci
	movl	%eax, -20(%rbp)
	movl	-8(%rbp), %ecx
	movl	$10, %edx
	callq	add
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	g2(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	addq	$72, %rsp
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
	.bss
	.globl	g2                              # @g2
	.p2align	2
g2:
	.long	0                               # 0x0

