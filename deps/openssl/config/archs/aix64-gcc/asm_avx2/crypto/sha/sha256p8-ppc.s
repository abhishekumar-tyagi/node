.machine	"any"
.csect	.text[PR],7

.globl	.sha256_block_p8
.align	6
.sha256_block_p8:
	stdu	1,-384(1)
	mflr	8
	li	10,207
	li	11,223
	stvx	24,10,1
	addi	10,10,32
	li	12,-1
	stvx	25,11,1
	addi	11,11,32
	stvx	26,10,1
	addi	10,10,32
	stvx	27,11,1
	addi	11,11,32
	stvx	28,10,1
	addi	10,10,32
	stvx	29,11,1
	addi	11,11,32
	stvx	30,10,1
	stvx	31,11,1
	li	11,-4096+255
	stw	12,332(1)
	li	10,0x10
	std	26,336(1)
	li	26,0x20
	std	27,344(1)
	li	27,0x30
	std	28,352(1)
	li	28,0x40
	std	29,360(1)
	li	29,0x50
	std	30,368(1)
	li	30,0x60
	std	31,376(1)
	li	31,0x70
	std	8,400(1)
	or	11,11,11

	bl	LPICmeup
	addi	11,1,79
	.long	0x7C001E19
	.long	0x7C8A1E19
	vsldoi	1,0,0,4
	vsldoi	2,0,0,8
	vsldoi	3,0,0,12
	vsldoi	5,4,4,4
	vsldoi	6,4,4,8
	vsldoi	7,4,4,12
	li	0,3
	b	Loop
