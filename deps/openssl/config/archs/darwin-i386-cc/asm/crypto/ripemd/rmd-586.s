.text
.globl	_ripemd160_block_asm_data_order
.align	4
_ripemd160_block_asm_data_order:
L_ripemd160_block_asm_data_order_begin:
.byte	243,15,30,251
	movl	4(%esp),%edx
	movl	8(%esp),%eax
	pushl	%esi
	movl	(%edx),%ecx
	pushl	%edi
	movl	4(%edx),%esi
	pushl	%ebp
	movl	8(%edx),%edi
	pushl	%ebx
	subl	$108,%esp
L000start:

	movl	(%eax),%ebx
	movl	4(%eax),%ebp
	movl	%ebx,(%esp)
	movl	%ebp,4(%esp)
	movl	8(%eax),%ebx
	movl	12(%eax),%ebp
	movl	%ebx,8(%esp)
	movl	%ebp,12(%esp)
	movl	16(%eax),%ebx
	movl	20(%eax),%ebp
	movl	%ebx,16(%esp)
	movl	%ebp,20(%esp)
	movl	24(%eax),%ebx
	movl	28(%eax),%ebp
	movl	%ebx,24(%esp)
	movl	%ebp,28(%esp)
	movl	32(%eax),%ebx
	movl	36(%eax),%ebp
	movl	%ebx,32(%esp)
	movl	%ebp,36(%esp)
	movl	40(%eax),%ebx
	movl	44(%eax),%ebp
	movl	%ebx,40(%esp)
	movl	%ebp,44(%esp)
	movl	48(%eax),%ebx
	movl	52(%eax),%ebp
	movl	%ebx,48(%esp)
	movl	%ebp,52(%esp)
	movl	56(%eax),%ebx
	movl	60(%eax),%ebp
	movl	%ebx,56(%esp)
	movl	%ebp,60(%esp)
	movl	%edi,%eax
	movl	12(%edx),%ebx
	movl	16(%edx),%ebp
	# 0 
	xorl	%ebx,%eax
	movl	(%esp),%edx
	xorl	%esi,%eax
	addl	%edx,%ecx
	roll	$10,%edi
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$11,%ecx
	addl	%ebp,%ecx
	# 1 
	xorl	%edi,%eax
	movl	4(%esp),%edx
	xorl	%ecx,%eax
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$10,%esi
	addl	%edx,%ebp
	xorl	%esi,%eax
	roll	$14,%ebp
	addl	%ebx,%ebp
	# 2 
	movl	8(%esp),%edx
	xorl	%ebp,%eax
	addl	%edx,%ebx
	roll	$10,%ecx
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$15,%ebx
	addl	%edi,%ebx
	# 3 
	xorl	%ecx,%eax
	movl	12(%esp),%edx
	xorl	%ebx,%eax
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$10,%ebp
	addl	%edx,%edi
	xorl	%ebp,%eax
	roll	$12,%edi
	addl	%esi,%edi
	# 4 
	movl	16(%esp),%edx
	xorl	%edi,%eax
	addl	%edx,%esi
	roll	$10,%ebx
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$5,%esi
	addl	%ecx,%esi
	# 5 
	xorl	%ebx,%eax
	movl	20(%esp),%edx
	xorl	%esi,%eax
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$10,%edi
	addl	%edx,%ecx
	xorl	%edi,%eax
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 6 
	movl	24(%esp),%edx
	xorl	%ecx,%eax
	addl	%edx,%ebp
	roll	$10,%esi
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$7,%ebp
	addl	%ebx,%ebp
	# 7 
	xorl	%esi,%eax
	movl	28(%esp),%edx
	xorl	%ebp,%eax
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$10,%ecx
	addl	%edx,%ebx
	xorl	%ecx,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 8 
	movl	32(%esp),%edx
	xorl	%ebx,%eax
	addl	%edx,%edi
	roll	$10,%ebp
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$11,%edi
	addl	%esi,%edi
	# 9 
	xorl	%ebp,%eax
	movl	36(%esp),%edx
	xorl	%edi,%eax
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$10,%ebx
	addl	%edx,%esi
	xorl	%ebx,%eax
	roll	$13,%esi
	addl	%ecx,%esi
	# 10 
	movl	40(%esp),%edx
	xorl	%esi,%eax
	addl	%edx,%ecx
	roll	$10,%edi
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 11 
	xorl	%edi,%eax
	movl	44(%esp),%edx
	xorl	%ecx,%eax
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$10,%esi
	addl	%edx,%ebp
	xorl	%esi,%eax
	roll	$15,%ebp
	addl	%ebx,%ebp
	# 12 
	movl	48(%esp),%edx
	xorl	%ebp,%eax
	addl	%edx,%ebx
	roll	$10,%ecx
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$6,%ebx
	addl	%edi,%ebx
	# 13 
	xorl	%ecx,%eax
	movl	52(%esp),%edx
	xorl	%ebx,%eax
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$10,%ebp
	addl	%edx,%edi
	xorl	%ebp,%eax
	roll	$7,%edi
	addl	%esi,%edi
	# 14 
	movl	56(%esp),%edx
	xorl	%edi,%eax
	addl	%edx,%esi
	roll	$10,%ebx
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$9,%esi
	addl	%ecx,%esi
	# 15 
	xorl	%ebx,%eax
	movl	60(%esp),%edx
	xorl	%esi,%eax
	addl	%eax,%ecx
	movl	$-1,%eax
	roll	$10,%edi
	addl	%edx,%ecx
	movl	28(%esp),%edx
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 16 
	addl	%edx,%ebp
	movl	%esi,%edx
	subl	%ecx,%eax
	andl	%ecx,%edx
	andl	%edi,%eax
	orl	%eax,%edx
	movl	16(%esp),%eax
	roll	$10,%esi
	leal	1518500249(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	roll	$7,%ebp
	addl	%ebx,%ebp
	# 17 
	addl	%eax,%ebx
	movl	%ecx,%eax
	subl	%ebp,%edx
	andl	%ebp,%eax
	andl	%esi,%edx
	orl	%edx,%eax
	movl	52(%esp),%edx
	roll	$10,%ecx
	leal	1518500249(%ebx,%eax,1),%ebx
	movl	$-1,%eax
	roll	$6,%ebx
	addl	%edi,%ebx
	# 18 
	addl	%edx,%edi
	movl	%ebp,%edx
	subl	%ebx,%eax
	andl	%ebx,%edx
	andl	%ecx,%eax
	orl	%eax,%edx
	movl	4(%esp),%eax
	roll	$10,%ebp
	leal	1518500249(%edi,%edx,1),%edi
	movl	$-1,%edx
	roll	$8,%edi
	addl	%esi,%edi
	# 19 
	addl	%eax,%esi
	movl	%ebx,%eax
	subl	%edi,%edx
	andl	%edi,%eax
	andl	%ebp,%edx
	orl	%edx,%eax
	movl	40(%esp),%edx
	roll	$10,%ebx
	leal	1518500249(%esi,%eax,1),%esi
	movl	$-1,%eax
	roll	$13,%esi
	addl	%ecx,%esi
	# 20 
	addl	%edx,%ecx
	movl	%edi,%edx
	subl	%esi,%eax
	andl	%esi,%edx
	andl	%ebx,%eax
	orl	%eax,%edx
	movl	24(%esp),%eax
	roll	$10,%edi
	leal	1518500249(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	roll	$11,%ecx
	addl	%ebp,%ecx
	# 21 
	addl	%eax,%ebp
	movl	%esi,%eax
	subl	%ecx,%edx
	andl	%ecx,%eax
	andl	%edi,%edx
	orl	%edx,%eax
	movl	60(%esp),%edx
	roll	$10,%esi
	leal	1518500249(%ebp,%eax,1),%ebp
	movl	$-1,%eax
	roll	$9,%ebp
	addl	%ebx,%ebp
	# 22 
	addl	%edx,%ebx
	movl	%ecx,%edx
	subl	%ebp,%eax
	andl	%ebp,%edx
	andl	%esi,%eax
	orl	%eax,%edx
	movl	12(%esp),%eax
	roll	$10,%ecx
	leal	1518500249(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	roll	$7,%ebx
	addl	%edi,%ebx
	# 23 
	addl	%eax,%edi
	movl	%ebp,%eax
	subl	%ebx,%edx
	andl	%ebx,%eax
	andl	%ecx,%edx
	orl	%edx,%eax
	movl	48(%esp),%edx
	roll	$10,%ebp
	leal	1518500249(%edi,%eax,1),%edi
	movl	$-1,%eax
	roll	$15,%edi
	addl	%esi,%edi
	# 24 
	addl	%edx,%esi
	movl	%ebx,%edx
	subl	%edi,%eax
	andl	%edi,%edx
	andl	%ebp,%eax
	orl	%eax,%edx
	movl	(%esp),%eax
	roll	$10,%ebx
	leal	1518500249(%esi,%edx,1),%esi
	movl	$-1,%edx
	roll	$7,%esi
	addl	%ecx,%esi
	# 25 
	addl	%eax,%ecx
	movl	%edi,%eax
	subl	%esi,%edx
	andl	%esi,%eax
	andl	%ebx,%edx
	orl	%edx,%eax
	movl	36(%esp),%edx
	roll	$10,%edi
	leal	1518500249(%ecx,%eax,1),%ecx
	movl	$-1,%eax
	roll	$12,%ecx
	addl	%ebp,%ecx
	# 26 
	addl	%edx,%ebp
	movl	%esi,%edx
	subl	%ecx,%eax
	andl	%ecx,%edx
	andl	%edi,%eax
	orl	%eax,%edx
	movl	20(%esp),%eax
	roll	$10,%esi
	leal	1518500249(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	roll	$15,%ebp
	addl	%ebx,%ebp
	# 27 
	addl	%eax,%ebx
	movl	%ecx,%eax
	subl	%ebp,%edx
	andl	%ebp,%eax
	andl	%esi,%edx
	orl	%edx,%eax
	movl	8(%esp),%edx
	roll	$10,%ecx
	leal	1518500249(%ebx,%eax,1),%ebx
	movl	$-1,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 28 
	addl	%edx,%edi
	movl	%ebp,%edx
	subl	%ebx,%eax
	andl	%ebx,%edx
	andl	%ecx,%eax
	orl	%eax,%edx
	movl	56(%esp),%eax
	roll	$10,%ebp
	leal	1518500249(%edi,%edx,1),%edi
	movl	$-1,%edx
	roll	$11,%edi
	addl	%esi,%edi
	# 29 
	addl	%eax,%esi
	movl	%ebx,%eax
	subl	%edi,%edx
	andl	%edi,%eax
	andl	%ebp,%edx
	orl	%edx,%eax
	movl	44(%esp),%edx
	roll	$10,%ebx
	leal	1518500249(%esi,%eax,1),%esi
	movl	$-1,%eax
	roll	$7,%esi
	addl	%ecx,%esi
	# 30 
	addl	%edx,%ecx
	movl	%edi,%edx
	subl	%esi,%eax
	andl	%esi,%edx
	andl	%ebx,%eax
	orl	%eax,%edx
	movl	32(%esp),%eax
	roll	$10,%edi
	leal	1518500249(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	roll	$13,%ecx
	addl	%ebp,%ecx
	# 31 
	addl	%eax,%ebp
	movl	%esi,%eax
	subl	%ecx,%edx
	andl	%ecx,%eax
	andl	%edi,%edx
	orl	%edx,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	1518500249(%ebp,%eax,1),%ebp
	subl	%ecx,%edx
	roll	$12,%ebp
	addl	%ebx,%ebp
	# 32 
	movl	12(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%ebx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1859775393(%ebx,%edx,1),%ebx
	subl	%ebp,%eax
	roll	$11,%ebx
	addl	%edi,%ebx
	# 33 
	movl	40(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%edi
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1859775393(%edi,%eax,1),%edi
	subl	%ebx,%edx
	roll	$13,%edi
	addl	%esi,%edi
	# 34 
	movl	56(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%esi
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1859775393(%esi,%edx,1),%esi
	subl	%edi,%eax
	roll	$6,%esi
	addl	%ecx,%esi
	# 35 
	movl	16(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ecx
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1859775393(%ecx,%eax,1),%ecx
	subl	%esi,%edx
	roll	$7,%ecx
	addl	%ebp,%ecx
	# 36 
	movl	36(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebp
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	1859775393(%ebp,%edx,1),%ebp
	subl	%ecx,%eax
	roll	$14,%ebp
	addl	%ebx,%ebp
	# 37 
	movl	60(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%ebx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	1859775393(%ebx,%eax,1),%ebx
	subl	%ebp,%edx
	roll	$9,%ebx
	addl	%edi,%ebx
	# 38 
	movl	32(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%edi
	xorl	%ecx,%edx
	movl	$-1,%eax
	roll	$10,%ebp
	leal	1859775393(%edi,%edx,1),%edi
	subl	%ebx,%eax
	roll	$13,%edi
	addl	%esi,%edi
	# 39 
	movl	4(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%esi
	xorl	%ebp,%eax
	movl	$-1,%edx
	roll	$10,%ebx
	leal	1859775393(%esi,%eax,1),%esi
	subl	%edi,%edx
	roll	$15,%esi
	addl	%ecx,%esi
	# 40 
	movl	8(%esp),%eax
	orl	%esi,%edx
	addl	%eax,%ecx
	xorl	%ebx,%edx
	movl	$-1,%eax
	roll	$10,%edi
	leal	1859775393(%ecx,%edx,1),%ecx
	subl	%esi,%eax
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 41 
	movl	28(%esp),%edx
	orl	%ecx,%eax
	addl	%edx,%ebp
	xorl	%edi,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	1859775393(%ebp,%eax,1),%ebp
	subl	%ecx,%edx
	roll	$8,%ebp
	addl	%ebx,%ebp
	# 42 
	movl	(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%ebx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1859775393(%ebx,%edx,1),%ebx
	subl	%ebp,%eax
	roll	$13,%ebx
	addl	%edi,%ebx
	# 43 
	movl	24(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%edi
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1859775393(%edi,%eax,1),%edi
	subl	%ebx,%edx
	roll	$6,%edi
	addl	%esi,%edi
	# 44 
	movl	52(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%esi
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1859775393(%esi,%edx,1),%esi
	subl	%edi,%eax
	roll	$5,%esi
	addl	%ecx,%esi
	# 45 
	movl	44(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ecx
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1859775393(%ecx,%eax,1),%ecx
	subl	%esi,%edx
	roll	$12,%ecx
	addl	%ebp,%ecx
	# 46 
	movl	20(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebp
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	1859775393(%ebp,%edx,1),%ebp
	subl	%ecx,%eax
	roll	$7,%ebp
	addl	%ebx,%ebp
	# 47 
	movl	48(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%ebx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	1859775393(%ebx,%eax,1),%ebx
	movl	%ecx,%eax
	roll	$5,%ebx
	addl	%edi,%ebx
	# 48 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	4(%esp),%eax
	roll	$10,%ebp
	leal	2400959708(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$11,%edi
	addl	%esi,%edi
	# 49 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	36(%esp),%eax
	roll	$10,%ebx
	leal	2400959708(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$12,%esi
	addl	%ecx,%esi
	# 50 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	44(%esp),%eax
	roll	$10,%edi
	leal	2400959708(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 51 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	40(%esp),%eax
	roll	$10,%esi
	leal	2400959708(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$15,%ebp
	addl	%ebx,%ebp
	# 52 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	(%esp),%eax
	roll	$10,%ecx
	leal	2400959708(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$14,%ebx
	addl	%edi,%ebx
	# 53 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	32(%esp),%eax
	roll	$10,%ebp
	leal	2400959708(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$15,%edi
	addl	%esi,%edi
	# 54 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	48(%esp),%eax
	roll	$10,%ebx
	leal	2400959708(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$9,%esi
	addl	%ecx,%esi
	# 55 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	16(%esp),%eax
	roll	$10,%edi
	leal	2400959708(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 56 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	52(%esp),%eax
	roll	$10,%esi
	leal	2400959708(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$9,%ebp
	addl	%ebx,%ebp
	# 57 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	12(%esp),%eax
	roll	$10,%ecx
	leal	2400959708(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$14,%ebx
	addl	%edi,%ebx
	# 58 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	28(%esp),%eax
	roll	$10,%ebp
	leal	2400959708(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$5,%edi
	addl	%esi,%edi
	# 59 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	60(%esp),%eax
	roll	$10,%ebx
	leal	2400959708(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$6,%esi
	addl	%ecx,%esi
	# 60 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	56(%esp),%eax
	roll	$10,%edi
	leal	2400959708(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 61 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	20(%esp),%eax
	roll	$10,%esi
	leal	2400959708(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$6,%ebp
	addl	%ebx,%ebp
	# 62 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	24(%esp),%eax
	roll	$10,%ecx
	leal	2400959708(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$5,%ebx
	addl	%edi,%ebx
	# 63 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	8(%esp),%eax
	roll	$10,%ebp
	leal	2400959708(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	subl	%ebp,%edx
	roll	$12,%edi
	addl	%esi,%edi
	# 64 
	movl	16(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%esi
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	2840853838(%esi,%edx,1),%esi
	subl	%ebx,%eax
	roll	$9,%esi
	addl	%ecx,%esi
	# 65 
	movl	(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%ecx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	2840853838(%ecx,%eax,1),%ecx
	subl	%edi,%edx
	roll	$15,%ecx
	addl	%ebp,%ecx
	# 66 
	movl	20(%esp),%eax
	orl	%esi,%edx
	addl	%eax,%ebp
	xorl	%ecx,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	2840853838(%ebp,%edx,1),%ebp
	subl	%esi,%eax
	roll	$5,%ebp
	addl	%ebx,%ebp
	# 67 
	movl	36(%esp),%edx
	orl	%ecx,%eax
	addl	%edx,%ebx
	xorl	%ebp,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	2840853838(%ebx,%eax,1),%ebx
	subl	%ecx,%edx
	roll	$11,%ebx
	addl	%edi,%ebx
	# 68 
	movl	28(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%edi
	xorl	%ebx,%edx
	movl	$-1,%eax
	roll	$10,%ebp
	leal	2840853838(%edi,%edx,1),%edi
	subl	%ebp,%eax
	roll	$6,%edi
	addl	%esi,%edi
	# 69 
	movl	48(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%esi
	xorl	%edi,%eax
	movl	$-1,%edx
	roll	$10,%ebx
	leal	2840853838(%esi,%eax,1),%esi
	subl	%ebx,%edx
	roll	$8,%esi
	addl	%ecx,%esi
	# 70 
	movl	8(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%ecx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%edi
	leal	2840853838(%ecx,%edx,1),%ecx
	subl	%edi,%eax
	roll	$13,%ecx
	addl	%ebp,%ecx
	# 71 
	movl	40(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ebp
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	2840853838(%ebp,%eax,1),%ebp
	subl	%esi,%edx
	roll	$12,%ebp
	addl	%ebx,%ebp
	# 72 
	movl	56(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebx
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	2840853838(%ebx,%edx,1),%ebx
	subl	%ecx,%eax
	roll	$5,%ebx
	addl	%edi,%ebx
	# 73 
	movl	4(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%edi
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	2840853838(%edi,%eax,1),%edi
	subl	%ebp,%edx
	roll	$12,%edi
	addl	%esi,%edi
	# 74 
	movl	12(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%esi
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	2840853838(%esi,%edx,1),%esi
	subl	%ebx,%eax
	roll	$13,%esi
	addl	%ecx,%esi
	# 75 
	movl	32(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%ecx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	2840853838(%ecx,%eax,1),%ecx
	subl	%edi,%edx
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 76 
	movl	44(%esp),%eax
	orl	%esi,%edx
	addl	%eax,%ebp
	xorl	%ecx,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	2840853838(%ebp,%edx,1),%ebp
	subl	%esi,%eax
	roll	$11,%ebp
	addl	%ebx,%ebp
	# 77 
	movl	24(%esp),%edx
	orl	%ecx,%eax
	addl	%edx,%ebx
	xorl	%ebp,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	2840853838(%ebx,%eax,1),%ebx
	subl	%ecx,%edx
	roll	$8,%ebx
	addl	%edi,%ebx
	# 78 
	movl	60(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%edi
	xorl	%ebx,%edx
	movl	$-1,%eax
	roll	$10,%ebp
	leal	2840853838(%edi,%edx,1),%edi
	subl	%ebp,%eax
	roll	$5,%edi
	addl	%esi,%edi
	# 79 
	movl	52(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%esi
	xorl	%edi,%eax
	movl	128(%esp),%edx
	roll	$10,%ebx
	leal	2840853838(%esi,%eax,1),%esi
	movl	%ecx,64(%esp)
	roll	$6,%esi
	addl	%ecx,%esi
	movl	(%edx),%ecx
	movl	%esi,68(%esp)
	movl	%edi,72(%esp)
	movl	4(%edx),%esi
	movl	%ebx,76(%esp)
	movl	8(%edx),%edi
	movl	%ebp,80(%esp)
	movl	12(%edx),%ebx
	movl	16(%edx),%ebp
	# 80 
	movl	$-1,%edx
	subl	%ebx,%edx
	movl	20(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%ecx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%edi
	leal	1352829926(%ecx,%edx,1),%ecx
	subl	%edi,%eax
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 81 
	movl	56(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ebp
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	1352829926(%ebp,%eax,1),%ebp
	subl	%esi,%edx
	roll	$9,%ebp
	addl	%ebx,%ebp
	# 82 
	movl	28(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebx
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1352829926(%ebx,%edx,1),%ebx
	subl	%ecx,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 83 
	movl	(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%edi
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1352829926(%edi,%eax,1),%edi
	subl	%ebp,%edx
	roll	$11,%edi
	addl	%esi,%edi
	# 84 
	movl	36(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%esi
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1352829926(%esi,%edx,1),%esi
	subl	%ebx,%eax
	roll	$13,%esi
	addl	%ecx,%esi
	# 85 
	movl	8(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%ecx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1352829926(%ecx,%eax,1),%ecx
	subl	%edi,%edx
	roll	$15,%ecx
	addl	%ebp,%ecx
	# 86 
	movl	44(%esp),%eax
	orl	%esi,%edx
	addl	%eax,%ebp
	xorl	%ecx,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	1352829926(%ebp,%edx,1),%ebp
	subl	%esi,%eax
	roll	$15,%ebp
	addl	%ebx,%ebp
	# 87 
	movl	16(%esp),%edx
	orl	%ecx,%eax
	addl	%edx,%ebx
	xorl	%ebp,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	1352829926(%ebx,%eax,1),%ebx
	subl	%ecx,%edx
	roll	$5,%ebx
	addl	%edi,%ebx
	# 88 
	movl	52(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%edi
	xorl	%ebx,%edx
	movl	$-1,%eax
	roll	$10,%ebp
	leal	1352829926(%edi,%edx,1),%edi
	subl	%ebp,%eax
	roll	$7,%edi
	addl	%esi,%edi
	# 89 
	movl	24(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%esi
	xorl	%edi,%eax
	movl	$-1,%edx
	roll	$10,%ebx
	leal	1352829926(%esi,%eax,1),%esi
	subl	%ebx,%edx
	roll	$7,%esi
	addl	%ecx,%esi
	# 90 
	movl	60(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%ecx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%edi
	leal	1352829926(%ecx,%edx,1),%ecx
	subl	%edi,%eax
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 91 
	movl	32(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ebp
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	1352829926(%ebp,%eax,1),%ebp
	subl	%esi,%edx
	roll	$11,%ebp
	addl	%ebx,%ebp
	# 92 
	movl	4(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebx
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1352829926(%ebx,%edx,1),%ebx
	subl	%ecx,%eax
	roll	$14,%ebx
	addl	%edi,%ebx
	# 93 
	movl	40(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%edi
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1352829926(%edi,%eax,1),%edi
	subl	%ebp,%edx
	roll	$14,%edi
	addl	%esi,%edi
	# 94 
	movl	12(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%esi
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1352829926(%esi,%edx,1),%esi
	subl	%ebx,%eax
	roll	$12,%esi
	addl	%ecx,%esi
	# 95 
	movl	48(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%ecx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1352829926(%ecx,%eax,1),%ecx
	movl	%edi,%eax
	roll	$6,%ecx
	addl	%ebp,%ecx
	# 96 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	24(%esp),%eax
	roll	$10,%esi
	leal	1548603684(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$9,%ebp
	addl	%ebx,%ebp
	# 97 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	44(%esp),%eax
	roll	$10,%ecx
	leal	1548603684(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$13,%ebx
	addl	%edi,%ebx
	# 98 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	12(%esp),%eax
	roll	$10,%ebp
	leal	1548603684(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$15,%edi
	addl	%esi,%edi
	# 99 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	28(%esp),%eax
	roll	$10,%ebx
	leal	1548603684(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$7,%esi
	addl	%ecx,%esi
	# 100 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	(%esp),%eax
	roll	$10,%edi
	leal	1548603684(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$12,%ecx
	addl	%ebp,%ecx
	# 101 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	52(%esp),%eax
	roll	$10,%esi
	leal	1548603684(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$8,%ebp
	addl	%ebx,%ebp
	# 102 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	20(%esp),%eax
	roll	$10,%ecx
	leal	1548603684(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 103 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	40(%esp),%eax
	roll	$10,%ebp
	leal	1548603684(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$11,%edi
	addl	%esi,%edi
	# 104 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	56(%esp),%eax
	roll	$10,%ebx
	leal	1548603684(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$7,%esi
	addl	%ecx,%esi
	# 105 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	60(%esp),%eax
	roll	$10,%edi
	leal	1548603684(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$7,%ecx
	addl	%ebp,%ecx
	# 106 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	32(%esp),%eax
	roll	$10,%esi
	leal	1548603684(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	movl	%esi,%eax
	roll	$12,%ebp
	addl	%ebx,%ebp
	# 107 
	subl	%esi,%edx
	andl	%ebp,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	48(%esp),%eax
	roll	$10,%ecx
	leal	1548603684(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	addl	%eax,%ebx
	movl	%ecx,%eax
	roll	$7,%ebx
	addl	%edi,%ebx
	# 108 
	subl	%ecx,%edx
	andl	%ebx,%eax
	andl	%ebp,%edx
	orl	%eax,%edx
	movl	16(%esp),%eax
	roll	$10,%ebp
	leal	1548603684(%edi,%edx,1),%edi
	movl	$-1,%edx
	addl	%eax,%edi
	movl	%ebp,%eax
	roll	$6,%edi
	addl	%esi,%edi
	# 109 
	subl	%ebp,%edx
	andl	%edi,%eax
	andl	%ebx,%edx
	orl	%eax,%edx
	movl	36(%esp),%eax
	roll	$10,%ebx
	leal	1548603684(%esi,%edx,1),%esi
	movl	$-1,%edx
	addl	%eax,%esi
	movl	%ebx,%eax
	roll	$15,%esi
	addl	%ecx,%esi
	# 110 
	subl	%ebx,%edx
	andl	%esi,%eax
	andl	%edi,%edx
	orl	%eax,%edx
	movl	4(%esp),%eax
	roll	$10,%edi
	leal	1548603684(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	addl	%eax,%ecx
	movl	%edi,%eax
	roll	$13,%ecx
	addl	%ebp,%ecx
	# 111 
	subl	%edi,%edx
	andl	%ecx,%eax
	andl	%esi,%edx
	orl	%eax,%edx
	movl	8(%esp),%eax
	roll	$10,%esi
	leal	1548603684(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	addl	%eax,%ebp
	subl	%ecx,%edx
	roll	$11,%ebp
	addl	%ebx,%ebp
	# 112 
	movl	60(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%ebx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1836072691(%ebx,%edx,1),%ebx
	subl	%ebp,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 113 
	movl	20(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%edi
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1836072691(%edi,%eax,1),%edi
	subl	%ebx,%edx
	roll	$7,%edi
	addl	%esi,%edi
	# 114 
	movl	4(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%esi
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1836072691(%esi,%edx,1),%esi
	subl	%edi,%eax
	roll	$15,%esi
	addl	%ecx,%esi
	# 115 
	movl	12(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ecx
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1836072691(%ecx,%eax,1),%ecx
	subl	%esi,%edx
	roll	$11,%ecx
	addl	%ebp,%ecx
	# 116 
	movl	28(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebp
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	1836072691(%ebp,%edx,1),%ebp
	subl	%ecx,%eax
	roll	$8,%ebp
	addl	%ebx,%ebp
	# 117 
	movl	56(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%ebx
	xorl	%esi,%eax
	movl	$-1,%edx
	roll	$10,%ecx
	leal	1836072691(%ebx,%eax,1),%ebx
	subl	%ebp,%edx
	roll	$6,%ebx
	addl	%edi,%ebx
	# 118 
	movl	24(%esp),%eax
	orl	%ebx,%edx
	addl	%eax,%edi
	xorl	%ecx,%edx
	movl	$-1,%eax
	roll	$10,%ebp
	leal	1836072691(%edi,%edx,1),%edi
	subl	%ebx,%eax
	roll	$6,%edi
	addl	%esi,%edi
	# 119 
	movl	36(%esp),%edx
	orl	%edi,%eax
	addl	%edx,%esi
	xorl	%ebp,%eax
	movl	$-1,%edx
	roll	$10,%ebx
	leal	1836072691(%esi,%eax,1),%esi
	subl	%edi,%edx
	roll	$14,%esi
	addl	%ecx,%esi
	# 120 
	movl	44(%esp),%eax
	orl	%esi,%edx
	addl	%eax,%ecx
	xorl	%ebx,%edx
	movl	$-1,%eax
	roll	$10,%edi
	leal	1836072691(%ecx,%edx,1),%ecx
	subl	%esi,%eax
	roll	$12,%ecx
	addl	%ebp,%ecx
	# 121 
	movl	32(%esp),%edx
	orl	%ecx,%eax
	addl	%edx,%ebp
	xorl	%edi,%eax
	movl	$-1,%edx
	roll	$10,%esi
	leal	1836072691(%ebp,%eax,1),%ebp
	subl	%ecx,%edx
	roll	$13,%ebp
	addl	%ebx,%ebp
	# 122 
	movl	48(%esp),%eax
	orl	%ebp,%edx
	addl	%eax,%ebx
	xorl	%esi,%edx
	movl	$-1,%eax
	roll	$10,%ecx
	leal	1836072691(%ebx,%edx,1),%ebx
	subl	%ebp,%eax
	roll	$5,%ebx
	addl	%edi,%ebx
	# 123 
	movl	8(%esp),%edx
	orl	%ebx,%eax
	addl	%edx,%edi
	xorl	%ecx,%eax
	movl	$-1,%edx
	roll	$10,%ebp
	leal	1836072691(%edi,%eax,1),%edi
	subl	%ebx,%edx
	roll	$14,%edi
	addl	%esi,%edi
	# 124 
	movl	40(%esp),%eax
	orl	%edi,%edx
	addl	%eax,%esi
	xorl	%ebp,%edx
	movl	$-1,%eax
	roll	$10,%ebx
	leal	1836072691(%esi,%edx,1),%esi
	subl	%edi,%eax
	roll	$13,%esi
	addl	%ecx,%esi
	# 125 
	movl	(%esp),%edx
	orl	%esi,%eax
	addl	%edx,%ecx
	xorl	%ebx,%eax
	movl	$-1,%edx
	roll	$10,%edi
	leal	1836072691(%ecx,%eax,1),%ecx
	subl	%esi,%edx
	roll	$13,%ecx
	addl	%ebp,%ecx
	# 126 
	movl	16(%esp),%eax
	orl	%ecx,%edx
	addl	%eax,%ebp
	xorl	%edi,%edx
	movl	$-1,%eax
	roll	$10,%esi
	leal	1836072691(%ebp,%edx,1),%ebp
	subl	%ecx,%eax
	roll	$7,%ebp
	addl	%ebx,%ebp
	# 127 
	movl	52(%esp),%edx
	orl	%ebp,%eax
	addl	%edx,%ebx
	xorl	%esi,%eax
	movl	32(%esp),%edx
	roll	$10,%ecx
	leal	1836072691(%ebx,%eax,1),%ebx
	movl	$-1,%eax
	roll	$5,%ebx
	addl	%edi,%ebx
	# 128 
	addl	%edx,%edi
	movl	%ebp,%edx
	subl	%ebx,%eax
	andl	%ebx,%edx
	andl	%ecx,%eax
	orl	%eax,%edx
	movl	24(%esp),%eax
	roll	$10,%ebp
	leal	2053994217(%edi,%edx,1),%edi
	movl	$-1,%edx
	roll	$15,%edi
	addl	%esi,%edi
	# 129 
	addl	%eax,%esi
	movl	%ebx,%eax
	subl	%edi,%edx
	andl	%edi,%eax
	andl	%ebp,%edx
	orl	%edx,%eax
	movl	16(%esp),%edx
	roll	$10,%ebx
	leal	2053994217(%esi,%eax,1),%esi
	movl	$-1,%eax
	roll	$5,%esi
	addl	%ecx,%esi
	# 130 
	addl	%edx,%ecx
	movl	%edi,%edx
	subl	%esi,%eax
	andl	%esi,%edx
	andl	%ebx,%eax
	orl	%eax,%edx
	movl	4(%esp),%eax
	roll	$10,%edi
	leal	2053994217(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	roll	$8,%ecx
	addl	%ebp,%ecx
	# 131 
	addl	%eax,%ebp
	movl	%esi,%eax
	subl	%ecx,%edx
	andl	%ecx,%eax
	andl	%edi,%edx
	orl	%edx,%eax
	movl	12(%esp),%edx
	roll	$10,%esi
	leal	2053994217(%ebp,%eax,1),%ebp
	movl	$-1,%eax
	roll	$11,%ebp
	addl	%ebx,%ebp
	# 132 
	addl	%edx,%ebx
	movl	%ecx,%edx
	subl	%ebp,%eax
	andl	%ebp,%edx
	andl	%esi,%eax
	orl	%eax,%edx
	movl	44(%esp),%eax
	roll	$10,%ecx
	leal	2053994217(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	roll	$14,%ebx
	addl	%edi,%ebx
	# 133 
	addl	%eax,%edi
	movl	%ebp,%eax
	subl	%ebx,%edx
	andl	%ebx,%eax
	andl	%ecx,%edx
	orl	%edx,%eax
	movl	60(%esp),%edx
	roll	$10,%ebp
	leal	2053994217(%edi,%eax,1),%edi
	movl	$-1,%eax
	roll	$14,%edi
	addl	%esi,%edi
	# 134 
	addl	%edx,%esi
	movl	%ebx,%edx
	subl	%edi,%eax
	andl	%edi,%edx
	andl	%ebp,%eax
	orl	%eax,%edx
	movl	(%esp),%eax
	roll	$10,%ebx
	leal	2053994217(%esi,%edx,1),%esi
	movl	$-1,%edx
	roll	$6,%esi
	addl	%ecx,%esi
	# 135 
	addl	%eax,%ecx
	movl	%edi,%eax
	subl	%esi,%edx
	andl	%esi,%eax
	andl	%ebx,%edx
	orl	%edx,%eax
	movl	20(%esp),%edx
	roll	$10,%edi
	leal	2053994217(%ecx,%eax,1),%ecx
	movl	$-1,%eax
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 136 
	addl	%edx,%ebp
	movl	%esi,%edx
	subl	%ecx,%eax
	andl	%ecx,%edx
	andl	%edi,%eax
	orl	%eax,%edx
	movl	48(%esp),%eax
	roll	$10,%esi
	leal	2053994217(%ebp,%edx,1),%ebp
	movl	$-1,%edx
	roll	$6,%ebp
	addl	%ebx,%ebp
	# 137 
	addl	%eax,%ebx
	movl	%ecx,%eax
	subl	%ebp,%edx
	andl	%ebp,%eax
	andl	%esi,%edx
	orl	%edx,%eax
	movl	8(%esp),%edx
	roll	$10,%ecx
	leal	2053994217(%ebx,%eax,1),%ebx
	movl	$-1,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 138 
	addl	%edx,%edi
	movl	%ebp,%edx
	subl	%ebx,%eax
	andl	%ebx,%edx
	andl	%ecx,%eax
	orl	%eax,%edx
	movl	52(%esp),%eax
	roll	$10,%ebp
	leal	2053994217(%edi,%edx,1),%edi
	movl	$-1,%edx
	roll	$12,%edi
	addl	%esi,%edi
	# 139 
	addl	%eax,%esi
	movl	%ebx,%eax
	subl	%edi,%edx
	andl	%edi,%eax
	andl	%ebp,%edx
	orl	%edx,%eax
	movl	36(%esp),%edx
	roll	$10,%ebx
	leal	2053994217(%esi,%eax,1),%esi
	movl	$-1,%eax
	roll	$9,%esi
	addl	%ecx,%esi
	# 140 
	addl	%edx,%ecx
	movl	%edi,%edx
	subl	%esi,%eax
	andl	%esi,%edx
	andl	%ebx,%eax
	orl	%eax,%edx
	movl	28(%esp),%eax
	roll	$10,%edi
	leal	2053994217(%ecx,%edx,1),%ecx
	movl	$-1,%edx
	roll	$12,%ecx
	addl	%ebp,%ecx
	# 141 
	addl	%eax,%ebp
	movl	%esi,%eax
	subl	%ecx,%edx
	andl	%ecx,%eax
	andl	%edi,%edx
	orl	%edx,%eax
	movl	40(%esp),%edx
	roll	$10,%esi
	leal	2053994217(%ebp,%eax,1),%ebp
	movl	$-1,%eax
	roll	$5,%ebp
	addl	%ebx,%ebp
	# 142 
	addl	%edx,%ebx
	movl	%ecx,%edx
	subl	%ebp,%eax
	andl	%ebp,%edx
	andl	%esi,%eax
	orl	%eax,%edx
	movl	56(%esp),%eax
	roll	$10,%ecx
	leal	2053994217(%ebx,%edx,1),%ebx
	movl	$-1,%edx
	roll	$15,%ebx
	addl	%edi,%ebx
	# 143 
	addl	%eax,%edi
	movl	%ebp,%eax
	subl	%ebx,%edx
	andl	%ebx,%eax
	andl	%ecx,%edx
	orl	%eax,%edx
	movl	%ebx,%eax
	roll	$10,%ebp
	leal	2053994217(%edi,%edx,1),%edi
	xorl	%ebp,%eax
	roll	$8,%edi
	addl	%esi,%edi
	# 144 
	movl	48(%esp),%edx
	xorl	%edi,%eax
	addl	%edx,%esi
	roll	$10,%ebx
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$8,%esi
	addl	%ecx,%esi
	# 145 
	xorl	%ebx,%eax
	movl	60(%esp),%edx
	xorl	%esi,%eax
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$10,%edi
	addl	%edx,%ecx
	xorl	%edi,%eax
	roll	$5,%ecx
	addl	%ebp,%ecx
	# 146 
	movl	40(%esp),%edx
	xorl	%ecx,%eax
	addl	%edx,%ebp
	roll	$10,%esi
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$12,%ebp
	addl	%ebx,%ebp
	# 147 
	xorl	%esi,%eax
	movl	16(%esp),%edx
	xorl	%ebp,%eax
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$10,%ecx
	addl	%edx,%ebx
	xorl	%ecx,%eax
	roll	$9,%ebx
	addl	%edi,%ebx
	# 148 
	movl	4(%esp),%edx
	xorl	%ebx,%eax
	addl	%edx,%edi
	roll	$10,%ebp
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$12,%edi
	addl	%esi,%edi
	# 149 
	xorl	%ebp,%eax
	movl	20(%esp),%edx
	xorl	%edi,%eax
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$10,%ebx
	addl	%edx,%esi
	xorl	%ebx,%eax
	roll	$5,%esi
	addl	%ecx,%esi
	# 150 
	movl	32(%esp),%edx
	xorl	%esi,%eax
	addl	%edx,%ecx
	roll	$10,%edi
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$14,%ecx
	addl	%ebp,%ecx
	# 151 
	xorl	%edi,%eax
	movl	28(%esp),%edx
	xorl	%ecx,%eax
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$10,%esi
	addl	%edx,%ebp
	xorl	%esi,%eax
	roll	$6,%ebp
	addl	%ebx,%ebp
	# 152 
	movl	24(%esp),%edx
	xorl	%ebp,%eax
	addl	%edx,%ebx
	roll	$10,%ecx
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$8,%ebx
	addl	%edi,%ebx
	# 153 
	xorl	%ecx,%eax
	movl	8(%esp),%edx
	xorl	%ebx,%eax
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$10,%ebp
	addl	%edx,%edi
	xorl	%ebp,%eax
	roll	$13,%edi
	addl	%esi,%edi
	# 154 
	movl	52(%esp),%edx
	xorl	%edi,%eax
	addl	%edx,%esi
	roll	$10,%ebx
	addl	%eax,%esi
	movl	%edi,%eax
	roll	$6,%esi
	addl	%ecx,%esi
	# 155 
	xorl	%ebx,%eax
	movl	56(%esp),%edx
	xorl	%esi,%eax
	addl	%eax,%ecx
	movl	%esi,%eax
	roll	$10,%edi
	addl	%edx,%ecx
	xorl	%edi,%eax
	roll	$5,%ecx
	addl	%ebp,%ecx
	# 156 
	movl	(%esp),%edx
	xorl	%ecx,%eax
	addl	%edx,%ebp
	roll	$10,%esi
	addl	%eax,%ebp
	movl	%ecx,%eax
	roll	$15,%ebp
	addl	%ebx,%ebp
	# 157 
	xorl	%esi,%eax
	movl	12(%esp),%edx
	xorl	%ebp,%eax
	addl	%eax,%ebx
	movl	%ebp,%eax
	roll	$10,%ecx
	addl	%edx,%ebx
	xorl	%ecx,%eax
	roll	$13,%ebx
	addl	%edi,%ebx
	# 158 
	movl	36(%esp),%edx
	xorl	%ebx,%eax
	addl	%edx,%edi
	roll	$10,%ebp
	addl	%eax,%edi
	movl	%ebx,%eax
	roll	$11,%edi
	addl	%esi,%edi
	# 159 
	xorl	%ebp,%eax
	movl	44(%esp),%edx
	xorl	%edi,%eax
	addl	%eax,%esi
	roll	$10,%ebx
	addl	%edx,%esi
	movl	128(%esp),%edx
	roll	$11,%esi
	addl	%ecx,%esi
	movl	4(%edx),%eax
	addl	%eax,%ebx
	movl	72(%esp),%eax
	addl	%eax,%ebx
	movl	8(%edx),%eax
	addl	%eax,%ebp
	movl	76(%esp),%eax
	addl	%eax,%ebp
	movl	12(%edx),%eax
	addl	%eax,%ecx
	movl	80(%esp),%eax
	addl	%eax,%ecx
	movl	16(%edx),%eax
	addl	%eax,%esi
	movl	64(%esp),%eax
	addl	%eax,%esi
	movl	(%edx),%eax
	addl	%eax,%edi
	movl	68(%esp),%eax
	addl	%eax,%edi
	movl	136(%esp),%eax
	movl	%ebx,(%edx)
	movl	%ebp,4(%edx)
	movl	%ecx,8(%edx)
	subl	$1,%eax
	movl	%esi,12(%edx)
	movl	%edi,16(%edx)
	jle	L001get_out
	movl	%eax,136(%esp)
	movl	%ecx,%edi
	movl	132(%esp),%eax
	movl	%ebx,%ecx
	addl	$64,%eax
	movl	%ebp,%esi
	movl	%eax,132(%esp)
	jmp	L000start
L001get_out:
	addl	$108,%esp
	popl	%ebx
	popl	%ebp
	popl	%edi
	popl	%esi
	ret
