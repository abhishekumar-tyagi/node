.file	"../openssl/crypto/bn/asm/x86-mont.s"
.text
.globl	_bn_mul_mont
.align	4
_bn_mul_mont:
L_bn_mul_mont_begin:
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	xorl	%eax,%eax
	movl	40(%esp),%edi
	cmpl	$4,%edi
	jl	L000just_leave
	leal	20(%esp),%esi
	leal	24(%esp),%edx
	movl	%esp,%ebp
	addl	$2,%edi
	negl	%edi
	leal	-32(%esp,%edi,4),%esp
	negl	%edi
	movl	%esp,%eax
	subl	%edx,%eax
	andl	$2047,%eax
	subl	%eax,%esp
	xorl	%esp,%edx
	andl	$2048,%edx
	xorl	$2048,%edx
	subl	%edx,%esp
	andl	$-64,%esp
	movl	%ebp,%eax
	subl	%esp,%eax
	andl	$-4096,%eax
L001page_walk:
	movl	(%esp,%eax,1),%edx
	subl	$4096,%eax
.byte	46
	jnc	L001page_walk
	movl	(%esi),%eax
	movl	4(%esi),%ebx
	movl	8(%esi),%ecx
	movl	12(%esi),%edx
	movl	16(%esi),%esi
	movl	(%esi),%esi
	movl	%eax,4(%esp)
	movl	%ebx,8(%esp)
	movl	%ecx,12(%esp)
	movl	%edx,16(%esp)
	movl	%esi,20(%esp)
	leal	-3(%edi),%ebx
	movl	%ebp,24(%esp)
	call	L002PIC_me_up
L002PIC_me_up:
	popl	%eax
	movl	L_OPENSSL_ia32cap_P$non_lazy_ptr-L002PIC_me_up(%eax),%eax
	btl	$26,(%eax)
	jnc	L003non_sse2
	movl	$-1,%eax
	movd	%eax,%mm7
	movl	8(%esp),%esi
	movl	12(%esp),%edi
	movl	16(%esp),%ebp
	xorl	%edx,%edx
	xorl	%ecx,%ecx
	movd	(%edi),%mm4
	movd	(%esi),%mm5
	movd	(%ebp),%mm3
	pmuludq	%mm4,%mm5
	movq	%mm5,%mm2
	movq	%mm5,%mm0
	pand	%mm7,%mm0
	pmuludq	20(%esp),%mm5
	pmuludq	%mm5,%mm3
	paddq	%mm0,%mm3
	movd	4(%ebp),%mm1
	movd	4(%esi),%mm0
	psrlq	$32,%mm2
	psrlq	$32,%mm3
	incl	%ecx
.align	4,0x90
L0041st:
	pmuludq	%mm4,%mm0
	pmuludq	%mm5,%mm1
	paddq	%mm0,%mm2
	paddq	%mm1,%mm3
	movq	%mm2,%mm0
	pand	%mm7,%mm0
	movd	4(%ebp,%ecx,4),%mm1
	paddq	%mm0,%mm3
	movd	4(%esi,%ecx,4),%mm0
	psrlq	$32,%mm2
	movd	%mm3,28(%esp,%ecx,4)
	psrlq	$32,%mm3
	leal	1(%ecx),%ecx
	cmpl	%ebx,%ecx
	jl	L0041st
	pmuludq	%mm4,%mm0
	pmuludq	%mm5,%mm1
	paddq	%mm0,%mm2
	paddq	%mm1,%mm3
	movq	%mm2,%mm0
	pand	%mm7,%mm0
	paddq	%mm0,%mm3
	movd	%mm3,28(%esp,%ecx,4)
	psrlq	$32,%mm2
	psrlq	$32,%mm3
	paddq	%mm2,%mm3
	movq	%mm3,32(%esp,%ebx,4)
	incl	%edx