.align	5
Loop:
	lvx	28,0,6
	.long	0x7D002699
	addi	4,4,16
	mr	7,6
	stvx	0,0,11
	stvx	1,10,11
	stvx	2,26,11
	stvx	3,27,11
	stvx	4,28,11
	stvx	5,29,11
	stvx	6,30,11
	stvx	7,31,11
	vadduwm	7,7,28
	lvx	28,10,6
	vadduwm	7,7,8
	vsel	29,6,5,4
	vadduwm	6,6,28
	vadduwm	7,7,29
	.long	0x13C4FE82
	vadduwm	7,7,30
	vxor	29,0,1
	vsel	29,1,2,29
	vadduwm	3,3,7
	.long	0x13C08682
	vadduwm	30,30,29
	vadduwm	7,7,30
	lvx	28,26,7
	vsldoi	9,8,8,4
	vadduwm	6,6,9
	vsel	29,5,4,3
	vadduwm	5,5,28
	vadduwm	6,6,29
	.long	0x13C3FE82
	vadduwm	6,6,30
	vxor	29,7,0
	vsel	29,0,1,29
	vadduwm	2,2,6
	.long	0x13C78682
	vadduwm	30,30,29
	vadduwm	6,6,30
	lvx	28,27,7
	vsldoi	10,9,9,4
	vadduwm	5,5,10
	vsel	29,4,3,2
	vadduwm	4,4,28
	vadduwm	5,5,29
	.long	0x13C2FE82
	vadduwm	5,5,30
	vxor	29,6,7
	vsel	29,7,0,29
	vadduwm	1,1,5
	.long	0x13C68682
	vadduwm	30,30,29
	vadduwm	5,5,30
	lvx	28,28,7
	.long	0x7D802699
	addi	4,4,16
	vsldoi	11,10,10,4
	vadduwm	4,4,11
	vsel	29,3,2,1
	vadduwm	3,3,28
	vadduwm	4,4,29
	.long	0x13C1FE82
	vadduwm	4,4,30
	vxor	29,5,6
	vsel	29,6,7,29
	vadduwm	0,0,4
	.long	0x13C58682
	vadduwm	30,30,29
	vadduwm	4,4,30
	lvx	28,29,7
	vadduwm	3,3,12
	vsel	29,2,1,0
	vadduwm	2,2,28
	vadduwm	3,3,29
	.long	0x13C0FE82
	vadduwm	3,3,30
	vxor	29,4,5
	vsel	29,5,6,29
	vadduwm	7,7,3
	.long	0x13C48682
	vadduwm	30,30,29
	vadduwm	3,3,30
	lvx	28,30,7
	vsldoi	13,12,12,4
	vadduwm	2,2,13
	vsel	29,1,0,7
	vadduwm	1,1,28
	vadduwm	2,2,29
	.long	0x13C7FE82
	vadduwm	2,2,30
	vxor	29,3,4
	vsel	29,4,5,29
	vadduwm	6,6,2
	.long	0x13C38682
	vadduwm	30,30,29
	vadduwm	2,2,30
	lvx	28,31,7
	addi	7,7,0x80
	vsldoi	14,13,13,4
	vadduwm	1,1,14
	vsel	29,0,7,6
	vadduwm	0,0,28
	vadduwm	1,1,29
	.long	0x13C6FE82
	vadduwm	1,1,30
	vxor	29,2,3
	vsel	29,3,4,29
	vadduwm	5,5,1
	.long	0x13C28682
	vadduwm	30,30,29
	vadduwm	1,1,30
	lvx	28,0,7
	.long	0x7E002699
	addi	4,4,16
	vsldoi	15,14,14,4
	vadduwm	0,0,15
	vsel	29,7,6,5
	vadduwm	7,7,28
	vadduwm	0,0,29
	.long	0x13C5FE82
	vadduwm	0,0,30
	vxor	29,1,2
	vsel	29,2,3,29
	vadduwm	4,4,0
	.long	0x13C18682
	vadduwm	30,30,29
	vadduwm	0,0,30
	lvx	28,10,7
	vadduwm	7,7,16
	vsel	29,6,5,4
	vadduwm	6,6,28
	vadduwm	7,7,29
	.long	0x13C4FE82
	vadduwm	7,7,30
	vxor	29,0,1
	vsel	29,1,2,29
	vadduwm	3,3,7
	.long	0x13C08682
	vadduwm	30,30,29
	vadduwm	7,7,30
	lvx	28,26,7
	vsldoi	17,16,16,4
	vadduwm	6,6,17
	vsel	29,5,4,3
	vadduwm	5,5,28
	vadduwm	6,6,29
	.long	0x13C3FE82
	vadduwm	6,6,30
	vxor	29,7,0
	vsel	29,0,1,29
	vadduwm	2,2,6
	.long	0x13C78682
	vadduwm	30,30,29
	vadduwm	6,6,30
	lvx	28,27,7
	vsldoi	18,17,17,4
	vadduwm	5,5,18
	vsel	29,4,3,2
	vadduwm	4,4,28
	vadduwm	5,5,29
	.long	0x13C2FE82
	vadduwm	5,5,30
	vxor	29,6,7
	vsel	29,7,0,29
	vadduwm	1,1,5
	.long	0x13C68682
	vadduwm	30,30,29
	vadduwm	5,5,30
	lvx	28,28,7
	.long	0x7F002699
	addi	4,4,16
	vsldoi	19,18,18,4
	vadduwm	4,4,19
	vsel	29,3,2,1
	vadduwm	3,3,28
	vadduwm	4,4,29
	.long	0x13C1FE82
	vadduwm	4,4,30
	vxor	29,5,6
	vsel	29,6,7,29
	vadduwm	0,0,4
	.long	0x13C58682
	vadduwm	30,30,29
	vadduwm	4,4,30
	lvx	28,29,7
	vadduwm	3,3,24
	vsel	29,2,1,0
	vadduwm	2,2,28
	vadduwm	3,3,29
	.long	0x13C0FE82
	vadduwm	3,3,30
	vxor	29,4,5
	vsel	29,5,6,29
	vadduwm	7,7,3
	.long	0x13C48682
	vadduwm	30,30,29
	vadduwm	3,3,30
	lvx	28,30,7
	vsldoi	25,24,24,4
	vadduwm	2,2,25
	vsel	29,1,0,7
	vadduwm	1,1,28
	vadduwm	2,2,29
	.long	0x13C7FE82
	vadduwm	2,2,30
	vxor	29,3,4
	vsel	29,4,5,29
	vadduwm	6,6,2
	.long	0x13C38682
	vadduwm	30,30,29
	vadduwm	2,2,30
	lvx	28,31,7
	addi	7,7,0x80
	vsldoi	26,25,25,4
	vadduwm	1,1,26
	vsel	29,0,7,6
	vadduwm	0,0,28
	vadduwm	1,1,29
	.long	0x13C6FE82
	vadduwm	1,1,30
	vxor	29,2,3
	vsel	29,3,4,29
	vadduwm	5,5,1
	.long	0x13C28682
	vadduwm	30,30,29
	vadduwm	1,1,30
	lvx	28,0,7
	vsldoi	27,26,26,4
	.long	0x13C90682
	vadduwm	8,8,30
	.long	0x13DA7E82
	vadduwm	8,8,30
	vadduwm	8,8,17
	vadduwm	0,0,27
	vsel	29,7,6,5
	vadduwm	7,7,28
	vadduwm	0,0,29
	.long	0x13C5FE82
	vadduwm	0,0,30
	vxor	29,1,2
	vsel	29,2,3,29
	vadduwm	4,4,0
	.long	0x13C18682
	vadduwm	30,30,29
	vadduwm	0,0,30
	lvx	28,10,7
	mtctr	0
	b	L16_xx
