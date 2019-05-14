.text
.globl	ChaCha20_ctr32
.type	ChaCha20_ctr32,@function
.align	16
ChaCha20_ctr32:
.L_ChaCha20_ctr32_begin:
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	xorl	%eax,%eax
	cmpl	28(%esp),%eax
	je	.L000no_data
	call	.Lpic_point
.Lpic_point:
	popl	%eax
	leal	OPENSSL_ia32cap_P-.Lpic_point(%eax),%ebp
	testl	$16777216,(%ebp)
	jz	.L001x86
	testl	$512,4(%ebp)
	jz	.L001x86
	jmp	.Lssse3_shortcut
.L001x86:
	movl	32(%esp),%esi
	movl	36(%esp),%edi
	subl	$132,%esp
	movl	(%esi),%eax
	movl	4(%esi),%ebx
	movl	8(%esi),%ecx
	movl	12(%esi),%edx
	movl	%eax,80(%esp)
	movl	%ebx,84(%esp)
	movl	%ecx,88(%esp)
	movl	%edx,92(%esp)
	movl	16(%esi),%eax
	movl	20(%esi),%ebx
	movl	24(%esi),%ecx
	movl	28(%esi),%edx
	movl	%eax,96(%esp)
	movl	%ebx,100(%esp)
	movl	%ecx,104(%esp)
	movl	%edx,108(%esp)
	movl	(%edi),%eax
	movl	4(%edi),%ebx
	movl	8(%edi),%ecx
	movl	12(%edi),%edx
	subl	$1,%eax
	movl	%eax,112(%esp)
	movl	%ebx,116(%esp)
	movl	%ecx,120(%esp)
	movl	%edx,124(%esp)
	jmp	.L002entry
.align	16
.L003outer_loop:
	movl	%ebx,156(%esp)
	movl	%eax,152(%esp)
	movl	%ecx,160(%esp)
.L002entry:
	movl	$1634760805,%eax
	movl	$857760878,4(%esp)
	movl	$2036477234,8(%esp)
	movl	$1797285236,12(%esp)
	movl	84(%esp),%ebx
	movl	88(%esp),%ebp
	movl	104(%esp),%ecx
	movl	108(%esp),%esi
	movl	116(%esp),%edx
	movl	120(%esp),%edi
	movl	%ebx,20(%esp)
	movl	%ebp,24(%esp)
	movl	%ecx,40(%esp)
	movl	%esi,44(%esp)
	movl	%edx,52(%esp)
	movl	%edi,56(%esp)
	movl	92(%esp),%ebx
	movl	124(%esp),%edi
	movl	112(%esp),%edx
	movl	80(%esp),%ebp
	movl	96(%esp),%ecx
	movl	100(%esp),%esi
	addl	$1,%edx
	movl	%ebx,28(%esp)
	movl	%edi,60(%esp)
	movl	%edx,112(%esp)
	movl	$10,%ebx
	jmp	.L004loop
.align	16
.L004loop:
	addl	%ebp,%eax
	movl	%ebx,128(%esp)
	movl	%ebp,%ebx
	xorl	%eax,%edx
	roll	$16,%edx
	addl	%edx,%ecx
	xorl	%ecx,%ebx
	movl	52(%esp),%edi
	roll	$12,%ebx
	movl	20(%esp),%ebp
	addl	%ebx,%eax
	xorl	%eax,%edx
	movl	%eax,(%esp)
	roll	$8,%edx
	movl	4(%esp),%eax
	addl	%edx,%ecx
	movl	%edx,48(%esp)
	xorl	%ecx,%ebx
	addl	%ebp,%eax
	roll	$7,%ebx
	xorl	%eax,%edi
	movl	%ecx,32(%esp)
	roll	$16,%edi
	movl	%ebx,16(%esp)
	addl	%edi,%esi
	movl	40(%esp),%ecx
	xorl	%esi,%ebp
	movl	56(%esp),%edx
	roll	$12,%ebp
	movl	24(%esp),%ebx
	addl	%ebp,%eax
	xorl	%eax,%edi
	movl	%eax,4(%esp)
	roll	$8,%edi
	movl	8(%esp),%eax
	addl	%edi,%esi
	movl	%edi,52(%esp)
	xorl	%esi,%ebp
	addl	%ebx,%eax
	roll	$7,%ebp
	xorl	%eax,%edx
	movl	%esi,36(%esp)
	roll	$16,%edx
	movl	%ebp,20(%esp)
	addl	%edx,%ecx
	movl	44(%esp),%esi
	xorl	%ecx,%ebx
	movl	60(%esp),%edi
	roll	$12,%ebx
	movl	28(%esp),%ebp
	addl	%ebx,%eax
	xorl	%eax,%edx
	movl	%eax,8(%esp)
	roll	$8,%edx
	movl	12(%esp),%eax
	addl	%edx,%ecx
	movl	%edx,56(%esp)
	xorl	%ecx,%ebx
	addl	%ebp,%eax
	roll	$7,%ebx
	xorl	%eax,%edi
	roll	$16,%edi
	movl	%ebx,24(%esp)
	addl	%edi,%esi
	xorl	%esi,%ebp
	roll	$12,%ebp
	movl	20(%esp),%ebx
	addl	%ebp,%eax
	xorl	%eax,%edi
	movl	%eax,12(%esp)
	roll	$8,%edi
	movl	(%esp),%eax
	addl	%edi,%esi
	movl	%edi,%edx
	xorl	%esi,%ebp
	addl	%ebx,%eax
	roll	$7,%ebp
	xorl	%eax,%edx
	roll	$16,%edx
	movl	%ebp,28(%esp)
	addl	%edx,%ecx
	xorl	%ecx,%ebx
	movl	48(%esp),%edi
	roll	$12,%ebx
	movl	24(%esp),%ebp
	addl	%ebx,%eax
	xorl	%eax,%edx
	movl	%eax,(%esp)
	roll	$8,%edx
	movl	4(%esp),%eax
	addl	%edx,%ecx
	movl	%edx,60(%esp)
	xorl	%ecx,%ebx
	addl	%ebp,%eax
	roll	$7,%ebx
	xorl	%eax,%edi
	movl	%ecx,40(%esp)
	roll	$16,%edi
	movl	%ebx,20(%esp)
	addl	%edi,%esi
	movl	32(%esp),%ecx
	xorl	%esi,%ebp
	movl	52(%esp),%edx
	roll	$12,%ebp
	movl	28(%esp),%ebx
	addl	%ebp,%eax
	xorl	%eax,%edi
	movl	%eax,4(%esp)
	roll	$8,%edi
	movl	8(%esp),%eax
	addl	%edi,%esi
	movl	%edi,48(%esp)
	xorl	%esi,%ebp
	addl	%ebx,%eax
	roll	$7,%ebp
	xorl	%eax,%edx
	movl	%esi,44(%esp)
	roll	$16,%edx
	movl	%ebp,24(%esp)
	addl	%edx,%ecx
	movl	36(%esp),%esi
	xorl	%ecx,%ebx
	movl	56(%esp),%edi
	roll	$12,%ebx
	movl	16(%esp),%ebp
	addl	%ebx,%eax
	xorl	%eax,%edx
	movl	%eax,8(%esp)
	roll	$8,%edx
	movl	12(%esp),%eax
	addl	%edx,%ecx
	movl	%edx,52(%esp)
	xorl	%ecx,%ebx
	addl	%ebp,%eax
	roll	$7,%ebx
	xorl	%eax,%edi
	roll	$16,%edi
	movl	%ebx,28(%esp)
	addl	%edi,%esi
	xorl	%esi,%ebp
	movl	48(%esp),%edx
	roll	$12,%ebp
	movl	128(%esp),%ebx
	addl	%ebp,%eax
	xorl	%eax,%edi
	movl	%eax,12(%esp)
	roll	$8,%edi
	movl	(%esp),%eax
	addl	%edi,%esi
	movl	%edi,56(%esp)
	xorl	%esi,%ebp
	roll	$7,%ebp
	decl	%ebx
	jnz	.L004loop
	movl	160(%esp),%ebx
	addl	$1634760805,%eax
	addl	80(%esp),%ebp
	addl	96(%esp),%ecx
	addl	100(%esp),%esi
	cmpl	$64,%ebx
	jb	.L005tail
	movl	156(%esp),%ebx
	addl	112(%esp),%edx
	addl	120(%esp),%edi
	xorl	(%ebx),%eax
	xorl	16(%ebx),%ebp
	movl	%eax,(%esp)
	movl	152(%esp),%eax
	xorl	32(%ebx),%ecx
	xorl	36(%ebx),%esi
	xorl	48(%ebx),%edx
	xorl	56(%ebx),%edi
	movl	%ebp,16(%eax)
	movl	%ecx,32(%eax)
	movl	%esi,36(%eax)
	movl	%edx,48(%eax)
	movl	%edi,56(%eax)
	movl	4(%esp),%ebp
	movl	8(%esp),%ecx
	movl	12(%esp),%esi
	movl	20(%esp),%edx
	movl	24(%esp),%edi
	addl	$857760878,%ebp
	addl	$2036477234,%ecx
	addl	$1797285236,%esi
	addl	84(%esp),%edx
	addl	88(%esp),%edi
	xorl	4(%ebx),%ebp
	xorl	8(%ebx),%ecx
	xorl	12(%ebx),%esi
	xorl	20(%ebx),%edx
	xorl	24(%ebx),%edi
	movl	%ebp,4(%eax)
	movl	%ecx,8(%eax)
	movl	%esi,12(%eax)
	movl	%edx,20(%eax)
	movl	%edi,24(%eax)
	movl	28(%esp),%ebp
	movl	40(%esp),%ecx
	movl	44(%esp),%esi
	movl	52(%esp),%edx
	movl	60(%esp),%edi
	addl	92(%esp),%ebp
	addl	104(%esp),%ecx
	addl	108(%esp),%esi
	addl	116(%esp),%edx
	addl	124(%esp),%edi
	xorl	28(%ebx),%ebp
	xorl	40(%ebx),%ecx
	xorl	44(%ebx),%esi
	xorl	52(%ebx),%edx
	xorl	60(%ebx),%edi
	leal	64(%ebx),%ebx
	movl	%ebp,28(%eax)
	movl	(%esp),%ebp
	movl	%ecx,40(%eax)
	movl	160(%esp),%ecx
	movl	%esi,44(%eax)
	movl	%edx,52(%eax)
	movl	%edi,60(%eax)
	movl	%ebp,(%eax)
	leal	64(%eax),%eax
	subl	$64,%ecx
	jnz	.L003outer_loop
	jmp	.L006done
