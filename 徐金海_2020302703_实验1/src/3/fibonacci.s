	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"fibonacci.c"
	.def	fibonacci;
	.scl	2;
	.type	32;
	.endef
	.globl	fibonacci                       # -- Begin function fibonacci
	.p2align	4, 0x90
fibonacci:                              # @fibonacci
.seh_proc fibonacci
# %bb.0:
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$48, %rsp
	.seh_stackalloc 48
	.seh_endprologue
	movl	%ecx, 40(%rsp)
	cmpl	$1, 40(%rsp)
	jg	.LBB0_2
# %bb.1:
	movl	40(%rsp), %eax
	movl	%eax, 44(%rsp)
	jmp	.LBB0_3
.LBB0_2:
	movl	40(%rsp), %ecx
	subl	$1, %ecx
	callq	fibonacci
	movl	%eax, %esi
	movl	40(%rsp), %ecx
	subl	$2, %ecx
	callq	fibonacci
	addl	%eax, %esi
	movl	%esi, 44(%rsp)
.LBB0_3:
	movl	44(%rsp), %eax
	addq	$48, %rsp
	popq	%rsi
	retq
	.seh_endproc
                                        # -- End function