L005outer:
	xorl	%ecx,%ecx
	movd	(%edi,%edx,4),%mm4
	movd	(%esi),%mm5
	movd	32(%esp),%mm6
	movd	(%ebp),%mm3
	pmuludq	%mm4,%mm5
	paddq	%mm6,%mm5
	movq	%mm5,%mm0
	movq	%mm5,%mm2
	pand	%mm7,%mm0
	pmuludq	20(%esp),%mm5
	pmuludq	%mm5,%mm3
	paddq	%mm0,%mm3
	movd	36(%esp),%mm6
	movd	4(%ebp),%mm1
	movd	4(%esi),%mm0
	psrlq	$32,%mm2
	psrlq	$32,%mm3
	paddq	%mm6,%mm2
	incl	%ecx
	decl	%ebx
L006inner:
	pmuludq	%mm4,%mm0
	pmuludq	%mm5,%mm1
	paddq	%mm0,%mm2
	paddq	%mm1,%mm3
	movq	%mm2,%mm0
	movd	36(%esp,%ecx,4),%mm6
	pand	%mm7,%mm0
	movd	4(%ebp,%ecx,4),%mm1
	paddq	%mm0,%mm3
	movd	4(%esi,%ecx,4),%mm0
	psrlq	$32,%mm2
	movd	%mm3,28(%esp,%ecx,4)
	psrlq	$32,%mm3
	paddq	%mm6,%mm2
	decl	%ebx
	leal	1(%ecx),%ecx
	jnz	L006inner
	movl	%ecx,%ebx
	pmuludq	%mm4,%mm0
	pmuludq	%mm5,%mm1
	paddq	%mm0,%mm2
	paddq	%mm1,%mm3
	movq	%mm2,%mm0
	pand	%mm7,%mm0
	paddq	%mm0,%mm3
	movd	%mm3,28(%esp,%ecx,4)
	psrlq	$32,%mm2
	psrlq	$32,%mm3
	movd	36(%esp,%ebx,4),%mm6
	paddq	%mm2,%mm3
	paddq	%mm6,%mm3
	movq	%mm3,32(%esp,%ebx,4)
	leal	1(%edx),%edx
	cmpl	%ebx,%edx
	jle	L005outer
	emms
	jmp	L007common_tail
.align	4,0x90
L003non_sse2:
	movl	8(%esp),%esi
	leal	1(%ebx),%ebp
	movl	12(%esp),%edi
	xorl	%ecx,%ecx
	movl	%esi,%edx
	andl	$1,%ebp
	subl	%edi,%edx
	leal	4(%edi,%ebx,4),%eax
	orl	%edx,%ebp
	movl	(%edi),%edi
	jz	L008bn_sqr_mont
	movl	%eax,28(%esp)
	movl	(%esi),%eax
	xorl	%edx,%edx
.align	4,0x90
L009mull:
	movl	%edx,%ebp
	mull	%edi
	addl	%eax,%ebp
	leal	1(%ecx),%ecx
	adcl	$0,%edx
	movl	(%esi,%ecx,4),%eax
	cmpl	%ebx,%ecx
	movl	%ebp,28(%esp,%ecx,4)
	jl	L009mull
	movl	%edx,%ebp
	mull	%edi
	movl	20(%esp),%edi
	addl	%ebp,%eax
	movl	16(%esp),%esi
	adcl	$0,%edx
	imull	32(%esp),%edi
	movl	%eax,32(%esp,%ebx,4)
	xorl	%ecx,%ecx
	movl	%edx,36(%esp,%ebx,4)
	movl	%ecx,40(%esp,%ebx,4)
	movl	(%esi),%eax
	mull	%edi
	addl	32(%esp),%eax
	movl	4(%esi),%eax
	adcl	$0,%edx
	incl	%ecx
	jmp	L0102ndmadd
.align	4,0x90
L0111stmadd:
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ecx,4),%ebp
	leal	1(%ecx),%ecx
	adcl	$0,%edx
	addl	%eax,%ebp
	movl	(%esi,%ecx,4),%eax
	adcl	$0,%edx
	cmpl	%ebx,%ecx
	movl	%ebp,28(%esp,%ecx,4)
	jl	L0111stmadd
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ebx,4),%eax
	movl	20(%esp),%edi
	adcl	$0,%edx
	movl	16(%esp),%esi
	addl	%eax,%ebp
	adcl	$0,%edx
	imull	32(%esp),%edi
	xorl	%ecx,%ecx
	addl	36(%esp,%ebx,4),%edx
	movl	%ebp,32(%esp,%ebx,4)
	adcl	$0,%ecx
	movl	(%esi),%eax
	movl	%edx,36(%esp,%ebx,4)
	movl	%ecx,40(%esp,%ebx,4)
	mull	%edi
	addl	32(%esp),%eax
	movl	4(%esi),%eax
	adcl	$0,%edx
	movl	$1,%ecx