.L005tail:
	addl	112(%esp),%edx
	addl	120(%esp),%edi
	movl	%eax,(%esp)
	movl	%ebp,16(%esp)
	movl	%ecx,32(%esp)
	movl	%esi,36(%esp)
	movl	%edx,48(%esp)
	movl	%edi,56(%esp)
	movl	4(%esp),%ebp
	movl	8(%esp),%ecx
	movl	12(%esp),%esi
	movl	20(%esp),%edx
	movl	24(%esp),%edi
	addl	$857760878,%ebp
	addl	$2036477234,%ecx
	addl	$1797285236,%esi
	addl	84(%esp),%edx
	addl	88(%esp),%edi
	movl	%ebp,4(%esp)
	movl	%ecx,8(%esp)
	movl	%esi,12(%esp)
	movl	%edx,20(%esp)
	movl	%edi,24(%esp)
	movl	28(%esp),%ebp
	movl	40(%esp),%ecx
	movl	44(%esp),%esi
	movl	52(%esp),%edx
	movl	60(%esp),%edi
	addl	92(%esp),%ebp
	addl	104(%esp),%ecx
	addl	108(%esp),%esi
	addl	116(%esp),%edx
	addl	124(%esp),%edi
	movl	%ebp,28(%esp)
	movl	156(%esp),%ebp
	movl	%ecx,40(%esp)
	movl	152(%esp),%ecx
	movl	%esi,44(%esp)
	xorl	%esi,%esi
	movl	%edx,52(%esp)
	movl	%edi,60(%esp)
	xorl	%eax,%eax
	xorl	%edx,%edx
.L007tail_loop:
	movb	(%esi,%ebp,1),%al
	movb	(%esp,%esi,1),%dl
	leal	1(%esi),%esi
	xorb	%dl,%al
	movb	%al,-1(%ecx,%esi,1)
	decl	%ebx
	jnz	.L007tail_loop
.L006done:
	addl	$132,%esp
.L000no_data:
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.size	ChaCha20_ctr32,.-.L_ChaCha20_ctr32_begin
.globl	ChaCha20_ssse3
.type	ChaCha20_ssse3,@function
.align	16
ChaCha20_ssse3:
.L_ChaCha20_ssse3_begin:
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
.Lssse3_shortcut:
	testl	$2048,4(%ebp)
	jnz	.Lxop_shortcut
	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ecx
	movl	32(%esp),%edx
	movl	36(%esp),%ebx
	movl	%esp,%ebp
	subl	$524,%esp
	andl	$-64,%esp
	movl	%ebp,512(%esp)
	leal	.Lssse3_data-.Lpic_point(%eax),%eax
	movdqu	(%ebx),%xmm3
	cmpl	$256,%ecx
	jb	.L0081x
	movl	%edx,516(%esp)
	movl	%ebx,520(%esp)
	subl	$256,%ecx
	leal	384(%esp),%ebp
	movdqu	(%edx),%xmm7
	pshufd	$0,%xmm3,%xmm0
	pshufd	$85,%xmm3,%xmm1
	pshufd	$170,%xmm3,%xmm2
	pshufd	$255,%xmm3,%xmm3
	paddd	48(%eax),%xmm0
	pshufd	$0,%xmm7,%xmm4
	pshufd	$85,%xmm7,%xmm5
	psubd	64(%eax),%xmm0
	pshufd	$170,%xmm7,%xmm6
	pshufd	$255,%xmm7,%xmm7
	movdqa	%xmm0,64(%ebp)
	movdqa	%xmm1,80(%ebp)
	movdqa	%xmm2,96(%ebp)
	movdqa	%xmm3,112(%ebp)
	movdqu	16(%edx),%xmm3
	movdqa	%xmm4,-64(%ebp)
	movdqa	%xmm5,-48(%ebp)
	movdqa	%xmm6,-32(%ebp)
	movdqa	%xmm7,-16(%ebp)
	movdqa	32(%eax),%xmm7
	leal	128(%esp),%ebx
	pshufd	$0,%xmm3,%xmm0
	pshufd	$85,%xmm3,%xmm1
	pshufd	$170,%xmm3,%xmm2
	pshufd	$255,%xmm3,%xmm3
	pshufd	$0,%xmm7,%xmm4
	pshufd	$85,%xmm7,%xmm5
	pshufd	$170,%xmm7,%xmm6
	pshufd	$255,%xmm7,%xmm7
	movdqa	%xmm0,(%ebp)
	movdqa	%xmm1,16(%ebp)
	movdqa	%xmm2,32(%ebp)
	movdqa	%xmm3,48(%ebp)
	movdqa	%xmm4,-128(%ebp)
	movdqa	%xmm5,-112(%ebp)
	movdqa	%xmm6,-96(%ebp)
	movdqa	%xmm7,-80(%ebp)
	leal	128(%esi),%esi
	leal	128(%edi),%edi
	jmp	.L009outer_loop