.align	5
L16_xx:
	.long	0x13CA0682
	vadduwm	9,9,30
	.long	0x13DB7E82
	vadduwm	9,9,30
	vadduwm	9,9,18
	vadduwm	7,7,8
	vsel	29,6,5,4
	vadduwm	6,6,28
	vadduwm	7,7,29
	.long	0x13C4FE82
	vadduwm	7,7,30
	vxor	29,0,1
	vsel	29,1,2,29
	vadduwm	3,3,7
	.long	0x13C08682
	vadduwm	30,30,29
	vadduwm	7,7,30
	lvx	28,26,7
	.long	0x13CB0682
	vadduwm	10,10,30
	.long	0x13C87E82
	vadduwm	10,10,30
	vadduwm	10,10,19
	vadduwm	6,6,9
	vsel	29,5,4,3
	vadduwm	5,5,28
	vadduwm	6,6,29
	.long	0x13C3FE82
	vadduwm	6,6,30
	vxor	29,7,0
	vsel	29,0,1,29
	vadduwm	2,2,6
	.long	0x13C78682
	vadduwm	30,30,29
	vadduwm	6,6,30
	lvx	28,27,7
	.long	0x13CC0682
	vadduwm	11,11,30
	.long	0x13C97E82
	vadduwm	11,11,30
	vadduwm	11,11,24
	vadduwm	5,5,10
	vsel	29,4,3,2
	vadduwm	4,4,28
	vadduwm	5,5,29
	.long	0x13C2FE82
	vadduwm	5,5,30
	vxor	29,6,7
	vsel	29,7,0,29
	vadduwm	1,1,5
	.long	0x13C68682
	vadduwm	30,30,29
	vadduwm	5,5,30
	lvx	28,28,7
	.long	0x13CD0682
	vadduwm	12,12,30
	.long	0x13CA7E82
	vadduwm	12,12,30
	vadduwm	12,12,25
	vadduwm	4,4,11
	vsel	29,3,2,1
	vadduwm	3,3,28
	vadduwm	4,4,29
	.long	0x13C1FE82
	vadduwm	4,4,30
	vxor	29,5,6
	vsel	29,6,7,29
	vadduwm	0,0,4
	.long	0x13C58682
	vadduwm	30,30,29
	vadduwm	4,4,30
	lvx	28,29,7
	.long	0x13CE0682
	vadduwm	13,13,30
	.long	0x13CB7E82
	vadduwm	13,13,30
	vadduwm	13,13,26
	vadduwm	3,3,12
	vsel	29,2,1,0
	vadduwm	2,2,28
	vadduwm	3,3,29
	.long	0x13C0FE82
	vadduwm	3,3,30
	vxor	29,4,5
	vsel	29,5,6,29
	vadduwm	7,7,3
	.long	0x13C48682
	vadduwm	30,30,29
	vadduwm	3,3,30
	lvx	28,30,7
	.long	0x13CF0682
	vadduwm	14,14,30
	.long	0x13CC7E82
	vadduwm	14,14,30
	vadduwm	14,14,27
	vadduwm	2,2,13
	vsel	29,1,0,7
	vadduwm	1,1,28
	vadduwm	2,2,29
	.long	0x13C7FE82
	vadduwm	2,2,30
	vxor	29,3,4
	vsel	29,4,5,29
	vadduwm	6,6,2
	.long	0x13C38682
	vadduwm	30,30,29
	vadduwm	2,2,30
	lvx	28,31,7
	addi	7,7,0x80
	.long	0x13D00682
	vadduwm	15,15,30
	.long	0x13CD7E82
	vadduwm	15,15,30
	vadduwm	15,15,8
	vadduwm	1,1,14
	vsel	29,0,7,6
	vadduwm	0,0,28
	vadduwm	1,1,29
	.long	0x13C6FE82
	vadduwm	1,1,30
	vxor	29,2,3
	vsel	29,3,4,29
	vadduwm	5,5,1
	.long	0x13C28682
	vadduwm	30,30,29
	vadduwm	1,1,30
	lvx	28,0,7
	.long	0x13D10682
	vadduwm	16,16,30
	.long	0x13CE7E82
	vadduwm	16,16,30
	vadduwm	16,16,9
	vadduwm	0,0,15
	vsel	29,7,6,5
	vadduwm	7,7,28
	vadduwm	0,0,29
	.long	0x13C5FE82
	vadduwm	0,0,30
	vxor	29,1,2
	vsel	29,2,3,29
	vadduwm	4,4,0
	.long	0x13C18682
	vadduwm	30,30,29
	vadduwm	0,0,30
	lvx	28,10,7
	.long	0x13D20682
	vadduwm	17,17,30
	.long	0x13CF7E82
	vadduwm	17,17,30
	vadduwm	17,17,10
	vadduwm	7,7,16
	vsel	29,6,5,4
	vadduwm	6,6,28
	vadduwm	7,7,29
	.long	0x13C4FE82
	vadduwm	7,7,30
	vxor	29,0,1
	vsel	29,1,2,29
	vadduwm	3,3,7
	.long	0x13C08682
	vadduwm	30,30,29
	vadduwm	7,7,30
	lvx	28,26,7
	.long	0x13D30682
	vadduwm	18,18,30
	.long	0x13D07E82
	vadduwm	18,18,30
	vadduwm	18,18,11
	vadduwm	6,6,17
	vsel	29,5,4,3
	vadduwm	5,5,28
	vadduwm	6,6,29
	.long	0x13C3FE82
	vadduwm	6,6,30
	vxor	29,7,0
	vsel	29,0,1,29
	vadduwm	2,2,6
	.long	0x13C78682
	vadduwm	30,30,29
	vadduwm	6,6,30
	lvx	28,27,7
	.long	0x13D80682
	vadduwm	19,19,30
	.long	0x13D17E82
	vadduwm	19,19,30
	vadduwm	19,19,12
	vadduwm	5,5,18
	vsel	29,4,3,2
	vadduwm	4,4,28
	vadduwm	5,5,29
	.long	0x13C2FE82
	vadduwm	5,5,30
	vxor	29,6,7
	vsel	29,7,0,29
	vadduwm	1,1,5
	.long	0x13C68682
	vadduwm	30,30,29
	vadduwm	5,5,30
	lvx	28,28,7
	.long	0x13D90682
	vadduwm	24,24,30
	.long	0x13D27E82
	vadduwm	24,24,30
	vadduwm	24,24,13
	vadduwm	4,4,19
	vsel	29,3,2,1
	vadduwm	3,3,28
	vadduwm	4,4,29
	.long	0x13C1FE82
	vadduwm	4,4,30
	vxor	29,5,6
	vsel	29,6,7,29
	vadduwm	0,0,4
	.long	0x13C58682
	vadduwm	30,30,29
	vadduwm	4,4,30
	lvx	28,29,7
	.long	0x13DA0682
	vadduwm	25,25,30
	.long	0x13D37E82
	vadduwm	25,25,30
	vadduwm	25,25,14
	vadduwm	3,3,24
	vsel	29,2,1,0
	vadduwm	2,2,28
	vadduwm	3,3,29
	.long	0x13C0FE82
	vadduwm	3,3,30
	vxor	29,4,5
	vsel	29,5,6,29
	vadduwm	7,7,3
	.long	0x13C48682
	vadduwm	30,30,29
	vadduwm	3,3,30
	lvx	28,30,7
	.long	0x13DB0682
	vadduwm	26,26,30
	.long	0x13D87E82
	vadduwm	26,26,30
	vadduwm	26,26,15
	vadduwm	2,2,25
	vsel	29,1,0,7
	vadduwm	1,1,28
	vadduwm	2,2,29
	.long	0x13C7FE82
	vadduwm	2,2,30
	vxor	29,3,4
	vsel	29,4,5,29
	vadduwm	6,6,2
	.long	0x13C38682
	vadduwm	30,30,29
	vadduwm	2,2,30
	lvx	28,31,7
	addi	7,7,0x80
	.long	0x13C80682
	vadduwm	27,27,30
	.long	0x13D97E82
	vadduwm	27,27,30
	vadduwm	27,27,16
	vadduwm	1,1,26
	vsel	29,0,7,6
	vadduwm	0,0,28
	vadduwm	1,1,29
	.long	0x13C6FE82
	vadduwm	1,1,30
	vxor	29,2,3
	vsel	29,3,4,29
	vadduwm	5,5,1
	.long	0x13C28682
	vadduwm	30,30,29
	vadduwm	1,1,30
	lvx	28,0,7
	.long	0x13C90682
	vadduwm	8,8,30
	.long	0x13DA7E82
	vadduwm	8,8,30
	vadduwm	8,8,17
	vadduwm	0,0,27
	vsel	29,7,6,5
	vadduwm	7,7,28
	vadduwm	0,0,29
	.long	0x13C5FE82
	vadduwm	0,0,30
	vxor	29,1,2
	vsel	29,2,3,29
	vadduwm	4,4,0
	.long	0x13C18682
	vadduwm	30,30,29
	vadduwm	0,0,30
	lvx	28,10,7
	bc	16,0,L16_xx

	lvx	10,0,11
	subic.	5,5,1
	lvx	11,10,11
	vadduwm	0,0,10
	lvx	12,26,11
	vadduwm	1,1,11
	lvx	13,27,11
	vadduwm	2,2,12
	lvx	14,28,11
	vadduwm	3,3,13
	lvx	15,29,11
	vadduwm	4,4,14
	lvx	16,30,11
	vadduwm	5,5,15
	lvx	17,31,11
	vadduwm	6,6,16
	vadduwm	7,7,17
	bne	Loop
	lvx	8,26,7
	vperm	0,0,1,28
	lvx	9,27,7
	vperm	4,4,5,28
	vperm	0,0,2,8
	vperm	4,4,6,8
	vperm	0,0,3,9
	vperm	4,4,7,9
	.long	0x7C001F19
	.long	0x7C8A1F19
	addi	11,1,207
	mtlr	8
	or	12,12,12
	lvx	24,0,11
	lvx	25,10,11
	lvx	26,26,11
	lvx	27,27,11
	lvx	28,28,11
	lvx	29,29,11
	lvx	30,30,11
	lvx	31,31,11
	ld	26,336(1)
	ld	27,344(1)
	ld	28,352(1)
	ld	29,360(1)
	ld	30,368(1)
	ld	31,376(1)
	addi	1,1,384
	blr	