.align	4,0x90
L0102ndmadd:
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ecx,4),%ebp
	leal	1(%ecx),%ecx
	adcl	$0,%edx
	addl	%eax,%ebp
	movl	(%esi,%ecx,4),%eax
	adcl	$0,%edx
	cmpl	%ebx,%ecx
	movl	%ebp,24(%esp,%ecx,4)
	jl	L0102ndmadd
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ebx,4),%ebp
	adcl	$0,%edx
	addl	%eax,%ebp
	adcl	$0,%edx
	movl	%ebp,28(%esp,%ebx,4)
	xorl	%eax,%eax
	movl	12(%esp),%ecx
	addl	36(%esp,%ebx,4),%edx
	adcl	40(%esp,%ebx,4),%eax
	leal	4(%ecx),%ecx
	movl	%edx,32(%esp,%ebx,4)
	cmpl	28(%esp),%ecx
	movl	%eax,36(%esp,%ebx,4)
	je	L007common_tail
	movl	(%ecx),%edi
	movl	8(%esp),%esi
	movl	%ecx,12(%esp)
	xorl	%ecx,%ecx
	xorl	%edx,%edx
	movl	(%esi),%eax
	jmp	L0111stmadd
.align	4,0x90
L008bn_sqr_mont:
	movl	%ebx,(%esp)
	movl	%ecx,12(%esp)
	movl	%edi,%eax
	mull	%edi
	movl	%eax,32(%esp)
	movl	%edx,%ebx
	shrl	$1,%edx
	andl	$1,%ebx
	incl	%ecx
.align	4,0x90
L012sqr:
	movl	(%esi,%ecx,4),%eax
	movl	%edx,%ebp
	mull	%edi
	addl	%ebp,%eax
	leal	1(%ecx),%ecx
	adcl	$0,%edx
	leal	(%ebx,%eax,2),%ebp
	shrl	$31,%eax
	cmpl	(%esp),%ecx
	movl	%eax,%ebx
	movl	%ebp,28(%esp,%ecx,4)
	jl	L012sqr
	movl	(%esi,%ecx,4),%eax
	movl	%edx,%ebp
	mull	%edi
	addl	%ebp,%eax
	movl	20(%esp),%edi
	adcl	$0,%edx
	movl	16(%esp),%esi
	leal	(%ebx,%eax,2),%ebp
	imull	32(%esp),%edi
	shrl	$31,%eax
	movl	%ebp,32(%esp,%ecx,4)
	leal	(%eax,%edx,2),%ebp
	movl	(%esi),%eax
	shrl	$31,%edx
	movl	%ebp,36(%esp,%ecx,4)
	movl	%edx,40(%esp,%ecx,4)
	mull	%edi
	addl	32(%esp),%eax
	movl	%ecx,%ebx
	adcl	$0,%edx
	movl	4(%esi),%eax
	movl	$1,%ecx