.align	16
.L009outer_loop:
	movdqa	-112(%ebp),%xmm1
	movdqa	-96(%ebp),%xmm2
	movdqa	-80(%ebp),%xmm3
	movdqa	-48(%ebp),%xmm5
	movdqa	-32(%ebp),%xmm6
	movdqa	-16(%ebp),%xmm7
	movdqa	%xmm1,-112(%ebx)
	movdqa	%xmm2,-96(%ebx)
	movdqa	%xmm3,-80(%ebx)
	movdqa	%xmm5,-48(%ebx)
	movdqa	%xmm6,-32(%ebx)
	movdqa	%xmm7,-16(%ebx)
	movdqa	32(%ebp),%xmm2
	movdqa	48(%ebp),%xmm3
	movdqa	64(%ebp),%xmm4
	movdqa	80(%ebp),%xmm5
	movdqa	96(%ebp),%xmm6
	movdqa	112(%ebp),%xmm7
	paddd	64(%eax),%xmm4
	movdqa	%xmm2,32(%ebx)
	movdqa	%xmm3,48(%ebx)
	movdqa	%xmm4,64(%ebx)
	movdqa	%xmm5,80(%ebx)
	movdqa	%xmm6,96(%ebx)
	movdqa	%xmm7,112(%ebx)
	movdqa	%xmm4,64(%ebp)
	movdqa	-128(%ebp),%xmm0
	movdqa	%xmm4,%xmm6
	movdqa	-64(%ebp),%xmm3
	movdqa	(%ebp),%xmm4
	movdqa	16(%ebp),%xmm5
	movl	$10,%edx
	nop