.long	0
.byte	0,12,4,1,0x80,6,3,0
.long	0

.align	6
LPICmeup:
	mflr	0
	bcl	20,31,$+4
	mflr	6
	addi	6,6,56
	mtlr	0
	blr	
.long	0
.byte	0,12,0x14,0,0,0,0,0
.space	28
.long	0x428a2f98,0x428a2f98,0x428a2f98,0x428a2f98
.long	0x71374491,0x71374491,0x71374491,0x71374491
.long	0xb5c0fbcf,0xb5c0fbcf,0xb5c0fbcf,0xb5c0fbcf
.long	0xe9b5dba5,0xe9b5dba5,0xe9b5dba5,0xe9b5dba5
.long	0x3956c25b,0x3956c25b,0x3956c25b,0x3956c25b
.long	0x59f111f1,0x59f111f1,0x59f111f1,0x59f111f1
.long	0x923f82a4,0x923f82a4,0x923f82a4,0x923f82a4
.long	0xab1c5ed5,0xab1c5ed5,0xab1c5ed5,0xab1c5ed5
.long	0xd807aa98,0xd807aa98,0xd807aa98,0xd807aa98
.long	0x12835b01,0x12835b01,0x12835b01,0x12835b01
.long	0x243185be,0x243185be,0x243185be,0x243185be
.long	0x550c7dc3,0x550c7dc3,0x550c7dc3,0x550c7dc3
.long	0x72be5d74,0x72be5d74,0x72be5d74,0x72be5d74
.long	0x80deb1fe,0x80deb1fe,0x80deb1fe,0x80deb1fe
.long	0x9bdc06a7,0x9bdc06a7,0x9bdc06a7,0x9bdc06a7
.long	0xc19bf174,0xc19bf174,0xc19bf174,0xc19bf174
.long	0xe49b69c1,0xe49b69c1,0xe49b69c1,0xe49b69c1
.long	0xefbe4786,0xefbe4786,0xefbe4786,0xefbe4786
.long	0x0fc19dc6,0x0fc19dc6,0x0fc19dc6,0x0fc19dc6
.long	0x240ca1cc,0x240ca1cc,0x240ca1cc,0x240ca1cc
.long	0x2de92c6f,0x2de92c6f,0x2de92c6f,0x2de92c6f
.long	0x4a7484aa,0x4a7484aa,0x4a7484aa,0x4a7484aa
.long	0x5cb0a9dc,0x5cb0a9dc,0x5cb0a9dc,0x5cb0a9dc
.long	0x76f988da,0x76f988da,0x76f988da,0x76f988da
.long	0x983e5152,0x983e5152,0x983e5152,0x983e5152
.long	0xa831c66d,0xa831c66d,0xa831c66d,0xa831c66d
.long	0xb00327c8,0xb00327c8,0xb00327c8,0xb00327c8
.long	0xbf597fc7,0xbf597fc7,0xbf597fc7,0xbf597fc7
.long	0xc6e00bf3,0xc6e00bf3,0xc6e00bf3,0xc6e00bf3
.long	0xd5a79147,0xd5a79147,0xd5a79147,0xd5a79147
.long	0x06ca6351,0x06ca6351,0x06ca6351,0x06ca6351
.long	0x14292967,0x14292967,0x14292967,0x14292967
.long	0x27b70a85,0x27b70a85,0x27b70a85,0x27b70a85
.long	0x2e1b2138,0x2e1b2138,0x2e1b2138,0x2e1b2138
.long	0x4d2c6dfc,0x4d2c6dfc,0x4d2c6dfc,0x4d2c6dfc
.long	0x53380d13,0x53380d13,0x53380d13,0x53380d13
.long	0x650a7354,0x650a7354,0x650a7354,0x650a7354
.long	0x766a0abb,0x766a0abb,0x766a0abb,0x766a0abb
.long	0x81c2c92e,0x81c2c92e,0x81c2c92e,0x81c2c92e
.long	0x92722c85,0x92722c85,0x92722c85,0x92722c85
.long	0xa2bfe8a1,0xa2bfe8a1,0xa2bfe8a1,0xa2bfe8a1
.long	0xa81a664b,0xa81a664b,0xa81a664b,0xa81a664b
.long	0xc24b8b70,0xc24b8b70,0xc24b8b70,0xc24b8b70
.long	0xc76c51a3,0xc76c51a3,0xc76c51a3,0xc76c51a3
.long	0xd192e819,0xd192e819,0xd192e819,0xd192e819
.long	0xd6990624,0xd6990624,0xd6990624,0xd6990624
.long	0xf40e3585,0xf40e3585,0xf40e3585,0xf40e3585
.long	0x106aa070,0x106aa070,0x106aa070,0x106aa070
.long	0x19a4c116,0x19a4c116,0x19a4c116,0x19a4c116
.long	0x1e376c08,0x1e376c08,0x1e376c08,0x1e376c08
.long	0x2748774c,0x2748774c,0x2748774c,0x2748774c
.long	0x34b0bcb5,0x34b0bcb5,0x34b0bcb5,0x34b0bcb5
.long	0x391c0cb3,0x391c0cb3,0x391c0cb3,0x391c0cb3
.long	0x4ed8aa4a,0x4ed8aa4a,0x4ed8aa4a,0x4ed8aa4a
.long	0x5b9cca4f,0x5b9cca4f,0x5b9cca4f,0x5b9cca4f
.long	0x682e6ff3,0x682e6ff3,0x682e6ff3,0x682e6ff3
.long	0x748f82ee,0x748f82ee,0x748f82ee,0x748f82ee
.long	0x78a5636f,0x78a5636f,0x78a5636f,0x78a5636f
.long	0x84c87814,0x84c87814,0x84c87814,0x84c87814
.long	0x8cc70208,0x8cc70208,0x8cc70208,0x8cc70208
.long	0x90befffa,0x90befffa,0x90befffa,0x90befffa
.long	0xa4506ceb,0xa4506ceb,0xa4506ceb,0xa4506ceb
.long	0xbef9a3f7,0xbef9a3f7,0xbef9a3f7,0xbef9a3f7
.long	0xc67178f2,0xc67178f2,0xc67178f2,0xc67178f2
.long	0,0,0,0
.long	0x00010203,0x10111213,0x10111213,0x10111213
.long	0x00010203,0x04050607,0x10111213,0x10111213
.long	0x00010203,0x04050607,0x08090a0b,0x10111213
.byte	83,72,65,50,53,54,32,102,111,114,32,80,111,119,101,114,73,83,65,32,50,46,48,55,44,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97,112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103,62,0
.align	2
.align	2