.align	4,0x90
L0133rdmadd:
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ecx,4),%ebp
	adcl	$0,%edx
	addl	%eax,%ebp
	movl	4(%esi,%ecx,4),%eax
	adcl	$0,%edx
	movl	%ebp,28(%esp,%ecx,4)
	movl	%edx,%ebp
	mull	%edi
	addl	36(%esp,%ecx,4),%ebp
	leal	2(%ecx),%ecx
	adcl	$0,%edx
	addl	%eax,%ebp
	movl	(%esi,%ecx,4),%eax
	adcl	$0,%edx
	cmpl	%ebx,%ecx
	movl	%ebp,24(%esp,%ecx,4)
	jl	L0133rdmadd
	movl	%edx,%ebp
	mull	%edi
	addl	32(%esp,%ebx,4),%ebp
	adcl	$0,%edx
	addl	%eax,%ebp
	adcl	$0,%edx
	movl	%ebp,28(%esp,%ebx,4)
	movl	12(%esp),%ecx
	xorl	%eax,%eax
	movl	8(%esp),%esi
	addl	36(%esp,%ebx,4),%edx
	adcl	40(%esp,%ebx,4),%eax
	movl	%edx,32(%esp,%ebx,4)
	cmpl	%ebx,%ecx
	movl	%eax,36(%esp,%ebx,4)
	je	L007common_tail
	movl	4(%esi,%ecx,4),%edi
	leal	1(%ecx),%ecx
	movl	%edi,%eax
	movl	%ecx,12(%esp)
	mull	%edi
	addl	32(%esp,%ecx,4),%eax
	adcl	$0,%edx
	movl	%eax,32(%esp,%ecx,4)
	xorl	%ebp,%ebp
	cmpl	%ebx,%ecx
	leal	1(%ecx),%ecx
	je	L014sqrlast
	movl	%edx,%ebx
	shrl	$1,%edx
	andl	$1,%ebx
.align	4,0x90
L015sqradd:
	movl	(%esi,%ecx,4),%eax
	movl	%edx,%ebp
	mull	%edi
	addl	%ebp,%eax
	leal	(%eax,%eax,1),%ebp
	adcl	$0,%edx
	shrl	$31,%eax
	addl	32(%esp,%ecx,4),%ebp
	leal	1(%ecx),%ecx
	adcl	$0,%eax
	addl	%ebx,%ebp
	adcl	$0,%eax
	cmpl	(%esp),%ecx
	movl	%ebp,28(%esp,%ecx,4)
	movl	%eax,%ebx
	jle	L015sqradd
	movl	%edx,%ebp
	addl	%edx,%edx
	shrl	$31,%ebp
	addl	%ebx,%edx
	adcl	$0,%ebp
L014sqrlast:
	movl	20(%esp),%edi
	movl	16(%esp),%esi
	imull	32(%esp),%edi
	addl	32(%esp,%ecx,4),%edx
	movl	(%esi),%eax
	adcl	$0,%ebp
	movl	%edx,32(%esp,%ecx,4)
	movl	%ebp,36(%esp,%ecx,4)
	mull	%edi
	addl	32(%esp),%eax
	leal	-1(%ecx),%ebx
	adcl	$0,%edx
	movl	$1,%ecx
	movl	4(%esi),%eax
	jmp	L0133rdmadd
.align	4,0x90
L007common_tail:
	movl	16(%esp),%ebp
	movl	4(%esp),%edi
	leal	32(%esp),%esi
	movl	(%esi),%eax
	movl	%ebx,%ecx
	xorl	%edx,%edx
.align	4,0x90
L016sub:
	sbbl	(%ebp,%edx,4),%eax
	movl	%eax,(%edi,%edx,4)
	decl	%ecx
	movl	4(%esi,%edx,4),%eax
	leal	1(%edx),%edx
	jge	L016sub
	sbbl	$0,%eax
	andl	%eax,%esi
	notl	%eax
	movl	%edi,%ebp
	andl	%eax,%ebp
	orl	%ebp,%esi
.align	4,0x90
L017copy:
	movl	(%esi,%ebx,4),%eax
	movl	%eax,(%edi,%ebx,4)
	movl	%ecx,32(%esp,%ebx,4)
	decl	%ebx
	jge	L017copy
	movl	24(%esp),%esp
	movl	$1,%eax
L000just_leave:
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.byte	77,111,110,116,103,111,109,101,114,121,32,77,117,108,116,105
.byte	112,108,105,99,97,116,105,111,110,32,102,111,114,32,120,56
.byte	54,44,32,67,82,89,80,84,79,71,65,77,83,32,98,121
.byte	32,60,97,112,112,114,111,64,111,112,101,110,115,115,108,46
.byte	111,114,103,62,0
.section __IMPORT,__pointers,non_lazy_symbol_pointers
L_OPENSSL_ia32cap_P$non_lazy_ptr:
.indirect_symbol	_OPENSSL_ia32cap_P
.long	0
.comm	_OPENSSL_ia32cap_P,16,2