.align	16
.L010loop:
	paddd	%xmm3,%xmm0
	movdqa	%xmm3,%xmm2
	pxor	%xmm0,%xmm6
	pshufb	(%eax),%xmm6
	paddd	%xmm6,%xmm4
	pxor	%xmm4,%xmm2
	movdqa	-48(%ebx),%xmm3
	movdqa	%xmm2,%xmm1
	pslld	$12,%xmm2
	psrld	$20,%xmm1
	por	%xmm1,%xmm2
	movdqa	-112(%ebx),%xmm1
	paddd	%xmm2,%xmm0
	movdqa	80(%ebx),%xmm7
	pxor	%xmm0,%xmm6
	movdqa	%xmm0,-128(%ebx)
	pshufb	16(%eax),%xmm6
	paddd	%xmm6,%xmm4
	movdqa	%xmm6,64(%ebx)
	pxor	%xmm4,%xmm2
	paddd	%xmm3,%xmm1
	movdqa	%xmm2,%xmm0
	pslld	$7,%xmm2
	psrld	$25,%xmm0
	pxor	%xmm1,%xmm7
	por	%xmm0,%xmm2
	movdqa	%xmm4,(%ebx)
	pshufb	(%eax),%xmm7
	movdqa	%xmm2,-64(%ebx)
	paddd	%xmm7,%xmm5
	movdqa	32(%ebx),%xmm4
	pxor	%xmm5,%xmm3
	movdqa	-32(%ebx),%xmm2
	movdqa	%xmm3,%xmm0
	pslld	$12,%xmm3
	psrld	$20,%xmm0
	por	%xmm0,%xmm3
	movdqa	-96(%ebx),%xmm0
	paddd	%xmm3,%xmm1
	movdqa	96(%ebx),%xmm6
	pxor	%xmm1,%xmm7
	movdqa	%xmm1,-112(%ebx)
	pshufb	16(%eax),%xmm7
	paddd	%xmm7,%xmm5
	movdqa	%xmm7,80(%ebx)
	pxor	%xmm5,%xmm3
	paddd	%xmm2,%xmm0
	movdqa	%xmm3,%xmm1
	pslld	$7,%xmm3
	psrld	$25,%xmm1
	pxor	%xmm0,%xmm6
	por	%xmm1,%xmm3
	movdqa	%xmm5,16(%ebx)
	pshufb	(%eax),%xmm6
	movdqa	%xmm3,-48(%ebx)
	paddd	%xmm6,%xmm4
	movdqa	48(%ebx),%xmm5
	pxor	%xmm4,%xmm2
	movdqa	-16(%ebx),%xmm3
	movdqa	%xmm2,%xmm1
	pslld	$12,%xmm2
	psrld	$20,%xmm1
	por	%xmm1,%xmm2
	movdqa	-80(%ebx),%xmm1
	paddd	%xmm2,%xmm0
	movdqa	112(%ebx),%xmm7
	pxor	%xmm0,%xmm6
	movdqa	%xmm0,-96(%ebx)
	pshufb	16(%eax),%xmm6
	paddd	%xmm6,%xmm4
	movdqa	%xmm6,96(%ebx)
	pxor	%xmm4,%xmm2
	paddd	%xmm3,%xmm1
	movdqa	%xmm2,%xmm0
	pslld	$7,%xmm2
	psrld	$25,%xmm0
	pxor	%xmm1,%xmm7
	por	%xmm0,%xmm2
	pshufb	(%eax),%xmm7
	movdqa	%xmm2,-32(%ebx)
	paddd	%xmm7,%xmm5
	pxor	%xmm5,%xmm3
	movdqa	-48(%ebx),%xmm2
	movdqa	%xmm3,%xmm0
	pslld	$12,%xmm3
	psrld	$20,%xmm0
	por	%xmm0,%xmm3
	movdqa	-128(%ebx),%xmm0
	paddd	%xmm3,%xmm1
	pxor	%xmm1,%xmm7
	movdqa	%xmm1,-80(%ebx)
	pshufb	16(%eax),%xmm7
	paddd	%xmm7,%xmm5
	movdqa	%xmm7,%xmm6
	pxor	%xmm5,%xmm3
	paddd	%xmm2,%xmm0
	movdqa	%xmm3,%xmm1
	pslld	$7,%xmm3
	psrld	$25,%xmm1
	pxor	%xmm0,%xmm6
	por	%xmm1,%xmm3
	pshufb	(%eax),%xmm6
	movdqa	%xmm3,-16(%ebx)
	paddd	%xmm6,%xmm4
	pxor	%xmm4,%xmm2
	movdqa	-32(%ebx),%xmm3
	movdqa	%xmm2,%xmm1
	pslld	$12,%xmm2
	psrld	$20,%xmm1
	por	%xmm1,%xmm2
	movdqa	-112(%ebx),%xmm1
	paddd	%xmm2,%xmm0
	movdqa	64(%ebx),%xmm7
	pxor	%xmm0,%xmm6
	movdqa	%xmm0,-128(%ebx)
	pshufb	16(%eax),%xmm6
	paddd	%xmm6,%xmm4
	movdqa	%xmm6,112(%ebx)
	pxor	%xmm4,%xmm2
	paddd	%xmm3,%xmm1
	movdqa	%xmm2,%xmm0
	pslld	$7,%xmm2
	psrld	$25,%xmm0
	pxor	%xmm1,%xmm7
	por	%xmm0,%xmm2
	movdqa	%xmm4,32(%ebx)
	pshufb	(%eax),%xmm7
	movdqa	%xmm2,-48(%ebx)
	paddd	%xmm7,%xmm5
	movdqa	(%ebx),%xmm4
	pxor	%xmm5,%xmm3
	movdqa	-16(%ebx),%xmm2
	movdqa	%xmm3,%xmm0
	pslld	$12,%xmm3
	psrld	$20,%xmm0
	por	%xmm0,%xmm3
	movdqa	-96(%ebx),%xmm0
	paddd	%xmm3,%xmm1
	movdqa	80(%ebx),%xmm6
	pxor	%xmm1,%xmm7
	movdqa	%xmm1,-112(%ebx)
	pshufb	16(%eax),%xmm7
	paddd	%xmm7,%xmm5
	movdqa	%xmm7,64(%ebx)
	pxor	%xmm5,%xmm3
	paddd	%xmm2,%xmm0
	movdqa	%xmm3,%xmm1
	pslld	$7,%xmm3
	psrld	$25,%xmm1
	pxor	%xmm0,%xmm6
	por	%xmm1,%xmm3
	movdqa	%xmm5,48(%ebx)
	pshufb	(%eax),%xmm6
	movdqa	%xmm3,-32(%ebx)
	paddd	%xmm6,%xmm4
	movdqa	16(%ebx),%xmm5
	pxor	%xmm4,%xmm2
	movdqa	-64(%ebx),%xmm3
	movdqa	%xmm2,%xmm1
	pslld	$12,%xmm2
	psrld	$20,%xmm1
	por	%xmm1,%xmm2
	movdqa	-80(%ebx),%xmm1
	paddd	%xmm2,%xmm0
	movdqa	96(%ebx),%xmm7
	pxor	%xmm0,%xmm6
	movdqa	%xmm0,-96(%ebx)
	pshufb	16(%eax),%xmm6
	paddd	%xmm6,%xmm4
	movdqa	%xmm6,80(%ebx)
	pxor	%xmm4,%xmm2
	paddd	%xmm3,%xmm1
	movdqa	%xmm2,%xmm0
	pslld	$7,%xmm2
	psrld	$25,%xmm0
	pxor	%xmm1,%xmm7
	por	%xmm0,%xmm2
	pshufb	(%eax),%xmm7
	movdqa	%xmm2,-16(%ebx)
	paddd	%xmm7,%xmm5
	pxor	%xmm5,%xmm3
	movdqa	%xmm3,%xmm0
	pslld	$12,%xmm3
	psrld	$20,%xmm0
	por	%xmm0,%xmm3
	movdqa	-128(%ebx),%xmm0
	paddd	%xmm3,%xmm1
	movdqa	64(%ebx),%xmm6
	pxor	%xmm1,%xmm7
	movdqa	%xmm1,-80(%ebx)
	pshufb	16(%eax),%xmm7
	paddd	%xmm7,%xmm5
	movdqa	%xmm7,96(%ebx)
	pxor	%xmm5,%xmm3
	movdqa	%xmm3,%xmm1
	pslld	$7,%xmm3
	psrld	$25,%xmm1
	por	%xmm1,%xmm3
	decl	%edx
	jnz	.L010loop
	movdqa	%xmm3,-64(%ebx)
	movdqa	%xmm4,(%ebx)
	movdqa	%xmm5,16(%ebx)
	movdqa	%xmm6,64(%ebx)
	movdqa	%xmm7,96(%ebx)
	movdqa	-112(%ebx),%xmm1
	movdqa	-96(%ebx),%xmm2
	movdqa	-80(%ebx),%xmm3
	paddd	-128(%ebp),%xmm0
	paddd	-112(%ebp),%xmm1
	paddd	-96(%ebp),%xmm2
	paddd	-80(%ebp),%xmm3
	movdqa	%xmm0,%xmm6
	punpckldq	%xmm1,%xmm0
	movdqa	%xmm2,%xmm7
	punpckldq	%xmm3,%xmm2
	punpckhdq	%xmm1,%xmm6
	punpckhdq	%xmm3,%xmm7
	movdqa	%xmm0,%xmm1
	punpcklqdq	%xmm2,%xmm0
	movdqa	%xmm6,%xmm3
	punpcklqdq	%xmm7,%xmm6
	punpckhqdq	%xmm2,%xmm1
	punpckhqdq	%xmm7,%xmm3
	movdqu	-128(%esi),%xmm4
	movdqu	-64(%esi),%xmm5
	movdqu	(%esi),%xmm2
	movdqu	64(%esi),%xmm7
	leal	16(%esi),%esi
	pxor	%xmm0,%xmm4
	movdqa	-64(%ebx),%xmm0
	pxor	%xmm1,%xmm5
	movdqa	-48(%ebx),%xmm1
	pxor	%xmm2,%xmm6
	movdqa	-32(%ebx),%xmm2
	pxor	%xmm3,%xmm7
	movdqa	-16(%ebx),%xmm3
	movdqu	%xmm4,-128(%edi)
	movdqu	%xmm5,-64(%edi)
	movdqu	%xmm6,(%edi)
	movdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	paddd	-64(%ebp),%xmm0
	paddd	-48(%ebp),%xmm1
	paddd	-32(%ebp),%xmm2
	paddd	-16(%ebp),%xmm3
	movdqa	%xmm0,%xmm6
	punpckldq	%xmm1,%xmm0
	movdqa	%xmm2,%xmm7
	punpckldq	%xmm3,%xmm2
	punpckhdq	%xmm1,%xmm6
	punpckhdq	%xmm3,%xmm7
	movdqa	%xmm0,%xmm1
	punpcklqdq	%xmm2,%xmm0
	movdqa	%xmm6,%xmm3
	punpcklqdq	%xmm7,%xmm6
	punpckhqdq	%xmm2,%xmm1
	punpckhqdq	%xmm7,%xmm3
	movdqu	-128(%esi),%xmm4
	movdqu	-64(%esi),%xmm5
	movdqu	(%esi),%xmm2
	movdqu	64(%esi),%xmm7
	leal	16(%esi),%esi
	pxor	%xmm0,%xmm4
	movdqa	(%ebx),%xmm0
	pxor	%xmm1,%xmm5
	movdqa	16(%ebx),%xmm1
	pxor	%xmm2,%xmm6
	movdqa	32(%ebx),%xmm2
	pxor	%xmm3,%xmm7
	movdqa	48(%ebx),%xmm3
	movdqu	%xmm4,-128(%edi)
	movdqu	%xmm5,-64(%edi)
	movdqu	%xmm6,(%edi)
	movdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	paddd	(%ebp),%xmm0
	paddd	16(%ebp),%xmm1
	paddd	32(%ebp),%xmm2
	paddd	48(%ebp),%xmm3
	movdqa	%xmm0,%xmm6
	punpckldq	%xmm1,%xmm0
	movdqa	%xmm2,%xmm7
	punpckldq	%xmm3,%xmm2
	punpckhdq	%xmm1,%xmm6
	punpckhdq	%xmm3,%xmm7
	movdqa	%xmm0,%xmm1
	punpcklqdq	%xmm2,%xmm0
	movdqa	%xmm6,%xmm3
	punpcklqdq	%xmm7,%xmm6
	punpckhqdq	%xmm2,%xmm1
	punpckhqdq	%xmm7,%xmm3
	movdqu	-128(%esi),%xmm4
	movdqu	-64(%esi),%xmm5
	movdqu	(%esi),%xmm2
	movdqu	64(%esi),%xmm7
	leal	16(%esi),%esi
	pxor	%xmm0,%xmm4
	movdqa	64(%ebx),%xmm0
	pxor	%xmm1,%xmm5
	movdqa	80(%ebx),%xmm1
	pxor	%xmm2,%xmm6
	movdqa	96(%ebx),%xmm2
	pxor	%xmm3,%xmm7
	movdqa	112(%ebx),%xmm3
	movdqu	%xmm4,-128(%edi)
	movdqu	%xmm5,-64(%edi)
	movdqu	%xmm6,(%edi)
	movdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	paddd	64(%ebp),%xmm0
	paddd	80(%ebp),%xmm1
	paddd	96(%ebp),%xmm2
	paddd	112(%ebp),%xmm3
	movdqa	%xmm0,%xmm6
	punpckldq	%xmm1,%xmm0
	movdqa	%xmm2,%xmm7
	punpckldq	%xmm3,%xmm2
	punpckhdq	%xmm1,%xmm6
	punpckhdq	%xmm3,%xmm7
	movdqa	%xmm0,%xmm1
	punpcklqdq	%xmm2,%xmm0
	movdqa	%xmm6,%xmm3
	punpcklqdq	%xmm7,%xmm6
	punpckhqdq	%xmm2,%xmm1
	punpckhqdq	%xmm7,%xmm3
	movdqu	-128(%esi),%xmm4
	movdqu	-64(%esi),%xmm5
	movdqu	(%esi),%xmm2
	movdqu	64(%esi),%xmm7
	leal	208(%esi),%esi
	pxor	%xmm0,%xmm4
	pxor	%xmm1,%xmm5
	pxor	%xmm2,%xmm6
	pxor	%xmm3,%xmm7
	movdqu	%xmm4,-128(%edi)
	movdqu	%xmm5,-64(%edi)
	movdqu	%xmm6,(%edi)
	movdqu	%xmm7,64(%edi)
	leal	208(%edi),%edi
	subl	$256,%ecx
	jnc	.L009outer_loop
	addl	$256,%ecx
	jz	.L011done
	movl	520(%esp),%ebx
	leal	-128(%esi),%esi
	movl	516(%esp),%edx
	leal	-128(%edi),%edi
	movd	64(%ebp),%xmm2
	movdqu	(%ebx),%xmm3
	paddd	96(%eax),%xmm2
	pand	112(%eax),%xmm3
	por	%xmm2,%xmm3
.L0081x:
	movdqa	32(%eax),%xmm0
	movdqu	(%edx),%xmm1
	movdqu	16(%edx),%xmm2
	movdqa	(%eax),%xmm6
	movdqa	16(%eax),%xmm7
	movl	%ebp,48(%esp)
	movdqa	%xmm0,(%esp)
	movdqa	%xmm1,16(%esp)
	movdqa	%xmm2,32(%esp)
	movdqa	%xmm3,48(%esp)
	movl	$10,%edx
	jmp	.L012loop1x
.align	16
.L013outer1x:
	movdqa	80(%eax),%xmm3
	movdqa	(%esp),%xmm0
	movdqa	16(%esp),%xmm1
	movdqa	32(%esp),%xmm2
	paddd	48(%esp),%xmm3
	movl	$10,%edx
	movdqa	%xmm3,48(%esp)
	jmp	.L012loop1x
.align	16
.L012loop1x:
	paddd	%xmm1,%xmm0
	pxor	%xmm0,%xmm3
.byte	102,15,56,0,222
	paddd	%xmm3,%xmm2
	pxor	%xmm2,%xmm1
	movdqa	%xmm1,%xmm4
	psrld	$20,%xmm1
	pslld	$12,%xmm4
	por	%xmm4,%xmm1
	paddd	%xmm1,%xmm0
	pxor	%xmm0,%xmm3
.byte	102,15,56,0,223
	paddd	%xmm3,%xmm2
	pxor	%xmm2,%xmm1
	movdqa	%xmm1,%xmm4
	psrld	$25,%xmm1
	pslld	$7,%xmm4
	por	%xmm4,%xmm1
	pshufd	$78,%xmm2,%xmm2
	pshufd	$57,%xmm1,%xmm1
	pshufd	$147,%xmm3,%xmm3
	nop
	paddd	%xmm1,%xmm0
	pxor	%xmm0,%xmm3
.byte	102,15,56,0,222
	paddd	%xmm3,%xmm2
	pxor	%xmm2,%xmm1
	movdqa	%xmm1,%xmm4
	psrld	$20,%xmm1
	pslld	$12,%xmm4
	por	%xmm4,%xmm1
	paddd	%xmm1,%xmm0
	pxor	%xmm0,%xmm3
.byte	102,15,56,0,223
	paddd	%xmm3,%xmm2
	pxor	%xmm2,%xmm1
	movdqa	%xmm1,%xmm4
	psrld	$25,%xmm1
	pslld	$7,%xmm4
	por	%xmm4,%xmm1
	pshufd	$78,%xmm2,%xmm2
	pshufd	$147,%xmm1,%xmm1
	pshufd	$57,%xmm3,%xmm3
	decl	%edx
	jnz	.L012loop1x
	paddd	(%esp),%xmm0
	paddd	16(%esp),%xmm1
	paddd	32(%esp),%xmm2
	paddd	48(%esp),%xmm3
	cmpl	$64,%ecx
	jb	.L014tail
	movdqu	(%esi),%xmm4
	movdqu	16(%esi),%xmm5
	pxor	%xmm4,%xmm0
	movdqu	32(%esi),%xmm4
	pxor	%xmm5,%xmm1
	movdqu	48(%esi),%xmm5
	pxor	%xmm4,%xmm2
	pxor	%xmm5,%xmm3
	leal	64(%esi),%esi
	movdqu	%xmm0,(%edi)
	movdqu	%xmm1,16(%edi)
	movdqu	%xmm2,32(%edi)
	movdqu	%xmm3,48(%edi)
	leal	64(%edi),%edi
	subl	$64,%ecx
	jnz	.L013outer1x
	jmp	.L011done
.L014tail:
	movdqa	%xmm0,(%esp)
	movdqa	%xmm1,16(%esp)
	movdqa	%xmm2,32(%esp)
	movdqa	%xmm3,48(%esp)
	xorl	%eax,%eax
	xorl	%edx,%edx
	xorl	%ebp,%ebp
.L015tail_loop:
	movb	(%esp,%ebp,1),%al
	movb	(%esi,%ebp,1),%dl
	leal	1(%ebp),%ebp
	xorb	%dl,%al
	movb	%al,-1(%edi,%ebp,1)
	decl	%ecx
	jnz	.L015tail_loop
.L011done:
	movl	512(%esp),%esp
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.size	ChaCha20_ssse3,.-.L_ChaCha20_ssse3_begin
.align	64
.Lssse3_data:
.byte	2,3,0,1,6,7,4,5,10,11,8,9,14,15,12,13
.byte	3,0,1,2,7,4,5,6,11,8,9,10,15,12,13,14
.long	1634760805,857760878,2036477234,1797285236
.long	0,1,2,3
.long	4,4,4,4
.long	1,0,0,0
.long	4,0,0,0
.long	0,-1,-1,-1
.align	64
.byte	67,104,97,67,104,97,50,48,32,102,111,114,32,120,56,54
.byte	44,32,67,82,89,80,84,79,71,65,77,83,32,98,121,32
.byte	60,97,112,112,114,111,64,111,112,101,110,115,115,108,46,111
.byte	114,103,62,0
.globl	ChaCha20_xop
.type	ChaCha20_xop,@function
.align	16
ChaCha20_xop:
.L_ChaCha20_xop_begin:
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
.Lxop_shortcut:
	movl	20(%esp),%edi
	movl	24(%esp),%esi
	movl	28(%esp),%ecx
	movl	32(%esp),%edx
	movl	36(%esp),%ebx
	vzeroupper
	movl	%esp,%ebp
	subl	$524,%esp
	andl	$-64,%esp
	movl	%ebp,512(%esp)
	leal	.Lssse3_data-.Lpic_point(%eax),%eax
	vmovdqu	(%ebx),%xmm3
	cmpl	$256,%ecx
	jb	.L0161x
	movl	%edx,516(%esp)
	movl	%ebx,520(%esp)
	subl	$256,%ecx
	leal	384(%esp),%ebp
	vmovdqu	(%edx),%xmm7
	vpshufd	$0,%xmm3,%xmm0
	vpshufd	$85,%xmm3,%xmm1
	vpshufd	$170,%xmm3,%xmm2
	vpshufd	$255,%xmm3,%xmm3
	vpaddd	48(%eax),%xmm0,%xmm0
	vpshufd	$0,%xmm7,%xmm4
	vpshufd	$85,%xmm7,%xmm5
	vpsubd	64(%eax),%xmm0,%xmm0
	vpshufd	$170,%xmm7,%xmm6
	vpshufd	$255,%xmm7,%xmm7
	vmovdqa	%xmm0,64(%ebp)
	vmovdqa	%xmm1,80(%ebp)
	vmovdqa	%xmm2,96(%ebp)
	vmovdqa	%xmm3,112(%ebp)
	vmovdqu	16(%edx),%xmm3
	vmovdqa	%xmm4,-64(%ebp)
	vmovdqa	%xmm5,-48(%ebp)
	vmovdqa	%xmm6,-32(%ebp)
	vmovdqa	%xmm7,-16(%ebp)
	vmovdqa	32(%eax),%xmm7
	leal	128(%esp),%ebx
	vpshufd	$0,%xmm3,%xmm0
	vpshufd	$85,%xmm3,%xmm1
	vpshufd	$170,%xmm3,%xmm2
	vpshufd	$255,%xmm3,%xmm3
	vpshufd	$0,%xmm7,%xmm4
	vpshufd	$85,%xmm7,%xmm5
	vpshufd	$170,%xmm7,%xmm6
	vpshufd	$255,%xmm7,%xmm7
	vmovdqa	%xmm0,(%ebp)
	vmovdqa	%xmm1,16(%ebp)
	vmovdqa	%xmm2,32(%ebp)
	vmovdqa	%xmm3,48(%ebp)
	vmovdqa	%xmm4,-128(%ebp)
	vmovdqa	%xmm5,-112(%ebp)
	vmovdqa	%xmm6,-96(%ebp)
	vmovdqa	%xmm7,-80(%ebp)
	leal	128(%esi),%esi
	leal	128(%edi),%edi
	jmp	.L017outer_loop
.align	32
.L017outer_loop:
	vmovdqa	-112(%ebp),%xmm1
	vmovdqa	-96(%ebp),%xmm2
	vmovdqa	-80(%ebp),%xmm3
	vmovdqa	-48(%ebp),%xmm5
	vmovdqa	-32(%ebp),%xmm6
	vmovdqa	-16(%ebp),%xmm7
	vmovdqa	%xmm1,-112(%ebx)
	vmovdqa	%xmm2,-96(%ebx)
	vmovdqa	%xmm3,-80(%ebx)
	vmovdqa	%xmm5,-48(%ebx)
	vmovdqa	%xmm6,-32(%ebx)
	vmovdqa	%xmm7,-16(%ebx)
	vmovdqa	32(%ebp),%xmm2
	vmovdqa	48(%ebp),%xmm3
	vmovdqa	64(%ebp),%xmm4
	vmovdqa	80(%ebp),%xmm5
	vmovdqa	96(%ebp),%xmm6
	vmovdqa	112(%ebp),%xmm7
	vpaddd	64(%eax),%xmm4,%xmm4
	vmovdqa	%xmm2,32(%ebx)
	vmovdqa	%xmm3,48(%ebx)
	vmovdqa	%xmm4,64(%ebx)
	vmovdqa	%xmm5,80(%ebx)
	vmovdqa	%xmm6,96(%ebx)
	vmovdqa	%xmm7,112(%ebx)
	vmovdqa	%xmm4,64(%ebp)
	vmovdqa	-128(%ebp),%xmm0
	vmovdqa	%xmm4,%xmm6
	vmovdqa	-64(%ebp),%xmm3
	vmovdqa	(%ebp),%xmm4
	vmovdqa	16(%ebp),%xmm5
	movl	$10,%edx
	nop
.align	32
.L018loop:
	vpaddd	%xmm3,%xmm0,%xmm0
	vpxor	%xmm0,%xmm6,%xmm6
.byte	143,232,120,194,246,16
	vpaddd	%xmm6,%xmm4,%xmm4
	vpxor	%xmm4,%xmm3,%xmm2
	vmovdqa	-112(%ebx),%xmm1
.byte	143,232,120,194,210,12
	vmovdqa	-48(%ebx),%xmm3
	vpaddd	%xmm2,%xmm0,%xmm0
	vmovdqa	80(%ebx),%xmm7
	vpxor	%xmm0,%xmm6,%xmm6
	vpaddd	%xmm3,%xmm1,%xmm1
.byte	143,232,120,194,246,8
	vmovdqa	%xmm0,-128(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	%xmm6,64(%ebx)
	vpxor	%xmm4,%xmm2,%xmm2
	vpxor	%xmm1,%xmm7,%xmm7
.byte	143,232,120,194,210,7
	vmovdqa	%xmm4,(%ebx)
.byte	143,232,120,194,255,16
	vmovdqa	%xmm2,-64(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vmovdqa	32(%ebx),%xmm4
	vpxor	%xmm5,%xmm3,%xmm3
	vmovdqa	-96(%ebx),%xmm0
.byte	143,232,120,194,219,12
	vmovdqa	-32(%ebx),%xmm2
	vpaddd	%xmm3,%xmm1,%xmm1
	vmovdqa	96(%ebx),%xmm6
	vpxor	%xmm1,%xmm7,%xmm7
	vpaddd	%xmm2,%xmm0,%xmm0
.byte	143,232,120,194,255,8
	vmovdqa	%xmm1,-112(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vmovdqa	%xmm7,80(%ebx)
	vpxor	%xmm5,%xmm3,%xmm3
	vpxor	%xmm0,%xmm6,%xmm6
.byte	143,232,120,194,219,7
	vmovdqa	%xmm5,16(%ebx)
.byte	143,232,120,194,246,16
	vmovdqa	%xmm3,-48(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	48(%ebx),%xmm5
	vpxor	%xmm4,%xmm2,%xmm2
	vmovdqa	-80(%ebx),%xmm1
.byte	143,232,120,194,210,12
	vmovdqa	-16(%ebx),%xmm3
	vpaddd	%xmm2,%xmm0,%xmm0
	vmovdqa	112(%ebx),%xmm7
	vpxor	%xmm0,%xmm6,%xmm6
	vpaddd	%xmm3,%xmm1,%xmm1
.byte	143,232,120,194,246,8
	vmovdqa	%xmm0,-96(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	%xmm6,96(%ebx)
	vpxor	%xmm4,%xmm2,%xmm2
	vpxor	%xmm1,%xmm7,%xmm7
.byte	143,232,120,194,210,7
.byte	143,232,120,194,255,16
	vmovdqa	%xmm2,-32(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vpxor	%xmm5,%xmm3,%xmm3
	vmovdqa	-128(%ebx),%xmm0
.byte	143,232,120,194,219,12
	vmovdqa	-48(%ebx),%xmm2
	vpaddd	%xmm3,%xmm1,%xmm1
	vpxor	%xmm1,%xmm7,%xmm7
	vpaddd	%xmm2,%xmm0,%xmm0
.byte	143,232,120,194,255,8
	vmovdqa	%xmm1,-80(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vpxor	%xmm5,%xmm3,%xmm3
	vpxor	%xmm0,%xmm7,%xmm6
.byte	143,232,120,194,219,7
.byte	143,232,120,194,246,16
	vmovdqa	%xmm3,-16(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vpxor	%xmm4,%xmm2,%xmm2
	vmovdqa	-112(%ebx),%xmm1
.byte	143,232,120,194,210,12
	vmovdqa	-32(%ebx),%xmm3
	vpaddd	%xmm2,%xmm0,%xmm0
	vmovdqa	64(%ebx),%xmm7
	vpxor	%xmm0,%xmm6,%xmm6
	vpaddd	%xmm3,%xmm1,%xmm1
.byte	143,232,120,194,246,8
	vmovdqa	%xmm0,-128(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	%xmm6,112(%ebx)
	vpxor	%xmm4,%xmm2,%xmm2
	vpxor	%xmm1,%xmm7,%xmm7
.byte	143,232,120,194,210,7
	vmovdqa	%xmm4,32(%ebx)
.byte	143,232,120,194,255,16
	vmovdqa	%xmm2,-48(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vmovdqa	(%ebx),%xmm4
	vpxor	%xmm5,%xmm3,%xmm3
	vmovdqa	-96(%ebx),%xmm0
.byte	143,232,120,194,219,12
	vmovdqa	-16(%ebx),%xmm2
	vpaddd	%xmm3,%xmm1,%xmm1
	vmovdqa	80(%ebx),%xmm6
	vpxor	%xmm1,%xmm7,%xmm7
	vpaddd	%xmm2,%xmm0,%xmm0
.byte	143,232,120,194,255,8
	vmovdqa	%xmm1,-112(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vmovdqa	%xmm7,64(%ebx)
	vpxor	%xmm5,%xmm3,%xmm3
	vpxor	%xmm0,%xmm6,%xmm6
.byte	143,232,120,194,219,7
	vmovdqa	%xmm5,48(%ebx)
.byte	143,232,120,194,246,16
	vmovdqa	%xmm3,-32(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	16(%ebx),%xmm5
	vpxor	%xmm4,%xmm2,%xmm2
	vmovdqa	-80(%ebx),%xmm1
.byte	143,232,120,194,210,12
	vmovdqa	-64(%ebx),%xmm3
	vpaddd	%xmm2,%xmm0,%xmm0
	vmovdqa	96(%ebx),%xmm7
	vpxor	%xmm0,%xmm6,%xmm6
	vpaddd	%xmm3,%xmm1,%xmm1
.byte	143,232,120,194,246,8
	vmovdqa	%xmm0,-96(%ebx)
	vpaddd	%xmm6,%xmm4,%xmm4
	vmovdqa	%xmm6,80(%ebx)
	vpxor	%xmm4,%xmm2,%xmm2
	vpxor	%xmm1,%xmm7,%xmm7
.byte	143,232,120,194,210,7
.byte	143,232,120,194,255,16
	vmovdqa	%xmm2,-16(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vpxor	%xmm5,%xmm3,%xmm3
	vmovdqa	-128(%ebx),%xmm0
.byte	143,232,120,194,219,12
	vpaddd	%xmm3,%xmm1,%xmm1
	vmovdqa	64(%ebx),%xmm6
	vpxor	%xmm1,%xmm7,%xmm7
.byte	143,232,120,194,255,8
	vmovdqa	%xmm1,-80(%ebx)
	vpaddd	%xmm7,%xmm5,%xmm5
	vmovdqa	%xmm7,96(%ebx)
	vpxor	%xmm5,%xmm3,%xmm3
.byte	143,232,120,194,219,7
	decl	%edx
	jnz	.L018loop
	vmovdqa	%xmm3,-64(%ebx)
	vmovdqa	%xmm4,(%ebx)
	vmovdqa	%xmm5,16(%ebx)
	vmovdqa	%xmm6,64(%ebx)
	vmovdqa	%xmm7,96(%ebx)
	vmovdqa	-112(%ebx),%xmm1
	vmovdqa	-96(%ebx),%xmm2
	vmovdqa	-80(%ebx),%xmm3
	vpaddd	-128(%ebp),%xmm0,%xmm0
	vpaddd	-112(%ebp),%xmm1,%xmm1
	vpaddd	-96(%ebp),%xmm2,%xmm2
	vpaddd	-80(%ebp),%xmm3,%xmm3
	vpunpckldq	%xmm1,%xmm0,%xmm6
	vpunpckldq	%xmm3,%xmm2,%xmm7
	vpunpckhdq	%xmm1,%xmm0,%xmm0
	vpunpckhdq	%xmm3,%xmm2,%xmm2
	vpunpcklqdq	%xmm7,%xmm6,%xmm1
	vpunpckhqdq	%xmm7,%xmm6,%xmm6
	vpunpcklqdq	%xmm2,%xmm0,%xmm7
	vpunpckhqdq	%xmm2,%xmm0,%xmm3
	vpxor	-128(%esi),%xmm1,%xmm4
	vpxor	-64(%esi),%xmm6,%xmm5
	vpxor	(%esi),%xmm7,%xmm6
	vpxor	64(%esi),%xmm3,%xmm7
	leal	16(%esi),%esi
	vmovdqa	-64(%ebx),%xmm0
	vmovdqa	-48(%ebx),%xmm1
	vmovdqa	-32(%ebx),%xmm2
	vmovdqa	-16(%ebx),%xmm3
	vmovdqu	%xmm4,-128(%edi)
	vmovdqu	%xmm5,-64(%edi)
	vmovdqu	%xmm6,(%edi)
	vmovdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	vpaddd	-64(%ebp),%xmm0,%xmm0
	vpaddd	-48(%ebp),%xmm1,%xmm1
	vpaddd	-32(%ebp),%xmm2,%xmm2
	vpaddd	-16(%ebp),%xmm3,%xmm3
	vpunpckldq	%xmm1,%xmm0,%xmm6
	vpunpckldq	%xmm3,%xmm2,%xmm7
	vpunpckhdq	%xmm1,%xmm0,%xmm0
	vpunpckhdq	%xmm3,%xmm2,%xmm2
	vpunpcklqdq	%xmm7,%xmm6,%xmm1
	vpunpckhqdq	%xmm7,%xmm6,%xmm6
	vpunpcklqdq	%xmm2,%xmm0,%xmm7
	vpunpckhqdq	%xmm2,%xmm0,%xmm3
	vpxor	-128(%esi),%xmm1,%xmm4
	vpxor	-64(%esi),%xmm6,%xmm5
	vpxor	(%esi),%xmm7,%xmm6
	vpxor	64(%esi),%xmm3,%xmm7
	leal	16(%esi),%esi
	vmovdqa	(%ebx),%xmm0
	vmovdqa	16(%ebx),%xmm1
	vmovdqa	32(%ebx),%xmm2
	vmovdqa	48(%ebx),%xmm3
	vmovdqu	%xmm4,-128(%edi)
	vmovdqu	%xmm5,-64(%edi)
	vmovdqu	%xmm6,(%edi)
	vmovdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	vpaddd	(%ebp),%xmm0,%xmm0
	vpaddd	16(%ebp),%xmm1,%xmm1
	vpaddd	32(%ebp),%xmm2,%xmm2
	vpaddd	48(%ebp),%xmm3,%xmm3
	vpunpckldq	%xmm1,%xmm0,%xmm6
	vpunpckldq	%xmm3,%xmm2,%xmm7
	vpunpckhdq	%xmm1,%xmm0,%xmm0
	vpunpckhdq	%xmm3,%xmm2,%xmm2
	vpunpcklqdq	%xmm7,%xmm6,%xmm1
	vpunpckhqdq	%xmm7,%xmm6,%xmm6
	vpunpcklqdq	%xmm2,%xmm0,%xmm7
	vpunpckhqdq	%xmm2,%xmm0,%xmm3
	vpxor	-128(%esi),%xmm1,%xmm4
	vpxor	-64(%esi),%xmm6,%xmm5
	vpxor	(%esi),%xmm7,%xmm6
	vpxor	64(%esi),%xmm3,%xmm7
	leal	16(%esi),%esi
	vmovdqa	64(%ebx),%xmm0
	vmovdqa	80(%ebx),%xmm1
	vmovdqa	96(%ebx),%xmm2
	vmovdqa	112(%ebx),%xmm3
	vmovdqu	%xmm4,-128(%edi)
	vmovdqu	%xmm5,-64(%edi)
	vmovdqu	%xmm6,(%edi)
	vmovdqu	%xmm7,64(%edi)
	leal	16(%edi),%edi
	vpaddd	64(%ebp),%xmm0,%xmm0
	vpaddd	80(%ebp),%xmm1,%xmm1
	vpaddd	96(%ebp),%xmm2,%xmm2
	vpaddd	112(%ebp),%xmm3,%xmm3
	vpunpckldq	%xmm1,%xmm0,%xmm6
	vpunpckldq	%xmm3,%xmm2,%xmm7
	vpunpckhdq	%xmm1,%xmm0,%xmm0
	vpunpckhdq	%xmm3,%xmm2,%xmm2
	vpunpcklqdq	%xmm7,%xmm6,%xmm1
	vpunpckhqdq	%xmm7,%xmm6,%xmm6
	vpunpcklqdq	%xmm2,%xmm0,%xmm7
	vpunpckhqdq	%xmm2,%xmm0,%xmm3
	vpxor	-128(%esi),%xmm1,%xmm4
	vpxor	-64(%esi),%xmm6,%xmm5
	vpxor	(%esi),%xmm7,%xmm6
	vpxor	64(%esi),%xmm3,%xmm7
	leal	208(%esi),%esi
	vmovdqu	%xmm4,-128(%edi)
	vmovdqu	%xmm5,-64(%edi)
	vmovdqu	%xmm6,(%edi)
	vmovdqu	%xmm7,64(%edi)
	leal	208(%edi),%edi
	subl	$256,%ecx
	jnc	.L017outer_loop
	addl	$256,%ecx
	jz	.L019done
	movl	520(%esp),%ebx
	leal	-128(%esi),%esi
	movl	516(%esp),%edx
	leal	-128(%edi),%edi
	vmovd	64(%ebp),%xmm2
	vmovdqu	(%ebx),%xmm3
	vpaddd	96(%eax),%xmm2,%xmm2
	vpand	112(%eax),%xmm3,%xmm3
	vpor	%xmm2,%xmm3,%xmm3
.L0161x:
	vmovdqa	32(%eax),%xmm0
	vmovdqu	(%edx),%xmm1
	vmovdqu	16(%edx),%xmm2
	vmovdqa	(%eax),%xmm6
	vmovdqa	16(%eax),%xmm7
	movl	%ebp,48(%esp)
	vmovdqa	%xmm0,(%esp)
	vmovdqa	%xmm1,16(%esp)
	vmovdqa	%xmm2,32(%esp)
	vmovdqa	%xmm3,48(%esp)
	movl	$10,%edx
	jmp	.L020loop1x
.align	16
.L021outer1x:
	vmovdqa	80(%eax),%xmm3
	vmovdqa	(%esp),%xmm0
	vmovdqa	16(%esp),%xmm1
	vmovdqa	32(%esp),%xmm2
	vpaddd	48(%esp),%xmm3,%xmm3
	movl	$10,%edx
	vmovdqa	%xmm3,48(%esp)
	jmp	.L020loop1x
.align	16
.L020loop1x:
	vpaddd	%xmm1,%xmm0,%xmm0
	vpxor	%xmm0,%xmm3,%xmm3
.byte	143,232,120,194,219,16
	vpaddd	%xmm3,%xmm2,%xmm2
	vpxor	%xmm2,%xmm1,%xmm1
.byte	143,232,120,194,201,12
	vpaddd	%xmm1,%xmm0,%xmm0
	vpxor	%xmm0,%xmm3,%xmm3
.byte	143,232,120,194,219,8
	vpaddd	%xmm3,%xmm2,%xmm2
	vpxor	%xmm2,%xmm1,%xmm1
.byte	143,232,120,194,201,7
	vpshufd	$78,%xmm2,%xmm2
	vpshufd	$57,%xmm1,%xmm1
	vpshufd	$147,%xmm3,%xmm3
	vpaddd	%xmm1,%xmm0,%xmm0
	vpxor	%xmm0,%xmm3,%xmm3
.byte	143,232,120,194,219,16
	vpaddd	%xmm3,%xmm2,%xmm2
	vpxor	%xmm2,%xmm1,%xmm1
.byte	143,232,120,194,201,12
	vpaddd	%xmm1,%xmm0,%xmm0
	vpxor	%xmm0,%xmm3,%xmm3
.byte	143,232,120,194,219,8
	vpaddd	%xmm3,%xmm2,%xmm2
	vpxor	%xmm2,%xmm1,%xmm1
.byte	143,232,120,194,201,7
	vpshufd	$78,%xmm2,%xmm2
	vpshufd	$147,%xmm1,%xmm1
	vpshufd	$57,%xmm3,%xmm3
	decl	%edx
	jnz	.L020loop1x
	vpaddd	(%esp),%xmm0,%xmm0
	vpaddd	16(%esp),%xmm1,%xmm1
	vpaddd	32(%esp),%xmm2,%xmm2
	vpaddd	48(%esp),%xmm3,%xmm3
	cmpl	$64,%ecx
	jb	.L022tail
	vpxor	(%esi),%xmm0,%xmm0
	vpxor	16(%esi),%xmm1,%xmm1
	vpxor	32(%esi),%xmm2,%xmm2
	vpxor	48(%esi),%xmm3,%xmm3
	leal	64(%esi),%esi
	vmovdqu	%xmm0,(%edi)
	vmovdqu	%xmm1,16(%edi)
	vmovdqu	%xmm2,32(%edi)
	vmovdqu	%xmm3,48(%edi)
	leal	64(%edi),%edi
	subl	$64,%ecx
	jnz	.L021outer1x
	jmp	.L019done
.L022tail:
	vmovdqa	%xmm0,(%esp)
	vmovdqa	%xmm1,16(%esp)
	vmovdqa	%xmm2,32(%esp)
	vmovdqa	%xmm3,48(%esp)
	xorl	%eax,%eax
	xorl	%edx,%edx
	xorl	%ebp,%ebp
.L023tail_loop:
	movb	(%esp,%ebp,1),%al
	movb	(%esi,%ebp,1),%dl
	leal	1(%ebp),%ebp
	xorb	%dl,%al
	movb	%al,-1(%edi,%ebp,1)
	decl	%ecx
	jnz	.L023tail_loop
.L019done:
	vzeroupper
	movl	512(%esp),%esp
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.size	ChaCha20_xop,.-.L_ChaCha20_xop_begin
.comm	OPENSSL_ia32cap_P,16,4
