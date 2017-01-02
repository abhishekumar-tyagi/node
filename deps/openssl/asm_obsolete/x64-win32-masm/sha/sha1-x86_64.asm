OPTION	DOTNAME
.text$	SEGMENT ALIGN(256) 'CODE'
EXTERN	OPENSSL_ia32cap_P:NEAR

PUBLIC	sha1_block_data_order

ALIGN	16
sha1_block_data_order	PROC PUBLIC
	mov	QWORD PTR[8+rsp],rdi	;WIN64 prologue
	mov	QWORD PTR[16+rsp],rsi
	mov	rax,rsp
$L$SEH_begin_sha1_block_data_order::
	mov	rdi,rcx
	mov	rsi,rdx
	mov	rdx,r8


	mov	r9d,DWORD PTR[((OPENSSL_ia32cap_P+0))]
	mov	r8d,DWORD PTR[((OPENSSL_ia32cap_P+4))]
	mov	r10d,DWORD PTR[((OPENSSL_ia32cap_P+8))]
	test	r8d,512
	jz	$L$ialu
	test	r10d,536870912
	jnz	_shaext_shortcut
	jmp	_ssse3_shortcut

ALIGN	16
$L$ialu::
	mov	rax,rsp
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	mov	r8,rdi
	sub	rsp,72
	mov	r9,rsi
	and	rsp,-64
	mov	r10,rdx
	mov	QWORD PTR[64+rsp],rax
$L$prologue::

	mov	esi,DWORD PTR[r8]
	mov	edi,DWORD PTR[4+r8]
	mov	r11d,DWORD PTR[8+r8]
	mov	r12d,DWORD PTR[12+r8]
	mov	r13d,DWORD PTR[16+r8]
	jmp	$L$loop

ALIGN	16
$L$loop::
	mov	edx,DWORD PTR[r9]
	bswap	edx
	mov	ebp,DWORD PTR[4+r9]
	mov	eax,r12d
	mov	DWORD PTR[rsp],edx
	mov	ecx,esi
	bswap	ebp
	xor	eax,r11d
	rol	ecx,5
	and	eax,edi
	lea	r13d,DWORD PTR[1518500249+r13*1+rdx]
	add	r13d,ecx
	xor	eax,r12d
	rol	edi,30
	add	r13d,eax
	mov	r14d,DWORD PTR[8+r9]
	mov	eax,r11d
	mov	DWORD PTR[4+rsp],ebp
	mov	ecx,r13d
	bswap	r14d
	xor	eax,edi
	rol	ecx,5
	and	eax,esi
	lea	r12d,DWORD PTR[1518500249+r12*1+rbp]
	add	r12d,ecx
	xor	eax,r11d
	rol	esi,30
	add	r12d,eax
	mov	edx,DWORD PTR[12+r9]
	mov	eax,edi
	mov	DWORD PTR[8+rsp],r14d
	mov	ecx,r12d
	bswap	edx
	xor	eax,esi
	rol	ecx,5
	and	eax,r13d
	lea	r11d,DWORD PTR[1518500249+r11*1+r14]
	add	r11d,ecx
	xor	eax,edi
	rol	r13d,30
	add	r11d,eax
	mov	ebp,DWORD PTR[16+r9]
	mov	eax,esi
	mov	DWORD PTR[12+rsp],edx
	mov	ecx,r11d
	bswap	ebp
	xor	eax,r13d
	rol	ecx,5
	and	eax,r12d
	lea	edi,DWORD PTR[1518500249+rdi*1+rdx]
	add	edi,ecx
	xor	eax,esi
	rol	r12d,30
	add	edi,eax
	mov	r14d,DWORD PTR[20+r9]
	mov	eax,r13d
	mov	DWORD PTR[16+rsp],ebp
	mov	ecx,edi
	bswap	r14d
	xor	eax,r12d
	rol	ecx,5
	and	eax,r11d
	lea	esi,DWORD PTR[1518500249+rsi*1+rbp]
	add	esi,ecx
	xor	eax,r13d
	rol	r11d,30
	add	esi,eax
	mov	edx,DWORD PTR[24+r9]
	mov	eax,r12d
	mov	DWORD PTR[20+rsp],r14d
	mov	ecx,esi
	bswap	edx
	xor	eax,r11d
	rol	ecx,5
	and	eax,edi
	lea	r13d,DWORD PTR[1518500249+r13*1+r14]
	add	r13d,ecx
	xor	eax,r12d
	rol	edi,30
	add	r13d,eax
	mov	ebp,DWORD PTR[28+r9]
	mov	eax,r11d
	mov	DWORD PTR[24+rsp],edx
	mov	ecx,r13d
	bswap	ebp
	xor	eax,edi
	rol	ecx,5
	and	eax,esi
	lea	r12d,DWORD PTR[1518500249+r12*1+rdx]
	add	r12d,ecx
	xor	eax,r11d
	rol	esi,30
	add	r12d,eax
	mov	r14d,DWORD PTR[32+r9]
	mov	eax,edi
	mov	DWORD PTR[28+rsp],ebp
	mov	ecx,r12d
	bswap	r14d
	xor	eax,esi
	rol	ecx,5
	and	eax,r13d
	lea	r11d,DWORD PTR[1518500249+r11*1+rbp]
	add	r11d,ecx
	xor	eax,edi
	rol	r13d,30
	add	r11d,eax
	mov	edx,DWORD PTR[36+r9]
	mov	eax,esi
	mov	DWORD PTR[32+rsp],r14d
	mov	ecx,r11d
	bswap	edx
	xor	eax,r13d
	rol	ecx,5
	and	eax,r12d
	lea	edi,DWORD PTR[1518500249+rdi*1+r14]
	add	edi,ecx
	xor	eax,esi
	rol	r12d,30
	add	edi,eax
	mov	ebp,DWORD PTR[40+r9]
	mov	eax,r13d
	mov	DWORD PTR[36+rsp],edx
	mov	ecx,edi
	bswap	ebp
	xor	eax,r12d
	rol	ecx,5
	and	eax,r11d
	lea	esi,DWORD PTR[1518500249+rsi*1+rdx]
	add	esi,ecx
	xor	eax,r13d
	rol	r11d,30
	add	esi,eax
	mov	r14d,DWORD PTR[44+r9]
	mov	eax,r12d
	mov	DWORD PTR[40+rsp],ebp
	mov	ecx,esi
	bswap	r14d
	xor	eax,r11d
	rol	ecx,5
	and	eax,edi
	lea	r13d,DWORD PTR[1518500249+r13*1+rbp]
	add	r13d,ecx
	xor	eax,r12d
	rol	edi,30
	add	r13d,eax
	mov	edx,DWORD PTR[48+r9]
	mov	eax,r11d
	mov	DWORD PTR[44+rsp],r14d
	mov	ecx,r13d
	bswap	edx
	xor	eax,edi
	rol	ecx,5
	and	eax,esi
	lea	r12d,DWORD PTR[1518500249+r12*1+r14]
	add	r12d,ecx
	xor	eax,r11d
	rol	esi,30
	add	r12d,eax
	mov	ebp,DWORD PTR[52+r9]
	mov	eax,edi
	mov	DWORD PTR[48+rsp],edx
	mov	ecx,r12d
	bswap	ebp
	xor	eax,esi
	rol	ecx,5
	and	eax,r13d
	lea	r11d,DWORD PTR[1518500249+r11*1+rdx]
	add	r11d,ecx
	xor	eax,edi
	rol	r13d,30
	add	r11d,eax
	mov	r14d,DWORD PTR[56+r9]
	mov	eax,esi
	mov	DWORD PTR[52+rsp],ebp
	mov	ecx,r11d
	bswap	r14d
	xor	eax,r13d
	rol	ecx,5
	and	eax,r12d
	lea	edi,DWORD PTR[1518500249+rdi*1+rbp]
	add	edi,ecx
	xor	eax,esi
	rol	r12d,30
	add	edi,eax
	mov	edx,DWORD PTR[60+r9]
	mov	eax,r13d
	mov	DWORD PTR[56+rsp],r14d
	mov	ecx,edi
	bswap	edx
	xor	eax,r12d
	rol	ecx,5
	and	eax,r11d
	lea	esi,DWORD PTR[1518500249+rsi*1+r14]
	add	esi,ecx
	xor	eax,r13d
	rol	r11d,30
	add	esi,eax
	xor	ebp,DWORD PTR[rsp]
	mov	eax,r12d
	mov	DWORD PTR[60+rsp],edx
	mov	ecx,esi
	xor	ebp,DWORD PTR[8+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	ebp,DWORD PTR[32+rsp]
	and	eax,edi
	lea	r13d,DWORD PTR[1518500249+r13*1+rdx]
	rol	edi,30
	xor	eax,r12d
	add	r13d,ecx
	rol	ebp,1
	add	r13d,eax
	xor	r14d,DWORD PTR[4+rsp]
	mov	eax,r11d
	mov	DWORD PTR[rsp],ebp
	mov	ecx,r13d
	xor	r14d,DWORD PTR[12+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	r14d,DWORD PTR[36+rsp]
	and	eax,esi
	lea	r12d,DWORD PTR[1518500249+r12*1+rbp]
	rol	esi,30
	xor	eax,r11d
	add	r12d,ecx
	rol	r14d,1
	add	r12d,eax
	xor	edx,DWORD PTR[8+rsp]
	mov	eax,edi
	mov	DWORD PTR[4+rsp],r14d
	mov	ecx,r12d
	xor	edx,DWORD PTR[16+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	edx,DWORD PTR[40+rsp]
	and	eax,r13d
	lea	r11d,DWORD PTR[1518500249+r11*1+r14]
	rol	r13d,30
	xor	eax,edi
	add	r11d,ecx
	rol	edx,1
	add	r11d,eax
	xor	ebp,DWORD PTR[12+rsp]
	mov	eax,esi
	mov	DWORD PTR[8+rsp],edx
	mov	ecx,r11d
	xor	ebp,DWORD PTR[20+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	ebp,DWORD PTR[44+rsp]
	and	eax,r12d
	lea	edi,DWORD PTR[1518500249+rdi*1+rdx]
	rol	r12d,30
	xor	eax,esi
	add	edi,ecx
	rol	ebp,1
	add	edi,eax
	xor	r14d,DWORD PTR[16+rsp]
	mov	eax,r13d
	mov	DWORD PTR[12+rsp],ebp
	mov	ecx,edi
	xor	r14d,DWORD PTR[24+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	r14d,DWORD PTR[48+rsp]
	and	eax,r11d
	lea	esi,DWORD PTR[1518500249+rsi*1+rbp]
	rol	r11d,30
	xor	eax,r13d
	add	esi,ecx
	rol	r14d,1
	add	esi,eax
	xor	edx,DWORD PTR[20+rsp]
	mov	eax,edi
	mov	DWORD PTR[16+rsp],r14d
	mov	ecx,esi
	xor	edx,DWORD PTR[28+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	edx,DWORD PTR[52+rsp]
	lea	r13d,DWORD PTR[1859775393+r13*1+r14]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[24+rsp]
	mov	eax,esi
	mov	DWORD PTR[20+rsp],edx
	mov	ecx,r13d
	xor	ebp,DWORD PTR[32+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	ebp,DWORD PTR[56+rsp]
	lea	r12d,DWORD PTR[1859775393+r12*1+rdx]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[28+rsp]
	mov	eax,r13d
	mov	DWORD PTR[24+rsp],ebp
	mov	ecx,r12d
	xor	r14d,DWORD PTR[36+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	r14d,DWORD PTR[60+rsp]
	lea	r11d,DWORD PTR[1859775393+r11*1+rbp]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[32+rsp]
	mov	eax,r12d
	mov	DWORD PTR[28+rsp],r14d
	mov	ecx,r11d
	xor	edx,DWORD PTR[40+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	edx,DWORD PTR[rsp]
	lea	edi,DWORD PTR[1859775393+rdi*1+r14]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	edx,1
	xor	ebp,DWORD PTR[36+rsp]
	mov	eax,r11d
	mov	DWORD PTR[32+rsp],edx
	mov	ecx,edi
	xor	ebp,DWORD PTR[44+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	ebp,DWORD PTR[4+rsp]
	lea	esi,DWORD PTR[1859775393+rsi*1+rdx]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[40+rsp]
	mov	eax,edi
	mov	DWORD PTR[36+rsp],ebp
	mov	ecx,esi
	xor	r14d,DWORD PTR[48+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	r14d,DWORD PTR[8+rsp]
	lea	r13d,DWORD PTR[1859775393+r13*1+rbp]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[44+rsp]
	mov	eax,esi
	mov	DWORD PTR[40+rsp],r14d
	mov	ecx,r13d
	xor	edx,DWORD PTR[52+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	edx,DWORD PTR[12+rsp]
	lea	r12d,DWORD PTR[1859775393+r12*1+r14]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[48+rsp]
	mov	eax,r13d
	mov	DWORD PTR[44+rsp],edx
	mov	ecx,r12d
	xor	ebp,DWORD PTR[56+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	ebp,DWORD PTR[16+rsp]
	lea	r11d,DWORD PTR[1859775393+r11*1+rdx]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[52+rsp]
	mov	eax,r12d
	mov	DWORD PTR[48+rsp],ebp
	mov	ecx,r11d
	xor	r14d,DWORD PTR[60+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	r14d,DWORD PTR[20+rsp]
	lea	edi,DWORD PTR[1859775393+rdi*1+rbp]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	r14d,1
	xor	edx,DWORD PTR[56+rsp]
	mov	eax,r11d
	mov	DWORD PTR[52+rsp],r14d
	mov	ecx,edi
	xor	edx,DWORD PTR[rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	edx,DWORD PTR[24+rsp]
	lea	esi,DWORD PTR[1859775393+rsi*1+r14]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	edx,1
	xor	ebp,DWORD PTR[60+rsp]
	mov	eax,edi
	mov	DWORD PTR[56+rsp],edx
	mov	ecx,esi
	xor	ebp,DWORD PTR[4+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	ebp,DWORD PTR[28+rsp]
	lea	r13d,DWORD PTR[1859775393+r13*1+rdx]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[rsp]
	mov	eax,esi
	mov	DWORD PTR[60+rsp],ebp
	mov	ecx,r13d
	xor	r14d,DWORD PTR[8+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	r14d,DWORD PTR[32+rsp]
	lea	r12d,DWORD PTR[1859775393+r12*1+rbp]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[4+rsp]
	mov	eax,r13d
	mov	DWORD PTR[rsp],r14d
	mov	ecx,r12d
	xor	edx,DWORD PTR[12+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	edx,DWORD PTR[36+rsp]
	lea	r11d,DWORD PTR[1859775393+r11*1+r14]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[8+rsp]
	mov	eax,r12d
	mov	DWORD PTR[4+rsp],edx
	mov	ecx,r11d
	xor	ebp,DWORD PTR[16+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	ebp,DWORD PTR[40+rsp]
	lea	edi,DWORD PTR[1859775393+rdi*1+rdx]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[12+rsp]
	mov	eax,r11d
	mov	DWORD PTR[8+rsp],ebp
	mov	ecx,edi
	xor	r14d,DWORD PTR[20+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	r14d,DWORD PTR[44+rsp]
	lea	esi,DWORD PTR[1859775393+rsi*1+rbp]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	r14d,1
	xor	edx,DWORD PTR[16+rsp]
	mov	eax,edi
	mov	DWORD PTR[12+rsp],r14d
	mov	ecx,esi
	xor	edx,DWORD PTR[24+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	edx,DWORD PTR[48+rsp]
	lea	r13d,DWORD PTR[1859775393+r13*1+r14]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[20+rsp]
	mov	eax,esi
	mov	DWORD PTR[16+rsp],edx
	mov	ecx,r13d
	xor	ebp,DWORD PTR[28+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	ebp,DWORD PTR[52+rsp]
	lea	r12d,DWORD PTR[1859775393+r12*1+rdx]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[24+rsp]
	mov	eax,r13d
	mov	DWORD PTR[20+rsp],ebp
	mov	ecx,r12d
	xor	r14d,DWORD PTR[32+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	r14d,DWORD PTR[56+rsp]
	lea	r11d,DWORD PTR[1859775393+r11*1+rbp]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[28+rsp]
	mov	eax,r12d
	mov	DWORD PTR[24+rsp],r14d
	mov	ecx,r11d
	xor	edx,DWORD PTR[36+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	edx,DWORD PTR[60+rsp]
	lea	edi,DWORD PTR[1859775393+rdi*1+r14]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	edx,1
	xor	ebp,DWORD PTR[32+rsp]
	mov	eax,r11d
	mov	DWORD PTR[28+rsp],edx
	mov	ecx,edi
	xor	ebp,DWORD PTR[40+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	ebp,DWORD PTR[rsp]
	lea	esi,DWORD PTR[1859775393+rsi*1+rdx]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[36+rsp]
	mov	eax,r12d
	mov	DWORD PTR[32+rsp],ebp
	mov	ebx,r12d
	xor	r14d,DWORD PTR[44+rsp]
	and	eax,r11d
	mov	ecx,esi
	xor	r14d,DWORD PTR[4+rsp]
	lea	r13d,DWORD PTR[((-1894007588))+r13*1+rbp]
	xor	ebx,r11d
	rol	ecx,5
	add	r13d,eax
	rol	r14d,1
	and	ebx,edi
	add	r13d,ecx
	rol	edi,30
	add	r13d,ebx
	xor	edx,DWORD PTR[40+rsp]
	mov	eax,r11d
	mov	DWORD PTR[36+rsp],r14d
	mov	ebx,r11d
	xor	edx,DWORD PTR[48+rsp]
	and	eax,edi
	mov	ecx,r13d
	xor	edx,DWORD PTR[8+rsp]
	lea	r12d,DWORD PTR[((-1894007588))+r12*1+r14]
	xor	ebx,edi
	rol	ecx,5
	add	r12d,eax
	rol	edx,1
	and	ebx,esi
	add	r12d,ecx
	rol	esi,30
	add	r12d,ebx
	xor	ebp,DWORD PTR[44+rsp]
	mov	eax,edi
	mov	DWORD PTR[40+rsp],edx
	mov	ebx,edi
	xor	ebp,DWORD PTR[52+rsp]
	and	eax,esi
	mov	ecx,r12d
	xor	ebp,DWORD PTR[12+rsp]
	lea	r11d,DWORD PTR[((-1894007588))+r11*1+rdx]
	xor	ebx,esi
	rol	ecx,5
	add	r11d,eax
	rol	ebp,1
	and	ebx,r13d
	add	r11d,ecx
	rol	r13d,30
	add	r11d,ebx
	xor	r14d,DWORD PTR[48+rsp]
	mov	eax,esi
	mov	DWORD PTR[44+rsp],ebp
	mov	ebx,esi
	xor	r14d,DWORD PTR[56+rsp]
	and	eax,r13d
	mov	ecx,r11d
	xor	r14d,DWORD PTR[16+rsp]
	lea	edi,DWORD PTR[((-1894007588))+rdi*1+rbp]
	xor	ebx,r13d
	rol	ecx,5
	add	edi,eax
	rol	r14d,1
	and	ebx,r12d
	add	edi,ecx
	rol	r12d,30
	add	edi,ebx
	xor	edx,DWORD PTR[52+rsp]
	mov	eax,r13d
	mov	DWORD PTR[48+rsp],r14d
	mov	ebx,r13d
	xor	edx,DWORD PTR[60+rsp]
	and	eax,r12d
	mov	ecx,edi
	xor	edx,DWORD PTR[20+rsp]
	lea	esi,DWORD PTR[((-1894007588))+rsi*1+r14]
	xor	ebx,r12d
	rol	ecx,5
	add	esi,eax
	rol	edx,1
	and	ebx,r11d
	add	esi,ecx
	rol	r11d,30
	add	esi,ebx
	xor	ebp,DWORD PTR[56+rsp]
	mov	eax,r12d
	mov	DWORD PTR[52+rsp],edx
	mov	ebx,r12d
	xor	ebp,DWORD PTR[rsp]
	and	eax,r11d
	mov	ecx,esi
	xor	ebp,DWORD PTR[24+rsp]
	lea	r13d,DWORD PTR[((-1894007588))+r13*1+rdx]
	xor	ebx,r11d
	rol	ecx,5
	add	r13d,eax
	rol	ebp,1
	and	ebx,edi
	add	r13d,ecx
	rol	edi,30
	add	r13d,ebx
	xor	r14d,DWORD PTR[60+rsp]
	mov	eax,r11d
	mov	DWORD PTR[56+rsp],ebp
	mov	ebx,r11d
	xor	r14d,DWORD PTR[4+rsp]
	and	eax,edi
	mov	ecx,r13d
	xor	r14d,DWORD PTR[28+rsp]
	lea	r12d,DWORD PTR[((-1894007588))+r12*1+rbp]
	xor	ebx,edi
	rol	ecx,5
	add	r12d,eax
	rol	r14d,1
	and	ebx,esi
	add	r12d,ecx
	rol	esi,30
	add	r12d,ebx
	xor	edx,DWORD PTR[rsp]
	mov	eax,edi
	mov	DWORD PTR[60+rsp],r14d
	mov	ebx,edi
	xor	edx,DWORD PTR[8+rsp]
	and	eax,esi
	mov	ecx,r12d
	xor	edx,DWORD PTR[32+rsp]
	lea	r11d,DWORD PTR[((-1894007588))+r11*1+r14]
	xor	ebx,esi
	rol	ecx,5
	add	r11d,eax
	rol	edx,1
	and	ebx,r13d
	add	r11d,ecx
	rol	r13d,30
	add	r11d,ebx
	xor	ebp,DWORD PTR[4+rsp]
	mov	eax,esi
	mov	DWORD PTR[rsp],edx
	mov	ebx,esi
	xor	ebp,DWORD PTR[12+rsp]
	and	eax,r13d
	mov	ecx,r11d
	xor	ebp,DWORD PTR[36+rsp]
	lea	edi,DWORD PTR[((-1894007588))+rdi*1+rdx]
	xor	ebx,r13d
	rol	ecx,5
	add	edi,eax
	rol	ebp,1
	and	ebx,r12d
	add	edi,ecx
	rol	r12d,30
	add	edi,ebx
	xor	r14d,DWORD PTR[8+rsp]
	mov	eax,r13d
	mov	DWORD PTR[4+rsp],ebp
	mov	ebx,r13d
	xor	r14d,DWORD PTR[16+rsp]
	and	eax,r12d
	mov	ecx,edi
	xor	r14d,DWORD PTR[40+rsp]
	lea	esi,DWORD PTR[((-1894007588))+rsi*1+rbp]
	xor	ebx,r12d
	rol	ecx,5
	add	esi,eax
	rol	r14d,1
	and	ebx,r11d
	add	esi,ecx
	rol	r11d,30
	add	esi,ebx
	xor	edx,DWORD PTR[12+rsp]
	mov	eax,r12d
	mov	DWORD PTR[8+rsp],r14d
	mov	ebx,r12d
	xor	edx,DWORD PTR[20+rsp]
	and	eax,r11d
	mov	ecx,esi
	xor	edx,DWORD PTR[44+rsp]
	lea	r13d,DWORD PTR[((-1894007588))+r13*1+r14]
	xor	ebx,r11d
	rol	ecx,5
	add	r13d,eax
	rol	edx,1
	and	ebx,edi
	add	r13d,ecx
	rol	edi,30
	add	r13d,ebx
	xor	ebp,DWORD PTR[16+rsp]
	mov	eax,r11d
	mov	DWORD PTR[12+rsp],edx
	mov	ebx,r11d
	xor	ebp,DWORD PTR[24+rsp]
	and	eax,edi
	mov	ecx,r13d
	xor	ebp,DWORD PTR[48+rsp]
	lea	r12d,DWORD PTR[((-1894007588))+r12*1+rdx]
	xor	ebx,edi
	rol	ecx,5
	add	r12d,eax
	rol	ebp,1
	and	ebx,esi
	add	r12d,ecx
	rol	esi,30
	add	r12d,ebx
	xor	r14d,DWORD PTR[20+rsp]
	mov	eax,edi
	mov	DWORD PTR[16+rsp],ebp
	mov	ebx,edi
	xor	r14d,DWORD PTR[28+rsp]
	and	eax,esi
	mov	ecx,r12d
	xor	r14d,DWORD PTR[52+rsp]
	lea	r11d,DWORD PTR[((-1894007588))+r11*1+rbp]
	xor	ebx,esi
	rol	ecx,5
	add	r11d,eax
	rol	r14d,1
	and	ebx,r13d
	add	r11d,ecx
	rol	r13d,30
	add	r11d,ebx
	xor	edx,DWORD PTR[24+rsp]
	mov	eax,esi
	mov	DWORD PTR[20+rsp],r14d
	mov	ebx,esi
	xor	edx,DWORD PTR[32+rsp]
	and	eax,r13d
	mov	ecx,r11d
	xor	edx,DWORD PTR[56+rsp]
	lea	edi,DWORD PTR[((-1894007588))+rdi*1+r14]
	xor	ebx,r13d
	rol	ecx,5
	add	edi,eax
	rol	edx,1
	and	ebx,r12d
	add	edi,ecx
	rol	r12d,30
	add	edi,ebx
	xor	ebp,DWORD PTR[28+rsp]
	mov	eax,r13d
	mov	DWORD PTR[24+rsp],edx
	mov	ebx,r13d
	xor	ebp,DWORD PTR[36+rsp]
	and	eax,r12d
	mov	ecx,edi
	xor	ebp,DWORD PTR[60+rsp]
	lea	esi,DWORD PTR[((-1894007588))+rsi*1+rdx]
	xor	ebx,r12d
	rol	ecx,5
	add	esi,eax
	rol	ebp,1
	and	ebx,r11d
	add	esi,ecx
	rol	r11d,30
	add	esi,ebx
	xor	r14d,DWORD PTR[32+rsp]
	mov	eax,r12d
	mov	DWORD PTR[28+rsp],ebp
	mov	ebx,r12d
	xor	r14d,DWORD PTR[40+rsp]
	and	eax,r11d
	mov	ecx,esi
	xor	r14d,DWORD PTR[rsp]
	lea	r13d,DWORD PTR[((-1894007588))+r13*1+rbp]
	xor	ebx,r11d
	rol	ecx,5
	add	r13d,eax
	rol	r14d,1
	and	ebx,edi
	add	r13d,ecx
	rol	edi,30
	add	r13d,ebx
	xor	edx,DWORD PTR[36+rsp]
	mov	eax,r11d
	mov	DWORD PTR[32+rsp],r14d
	mov	ebx,r11d
	xor	edx,DWORD PTR[44+rsp]
	and	eax,edi
	mov	ecx,r13d
	xor	edx,DWORD PTR[4+rsp]
	lea	r12d,DWORD PTR[((-1894007588))+r12*1+r14]
	xor	ebx,edi
	rol	ecx,5
	add	r12d,eax
	rol	edx,1
	and	ebx,esi
	add	r12d,ecx
	rol	esi,30
	add	r12d,ebx
	xor	ebp,DWORD PTR[40+rsp]
	mov	eax,edi
	mov	DWORD PTR[36+rsp],edx
	mov	ebx,edi
	xor	ebp,DWORD PTR[48+rsp]
	and	eax,esi
	mov	ecx,r12d
	xor	ebp,DWORD PTR[8+rsp]
	lea	r11d,DWORD PTR[((-1894007588))+r11*1+rdx]
	xor	ebx,esi
	rol	ecx,5
	add	r11d,eax
	rol	ebp,1
	and	ebx,r13d
	add	r11d,ecx
	rol	r13d,30
	add	r11d,ebx
	xor	r14d,DWORD PTR[44+rsp]
	mov	eax,esi
	mov	DWORD PTR[40+rsp],ebp
	mov	ebx,esi
	xor	r14d,DWORD PTR[52+rsp]
	and	eax,r13d
	mov	ecx,r11d
	xor	r14d,DWORD PTR[12+rsp]
	lea	edi,DWORD PTR[((-1894007588))+rdi*1+rbp]
	xor	ebx,r13d
	rol	ecx,5
	add	edi,eax
	rol	r14d,1
	and	ebx,r12d
	add	edi,ecx
	rol	r12d,30
	add	edi,ebx
	xor	edx,DWORD PTR[48+rsp]
	mov	eax,r13d
	mov	DWORD PTR[44+rsp],r14d
	mov	ebx,r13d
	xor	edx,DWORD PTR[56+rsp]
	and	eax,r12d
	mov	ecx,edi
	xor	edx,DWORD PTR[16+rsp]
	lea	esi,DWORD PTR[((-1894007588))+rsi*1+r14]
	xor	ebx,r12d
	rol	ecx,5
	add	esi,eax
	rol	edx,1
	and	ebx,r11d
	add	esi,ecx
	rol	r11d,30
	add	esi,ebx
	xor	ebp,DWORD PTR[52+rsp]
	mov	eax,edi
	mov	DWORD PTR[48+rsp],edx
	mov	ecx,esi
	xor	ebp,DWORD PTR[60+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	ebp,DWORD PTR[20+rsp]
	lea	r13d,DWORD PTR[((-899497514))+r13*1+rdx]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[56+rsp]
	mov	eax,esi
	mov	DWORD PTR[52+rsp],ebp
	mov	ecx,r13d
	xor	r14d,DWORD PTR[rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	r14d,DWORD PTR[24+rsp]
	lea	r12d,DWORD PTR[((-899497514))+r12*1+rbp]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[60+rsp]
	mov	eax,r13d
	mov	DWORD PTR[56+rsp],r14d
	mov	ecx,r12d
	xor	edx,DWORD PTR[4+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	edx,DWORD PTR[28+rsp]
	lea	r11d,DWORD PTR[((-899497514))+r11*1+r14]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[rsp]
	mov	eax,r12d
	mov	DWORD PTR[60+rsp],edx
	mov	ecx,r11d
	xor	ebp,DWORD PTR[8+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	ebp,DWORD PTR[32+rsp]
	lea	edi,DWORD PTR[((-899497514))+rdi*1+rdx]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[4+rsp]
	mov	eax,r11d
	mov	DWORD PTR[rsp],ebp
	mov	ecx,edi
	xor	r14d,DWORD PTR[12+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	r14d,DWORD PTR[36+rsp]
	lea	esi,DWORD PTR[((-899497514))+rsi*1+rbp]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	r14d,1
	xor	edx,DWORD PTR[8+rsp]
	mov	eax,edi
	mov	DWORD PTR[4+rsp],r14d
	mov	ecx,esi
	xor	edx,DWORD PTR[16+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	edx,DWORD PTR[40+rsp]
	lea	r13d,DWORD PTR[((-899497514))+r13*1+r14]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[12+rsp]
	mov	eax,esi
	mov	DWORD PTR[8+rsp],edx
	mov	ecx,r13d
	xor	ebp,DWORD PTR[20+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	ebp,DWORD PTR[44+rsp]
	lea	r12d,DWORD PTR[((-899497514))+r12*1+rdx]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[16+rsp]
	mov	eax,r13d
	mov	DWORD PTR[12+rsp],ebp
	mov	ecx,r12d
	xor	r14d,DWORD PTR[24+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	r14d,DWORD PTR[48+rsp]
	lea	r11d,DWORD PTR[((-899497514))+r11*1+rbp]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[20+rsp]
	mov	eax,r12d
	mov	DWORD PTR[16+rsp],r14d
	mov	ecx,r11d
	xor	edx,DWORD PTR[28+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	edx,DWORD PTR[52+rsp]
	lea	edi,DWORD PTR[((-899497514))+rdi*1+r14]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	edx,1
	xor	ebp,DWORD PTR[24+rsp]
	mov	eax,r11d
	mov	DWORD PTR[20+rsp],edx
	mov	ecx,edi
	xor	ebp,DWORD PTR[32+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	ebp,DWORD PTR[56+rsp]
	lea	esi,DWORD PTR[((-899497514))+rsi*1+rdx]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[28+rsp]
	mov	eax,edi
	mov	DWORD PTR[24+rsp],ebp
	mov	ecx,esi
	xor	r14d,DWORD PTR[36+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	r14d,DWORD PTR[60+rsp]
	lea	r13d,DWORD PTR[((-899497514))+r13*1+rbp]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[32+rsp]
	mov	eax,esi
	mov	DWORD PTR[28+rsp],r14d
	mov	ecx,r13d
	xor	edx,DWORD PTR[40+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	edx,DWORD PTR[rsp]
	lea	r12d,DWORD PTR[((-899497514))+r12*1+r14]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[36+rsp]
	mov	eax,r13d

	mov	ecx,r12d
	xor	ebp,DWORD PTR[44+rsp]
	xor	eax,edi
	rol	ecx,5
	xor	ebp,DWORD PTR[4+rsp]
	lea	r11d,DWORD PTR[((-899497514))+r11*1+rdx]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[40+rsp]
	mov	eax,r12d

	mov	ecx,r11d
	xor	r14d,DWORD PTR[48+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	r14d,DWORD PTR[8+rsp]
	lea	edi,DWORD PTR[((-899497514))+rdi*1+rbp]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	r14d,1
	xor	edx,DWORD PTR[44+rsp]
	mov	eax,r11d

	mov	ecx,edi
	xor	edx,DWORD PTR[52+rsp]
	xor	eax,r13d
	rol	ecx,5
	xor	edx,DWORD PTR[12+rsp]
	lea	esi,DWORD PTR[((-899497514))+rsi*1+r14]
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	rol	edx,1
	xor	ebp,DWORD PTR[48+rsp]
	mov	eax,edi

	mov	ecx,esi
	xor	ebp,DWORD PTR[56+rsp]
	xor	eax,r12d
	rol	ecx,5
	xor	ebp,DWORD PTR[16+rsp]
	lea	r13d,DWORD PTR[((-899497514))+r13*1+rdx]
	xor	eax,r11d
	add	r13d,ecx
	rol	edi,30
	add	r13d,eax
	rol	ebp,1
	xor	r14d,DWORD PTR[52+rsp]
	mov	eax,esi

	mov	ecx,r13d
	xor	r14d,DWORD PTR[60+rsp]
	xor	eax,r11d
	rol	ecx,5
	xor	r14d,DWORD PTR[20+rsp]
	lea	r12d,DWORD PTR[((-899497514))+r12*1+rbp]
	xor	eax,edi
	add	r12d,ecx
	rol	esi,30
	add	r12d,eax
	rol	r14d,1
	xor	edx,DWORD PTR[56+rsp]
	mov	eax,r13d

	mov	ecx,r12d
	xor	edx,DWORD PTR[rsp]
	xor	eax,edi
	rol	ecx,5
	xor	edx,DWORD PTR[24+rsp]
	lea	r11d,DWORD PTR[((-899497514))+r11*1+r14]
	xor	eax,esi
	add	r11d,ecx
	rol	r13d,30
	add	r11d,eax
	rol	edx,1
	xor	ebp,DWORD PTR[60+rsp]
	mov	eax,r12d

	mov	ecx,r11d
	xor	ebp,DWORD PTR[4+rsp]
	xor	eax,esi
	rol	ecx,5
	xor	ebp,DWORD PTR[28+rsp]
	lea	edi,DWORD PTR[((-899497514))+rdi*1+rdx]
	xor	eax,r13d
	add	edi,ecx
	rol	r12d,30
	add	edi,eax
	rol	ebp,1
	mov	eax,r11d
	mov	ecx,edi
	xor	eax,r13d
	lea	esi,DWORD PTR[((-899497514))+rsi*1+rbp]
	rol	ecx,5
	xor	eax,r12d
	add	esi,ecx
	rol	r11d,30
	add	esi,eax
	add	esi,DWORD PTR[r8]
	add	edi,DWORD PTR[4+r8]
	add	r11d,DWORD PTR[8+r8]
	add	r12d,DWORD PTR[12+r8]
	add	r13d,DWORD PTR[16+r8]
	mov	DWORD PTR[r8],esi
	mov	DWORD PTR[4+r8],edi
	mov	DWORD PTR[8+r8],r11d
	mov	DWORD PTR[12+r8],r12d
	mov	DWORD PTR[16+r8],r13d

	sub	r10,1
	lea	r9,QWORD PTR[64+r9]
	jnz	$L$loop

	mov	rsi,QWORD PTR[64+rsp]
	mov	r14,QWORD PTR[((-40))+rsi]
	mov	r13,QWORD PTR[((-32))+rsi]
	mov	r12,QWORD PTR[((-24))+rsi]
	mov	rbp,QWORD PTR[((-16))+rsi]
	mov	rbx,QWORD PTR[((-8))+rsi]
	lea	rsp,QWORD PTR[rsi]
$L$epilogue::
	mov	rdi,QWORD PTR[8+rsp]	;WIN64 epilogue
	mov	rsi,QWORD PTR[16+rsp]
	DB	0F3h,0C3h		;repret
$L$SEH_end_sha1_block_data_order::
sha1_block_data_order	ENDP

ALIGN	32
sha1_block_data_order_shaext	PROC PRIVATE
	mov	QWORD PTR[8+rsp],rdi	;WIN64 prologue
	mov	QWORD PTR[16+rsp],rsi
	mov	rax,rsp
$L$SEH_begin_sha1_block_data_order_shaext::
	mov	rdi,rcx
	mov	rsi,rdx
	mov	rdx,r8


_shaext_shortcut::
	lea	rsp,QWORD PTR[((-72))+rsp]
	movaps	XMMWORD PTR[(-8-64)+rax],xmm6
	movaps	XMMWORD PTR[(-8-48)+rax],xmm7
	movaps	XMMWORD PTR[(-8-32)+rax],xmm8
	movaps	XMMWORD PTR[(-8-16)+rax],xmm9
$L$prologue_shaext::
	movdqu	xmm0,XMMWORD PTR[rdi]
	movd	xmm1,DWORD PTR[16+rdi]
	movdqa	xmm3,XMMWORD PTR[((K_XX_XX+160))]

	movdqu	xmm4,XMMWORD PTR[rsi]
	pshufd	xmm0,xmm0,27
	movdqu	xmm5,XMMWORD PTR[16+rsi]
	pshufd	xmm1,xmm1,27
	movdqu	xmm6,XMMWORD PTR[32+rsi]
DB	102,15,56,0,227
	movdqu	xmm7,XMMWORD PTR[48+rsi]
DB	102,15,56,0,235
DB	102,15,56,0,243
	movdqa	xmm9,xmm1
DB	102,15,56,0,251
	jmp	$L$oop_shaext

ALIGN	16
$L$oop_shaext::
	dec	rdx
	lea	r8,QWORD PTR[64+rsi]
	paddd	xmm1,xmm4
	cmovne	rsi,r8
	movdqa	xmm8,xmm0
DB	15,56,201,229
	movdqa	xmm2,xmm0
DB	15,58,204,193,0
DB	15,56,200,213
	pxor	xmm4,xmm6
DB	15,56,201,238
DB	15,56,202,231

	movdqa	xmm1,xmm0
DB	15,58,204,194,0
DB	15,56,200,206
	pxor	xmm5,xmm7
DB	15,56,202,236
DB	15,56,201,247
	movdqa	xmm2,xmm0
DB	15,58,204,193,0
DB	15,56,200,215
	pxor	xmm6,xmm4
DB	15,56,201,252
DB	15,56,202,245

	movdqa	xmm1,xmm0
DB	15,58,204,194,0
DB	15,56,200,204
	pxor	xmm7,xmm5
DB	15,56,202,254
DB	15,56,201,229
	movdqa	xmm2,xmm0
DB	15,58,204,193,0
DB	15,56,200,213
	pxor	xmm4,xmm6
DB	15,56,201,238
DB	15,56,202,231

	movdqa	xmm1,xmm0
DB	15,58,204,194,1
DB	15,56,200,206
	pxor	xmm5,xmm7
DB	15,56,202,236
DB	15,56,201,247
	movdqa	xmm2,xmm0
DB	15,58,204,193,1
DB	15,56,200,215
	pxor	xmm6,xmm4
DB	15,56,201,252
DB	15,56,202,245

	movdqa	xmm1,xmm0
DB	15,58,204,194,1
DB	15,56,200,204
	pxor	xmm7,xmm5
DB	15,56,202,254
DB	15,56,201,229
	movdqa	xmm2,xmm0
DB	15,58,204,193,1
DB	15,56,200,213
	pxor	xmm4,xmm6
DB	15,56,201,238
DB	15,56,202,231

	movdqa	xmm1,xmm0
DB	15,58,204,194,1
DB	15,56,200,206
	pxor	xmm5,xmm7
DB	15,56,202,236
DB	15,56,201,247
	movdqa	xmm2,xmm0
DB	15,58,204,193,2
DB	15,56,200,215
	pxor	xmm6,xmm4
DB	15,56,201,252
DB	15,56,202,245

	movdqa	xmm1,xmm0
DB	15,58,204,194,2
DB	15,56,200,204
	pxor	xmm7,xmm5
DB	15,56,202,254
DB	15,56,201,229
	movdqa	xmm2,xmm0
DB	15,58,204,193,2
DB	15,56,200,213
	pxor	xmm4,xmm6
DB	15,56,201,238
DB	15,56,202,231

	movdqa	xmm1,xmm0
DB	15,58,204,194,2
DB	15,56,200,206
	pxor	xmm5,xmm7
DB	15,56,202,236
DB	15,56,201,247
	movdqa	xmm2,xmm0
DB	15,58,204,193,2
DB	15,56,200,215
	pxor	xmm6,xmm4
DB	15,56,201,252
DB	15,56,202,245

	movdqa	xmm1,xmm0
DB	15,58,204,194,3
DB	15,56,200,204
	pxor	xmm7,xmm5
DB	15,56,202,254
	movdqu	xmm4,XMMWORD PTR[rsi]
	movdqa	xmm2,xmm0
DB	15,58,204,193,3
DB	15,56,200,213
	movdqu	xmm5,XMMWORD PTR[16+rsi]
DB	102,15,56,0,227

	movdqa	xmm1,xmm0
DB	15,58,204,194,3
DB	15,56,200,206
	movdqu	xmm6,XMMWORD PTR[32+rsi]
DB	102,15,56,0,235

	movdqa	xmm2,xmm0
DB	15,58,204,193,3
DB	15,56,200,215
	movdqu	xmm7,XMMWORD PTR[48+rsi]
DB	102,15,56,0,243

	movdqa	xmm1,xmm0
DB	15,58,204,194,3
DB	65,15,56,200,201
DB	102,15,56,0,251

	paddd	xmm0,xmm8
	movdqa	xmm9,xmm1

	jnz	$L$oop_shaext

	pshufd	xmm0,xmm0,27
	pshufd	xmm1,xmm1,27
	movdqu	XMMWORD PTR[rdi],xmm0
	movd	DWORD PTR[16+rdi],xmm1
	movaps	xmm6,XMMWORD PTR[((-8-64))+rax]
	movaps	xmm7,XMMWORD PTR[((-8-48))+rax]
	movaps	xmm8,XMMWORD PTR[((-8-32))+rax]
	movaps	xmm9,XMMWORD PTR[((-8-16))+rax]
	mov	rsp,rax
$L$epilogue_shaext::
	mov	rdi,QWORD PTR[8+rsp]	;WIN64 epilogue
	mov	rsi,QWORD PTR[16+rsp]
	DB	0F3h,0C3h		;repret
$L$SEH_end_sha1_block_data_order_shaext::
sha1_block_data_order_shaext	ENDP

ALIGN	16
sha1_block_data_order_ssse3	PROC PRIVATE
	mov	QWORD PTR[8+rsp],rdi	;WIN64 prologue
	mov	QWORD PTR[16+rsp],rsi
	mov	rax,rsp
$L$SEH_begin_sha1_block_data_order_ssse3::
	mov	rdi,rcx
	mov	rsi,rdx
	mov	rdx,r8


_ssse3_shortcut::
	mov	rax,rsp
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	lea	rsp,QWORD PTR[((-160))+rsp]
	movaps	XMMWORD PTR[(-40-96)+rax],xmm6
	movaps	XMMWORD PTR[(-40-80)+rax],xmm7
	movaps	XMMWORD PTR[(-40-64)+rax],xmm8
	movaps	XMMWORD PTR[(-40-48)+rax],xmm9
	movaps	XMMWORD PTR[(-40-32)+rax],xmm10
	movaps	XMMWORD PTR[(-40-16)+rax],xmm11
$L$prologue_ssse3::
	mov	r14,rax
	and	rsp,-64
	mov	r8,rdi
	mov	r9,rsi
	mov	r10,rdx

	shl	r10,6
	add	r10,r9
	lea	r11,QWORD PTR[((K_XX_XX+64))]

	mov	eax,DWORD PTR[r8]
	mov	ebx,DWORD PTR[4+r8]
	mov	ecx,DWORD PTR[8+r8]
	mov	edx,DWORD PTR[12+r8]
	mov	esi,ebx
	mov	ebp,DWORD PTR[16+r8]
	mov	edi,ecx
	xor	edi,edx
	and	esi,edi

	movdqa	xmm6,XMMWORD PTR[64+r11]
	movdqa	xmm9,XMMWORD PTR[((-64))+r11]
	movdqu	xmm0,XMMWORD PTR[r9]
	movdqu	xmm1,XMMWORD PTR[16+r9]
	movdqu	xmm2,XMMWORD PTR[32+r9]
	movdqu	xmm3,XMMWORD PTR[48+r9]
DB	102,15,56,0,198
DB	102,15,56,0,206
DB	102,15,56,0,214
	add	r9,64
	paddd	xmm0,xmm9
DB	102,15,56,0,222
	paddd	xmm1,xmm9
	paddd	xmm2,xmm9
	movdqa	XMMWORD PTR[rsp],xmm0
	psubd	xmm0,xmm9
	movdqa	XMMWORD PTR[16+rsp],xmm1
	psubd	xmm1,xmm9
	movdqa	XMMWORD PTR[32+rsp],xmm2
	psubd	xmm2,xmm9
	jmp	$L$oop_ssse3
ALIGN	16
$L$oop_ssse3::
	ror	ebx,2
	pshufd	xmm4,xmm0,238
	xor	esi,edx
	movdqa	xmm8,xmm3
	paddd	xmm9,xmm3
	mov	edi,eax
	add	ebp,DWORD PTR[rsp]
	punpcklqdq	xmm4,xmm1
	xor	ebx,ecx
	rol	eax,5
	add	ebp,esi
	psrldq	xmm8,4
	and	edi,ebx
	xor	ebx,ecx
	pxor	xmm4,xmm0
	add	ebp,eax
	ror	eax,7
	pxor	xmm8,xmm2
	xor	edi,ecx
	mov	esi,ebp
	add	edx,DWORD PTR[4+rsp]
	pxor	xmm4,xmm8
	xor	eax,ebx
	rol	ebp,5
	movdqa	XMMWORD PTR[48+rsp],xmm9
	add	edx,edi
	and	esi,eax
	movdqa	xmm10,xmm4
	xor	eax,ebx
	add	edx,ebp
	ror	ebp,7
	movdqa	xmm8,xmm4
	xor	esi,ebx
	pslldq	xmm10,12
	paddd	xmm4,xmm4
	mov	edi,edx
	add	ecx,DWORD PTR[8+rsp]
	psrld	xmm8,31
	xor	ebp,eax
	rol	edx,5
	add	ecx,esi
	movdqa	xmm9,xmm10
	and	edi,ebp
	xor	ebp,eax
	psrld	xmm10,30
	add	ecx,edx
	ror	edx,7
	por	xmm4,xmm8
	xor	edi,eax
	mov	esi,ecx
	add	ebx,DWORD PTR[12+rsp]
	pslld	xmm9,2
	pxor	xmm4,xmm10
	xor	edx,ebp
	movdqa	xmm10,XMMWORD PTR[((-64))+r11]
	rol	ecx,5
	add	ebx,edi
	and	esi,edx
	pxor	xmm4,xmm9
	xor	edx,ebp
	add	ebx,ecx
	ror	ecx,7
	pshufd	xmm5,xmm1,238
	xor	esi,ebp
	movdqa	xmm9,xmm4
	paddd	xmm10,xmm4
	mov	edi,ebx
	add	eax,DWORD PTR[16+rsp]
	punpcklqdq	xmm5,xmm2
	xor	ecx,edx
	rol	ebx,5
	add	eax,esi
	psrldq	xmm9,4
	and	edi,ecx
	xor	ecx,edx
	pxor	xmm5,xmm1
	add	eax,ebx
	ror	ebx,7
	pxor	xmm9,xmm3
	xor	edi,edx
	mov	esi,eax
	add	ebp,DWORD PTR[20+rsp]
	pxor	xmm5,xmm9
	xor	ebx,ecx
	rol	eax,5
	movdqa	XMMWORD PTR[rsp],xmm10
	add	ebp,edi
	and	esi,ebx
	movdqa	xmm8,xmm5
	xor	ebx,ecx
	add	ebp,eax
	ror	eax,7
	movdqa	xmm9,xmm5
	xor	esi,ecx
	pslldq	xmm8,12
	paddd	xmm5,xmm5
	mov	edi,ebp
	add	edx,DWORD PTR[24+rsp]
	psrld	xmm9,31
	xor	eax,ebx
	rol	ebp,5
	add	edx,esi
	movdqa	xmm10,xmm8
	and	edi,eax
	xor	eax,ebx
	psrld	xmm8,30
	add	edx,ebp
	ror	ebp,7
	por	xmm5,xmm9
	xor	edi,ebx
	mov	esi,edx
	add	ecx,DWORD PTR[28+rsp]
	pslld	xmm10,2
	pxor	xmm5,xmm8
	xor	ebp,eax
	movdqa	xmm8,XMMWORD PTR[((-32))+r11]
	rol	edx,5
	add	ecx,edi
	and	esi,ebp
	pxor	xmm5,xmm10
	xor	ebp,eax
	add	ecx,edx
	ror	edx,7
	pshufd	xmm6,xmm2,238
	xor	esi,eax
	movdqa	xmm10,xmm5
	paddd	xmm8,xmm5
	mov	edi,ecx
	add	ebx,DWORD PTR[32+rsp]
	punpcklqdq	xmm6,xmm3
	xor	edx,ebp
	rol	ecx,5
	add	ebx,esi
	psrldq	xmm10,4
	and	edi,edx
	xor	edx,ebp
	pxor	xmm6,xmm2
	add	ebx,ecx
	ror	ecx,7
	pxor	xmm10,xmm4
	xor	edi,ebp
	mov	esi,ebx
	add	eax,DWORD PTR[36+rsp]
	pxor	xmm6,xmm10
	xor	ecx,edx
	rol	ebx,5
	movdqa	XMMWORD PTR[16+rsp],xmm8
	add	eax,edi
	and	esi,ecx
	movdqa	xmm9,xmm6
	xor	ecx,edx
	add	eax,ebx
	ror	ebx,7
	movdqa	xmm10,xmm6
	xor	esi,edx
	pslldq	xmm9,12
	paddd	xmm6,xmm6
	mov	edi,eax
	add	ebp,DWORD PTR[40+rsp]
	psrld	xmm10,31
	xor	ebx,ecx
	rol	eax,5
	add	ebp,esi
	movdqa	xmm8,xmm9
	and	edi,ebx
	xor	ebx,ecx
	psrld	xmm9,30
	add	ebp,eax
	ror	eax,7
	por	xmm6,xmm10
	xor	edi,ecx
	mov	esi,ebp
	add	edx,DWORD PTR[44+rsp]
	pslld	xmm8,2
	pxor	xmm6,xmm9
	xor	eax,ebx
	movdqa	xmm9,XMMWORD PTR[((-32))+r11]
	rol	ebp,5
	add	edx,edi
	and	esi,eax
	pxor	xmm6,xmm8
	xor	eax,ebx
	add	edx,ebp
	ror	ebp,7
	pshufd	xmm7,xmm3,238
	xor	esi,ebx
	movdqa	xmm8,xmm6
	paddd	xmm9,xmm6
	mov	edi,edx
	add	ecx,DWORD PTR[48+rsp]
	punpcklqdq	xmm7,xmm4
	xor	ebp,eax
	rol	edx,5
	add	ecx,esi
	psrldq	xmm8,4
	and	edi,ebp
	xor	ebp,eax
	pxor	xmm7,xmm3
	add	ecx,edx
	ror	edx,7
	pxor	xmm8,xmm5
	xor	edi,eax
	mov	esi,ecx
	add	ebx,DWORD PTR[52+rsp]
	pxor	xmm7,xmm8
	xor	edx,ebp
	rol	ecx,5
	movdqa	XMMWORD PTR[32+rsp],xmm9
	add	ebx,edi
	and	esi,edx
	movdqa	xmm10,xmm7
	xor	edx,ebp
	add	ebx,ecx
	ror	ecx,7
	movdqa	xmm8,xmm7
	xor	esi,ebp
	pslldq	xmm10,12
	paddd	xmm7,xmm7
	mov	edi,ebx
	add	eax,DWORD PTR[56+rsp]
	psrld	xmm8,31
	xor	ecx,edx
	rol	ebx,5
	add	eax,esi
	movdqa	xmm9,xmm10
	and	edi,ecx
	xor	ecx,edx
	psrld	xmm10,30
	add	eax,ebx
	ror	ebx,7
	por	xmm7,xmm8
	xor	edi,edx
	mov	esi,eax
	add	ebp,DWORD PTR[60+rsp]
	pslld	xmm9,2
	pxor	xmm7,xmm10
	xor	ebx,ecx
	movdqa	xmm10,XMMWORD PTR[((-32))+r11]
	rol	eax,5
	add	ebp,edi
	and	esi,ebx
	pxor	xmm7,xmm9
	pshufd	xmm9,xmm6,238
	xor	ebx,ecx
	add	ebp,eax
	ror	eax,7
	pxor	xmm0,xmm4
	xor	esi,ecx
	mov	edi,ebp
	add	edx,DWORD PTR[rsp]
	punpcklqdq	xmm9,xmm7
	xor	eax,ebx
	rol	ebp,5
	pxor	xmm0,xmm1
	add	edx,esi
	and	edi,eax
	movdqa	xmm8,xmm10
	xor	eax,ebx
	paddd	xmm10,xmm7
	add	edx,ebp
	pxor	xmm0,xmm9
	ror	ebp,7
	xor	edi,ebx
	mov	esi,edx
	add	ecx,DWORD PTR[4+rsp]
	movdqa	xmm9,xmm0
	xor	ebp,eax
	rol	edx,5
	movdqa	XMMWORD PTR[48+rsp],xmm10
	add	ecx,edi
	and	esi,ebp
	xor	ebp,eax
	pslld	xmm0,2
	add	ecx,edx
	ror	edx,7
	psrld	xmm9,30
	xor	esi,eax
	mov	edi,ecx
	add	ebx,DWORD PTR[8+rsp]
	por	xmm0,xmm9
	xor	edx,ebp
	rol	ecx,5
	pshufd	xmm10,xmm7,238
	add	ebx,esi
	and	edi,edx
	xor	edx,ebp
	add	ebx,ecx
	add	eax,DWORD PTR[12+rsp]
	xor	edi,ebp
	mov	esi,ebx
	rol	ebx,5
	add	eax,edi
	xor	esi,edx
	ror	ecx,7
	add	eax,ebx
	pxor	xmm1,xmm5
	add	ebp,DWORD PTR[16+rsp]
	xor	esi,ecx
	punpcklqdq	xmm10,xmm0
	mov	edi,eax
	rol	eax,5
	pxor	xmm1,xmm2
	add	ebp,esi
	xor	edi,ecx
	movdqa	xmm9,xmm8
	ror	ebx,7
	paddd	xmm8,xmm0
	add	ebp,eax
	pxor	xmm1,xmm10
	add	edx,DWORD PTR[20+rsp]
	xor	edi,ebx
	mov	esi,ebp
	rol	ebp,5
	movdqa	xmm10,xmm1
	add	edx,edi
	xor	esi,ebx
	movdqa	XMMWORD PTR[rsp],xmm8
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[24+rsp]
	pslld	xmm1,2
	xor	esi,eax
	mov	edi,edx
	psrld	xmm10,30
	rol	edx,5
	add	ecx,esi
	xor	edi,eax
	ror	ebp,7
	por	xmm1,xmm10
	add	ecx,edx
	add	ebx,DWORD PTR[28+rsp]
	pshufd	xmm8,xmm0,238
	xor	edi,ebp
	mov	esi,ecx
	rol	ecx,5
	add	ebx,edi
	xor	esi,ebp
	ror	edx,7
	add	ebx,ecx
	pxor	xmm2,xmm6
	add	eax,DWORD PTR[32+rsp]
	xor	esi,edx
	punpcklqdq	xmm8,xmm1
	mov	edi,ebx
	rol	ebx,5
	pxor	xmm2,xmm3
	add	eax,esi
	xor	edi,edx
	movdqa	xmm10,XMMWORD PTR[r11]
	ror	ecx,7
	paddd	xmm9,xmm1
	add	eax,ebx
	pxor	xmm2,xmm8
	add	ebp,DWORD PTR[36+rsp]
	xor	edi,ecx
	mov	esi,eax
	rol	eax,5
	movdqa	xmm8,xmm2
	add	ebp,edi
	xor	esi,ecx
	movdqa	XMMWORD PTR[16+rsp],xmm9
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[40+rsp]
	pslld	xmm2,2
	xor	esi,ebx
	mov	edi,ebp
	psrld	xmm8,30
	rol	ebp,5
	add	edx,esi
	xor	edi,ebx
	ror	eax,7
	por	xmm2,xmm8
	add	edx,ebp
	add	ecx,DWORD PTR[44+rsp]
	pshufd	xmm9,xmm1,238
	xor	edi,eax
	mov	esi,edx
	rol	edx,5
	add	ecx,edi
	xor	esi,eax
	ror	ebp,7
	add	ecx,edx
	pxor	xmm3,xmm7
	add	ebx,DWORD PTR[48+rsp]
	xor	esi,ebp
	punpcklqdq	xmm9,xmm2
	mov	edi,ecx
	rol	ecx,5
	pxor	xmm3,xmm4
	add	ebx,esi
	xor	edi,ebp
	movdqa	xmm8,xmm10
	ror	edx,7
	paddd	xmm10,xmm2
	add	ebx,ecx
	pxor	xmm3,xmm9
	add	eax,DWORD PTR[52+rsp]
	xor	edi,edx
	mov	esi,ebx
	rol	ebx,5
	movdqa	xmm9,xmm3
	add	eax,edi
	xor	esi,edx
	movdqa	XMMWORD PTR[32+rsp],xmm10
	ror	ecx,7
	add	eax,ebx
	add	ebp,DWORD PTR[56+rsp]
	pslld	xmm3,2
	xor	esi,ecx
	mov	edi,eax
	psrld	xmm9,30
	rol	eax,5
	add	ebp,esi
	xor	edi,ecx
	ror	ebx,7
	por	xmm3,xmm9
	add	ebp,eax
	add	edx,DWORD PTR[60+rsp]
	pshufd	xmm10,xmm2,238
	xor	edi,ebx
	mov	esi,ebp
	rol	ebp,5
	add	edx,edi
	xor	esi,ebx
	ror	eax,7
	add	edx,ebp
	pxor	xmm4,xmm0
	add	ecx,DWORD PTR[rsp]
	xor	esi,eax
	punpcklqdq	xmm10,xmm3
	mov	edi,edx
	rol	edx,5
	pxor	xmm4,xmm5
	add	ecx,esi
	xor	edi,eax
	movdqa	xmm9,xmm8
	ror	ebp,7
	paddd	xmm8,xmm3
	add	ecx,edx
	pxor	xmm4,xmm10
	add	ebx,DWORD PTR[4+rsp]
	xor	edi,ebp
	mov	esi,ecx
	rol	ecx,5
	movdqa	xmm10,xmm4
	add	ebx,edi
	xor	esi,ebp
	movdqa	XMMWORD PTR[48+rsp],xmm8
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[8+rsp]
	pslld	xmm4,2
	xor	esi,edx
	mov	edi,ebx
	psrld	xmm10,30
	rol	ebx,5
	add	eax,esi
	xor	edi,edx
	ror	ecx,7
	por	xmm4,xmm10
	add	eax,ebx
	add	ebp,DWORD PTR[12+rsp]
	pshufd	xmm8,xmm3,238
	xor	edi,ecx
	mov	esi,eax
	rol	eax,5
	add	ebp,edi
	xor	esi,ecx
	ror	ebx,7
	add	ebp,eax
	pxor	xmm5,xmm1
	add	edx,DWORD PTR[16+rsp]
	xor	esi,ebx
	punpcklqdq	xmm8,xmm4
	mov	edi,ebp
	rol	ebp,5
	pxor	xmm5,xmm6
	add	edx,esi
	xor	edi,ebx
	movdqa	xmm10,xmm9
	ror	eax,7
	paddd	xmm9,xmm4
	add	edx,ebp
	pxor	xmm5,xmm8
	add	ecx,DWORD PTR[20+rsp]
	xor	edi,eax
	mov	esi,edx
	rol	edx,5
	movdqa	xmm8,xmm5
	add	ecx,edi
	xor	esi,eax
	movdqa	XMMWORD PTR[rsp],xmm9
	ror	ebp,7
	add	ecx,edx
	add	ebx,DWORD PTR[24+rsp]
	pslld	xmm5,2
	xor	esi,ebp
	mov	edi,ecx
	psrld	xmm8,30
	rol	ecx,5
	add	ebx,esi
	xor	edi,ebp
	ror	edx,7
	por	xmm5,xmm8
	add	ebx,ecx
	add	eax,DWORD PTR[28+rsp]
	pshufd	xmm9,xmm4,238
	ror	ecx,7
	mov	esi,ebx
	xor	edi,edx
	rol	ebx,5
	add	eax,edi
	xor	esi,ecx
	xor	ecx,edx
	add	eax,ebx
	pxor	xmm6,xmm2
	add	ebp,DWORD PTR[32+rsp]
	and	esi,ecx
	xor	ecx,edx
	ror	ebx,7
	punpcklqdq	xmm9,xmm5
	mov	edi,eax
	xor	esi,ecx
	pxor	xmm6,xmm7
	rol	eax,5
	add	ebp,esi
	movdqa	xmm8,xmm10
	xor	edi,ebx
	paddd	xmm10,xmm5
	xor	ebx,ecx
	pxor	xmm6,xmm9
	add	ebp,eax
	add	edx,DWORD PTR[36+rsp]
	and	edi,ebx
	xor	ebx,ecx
	ror	eax,7
	movdqa	xmm9,xmm6
	mov	esi,ebp
	xor	edi,ebx
	movdqa	XMMWORD PTR[16+rsp],xmm10
	rol	ebp,5
	add	edx,edi
	xor	esi,eax
	pslld	xmm6,2
	xor	eax,ebx
	add	edx,ebp
	psrld	xmm9,30
	add	ecx,DWORD PTR[40+rsp]
	and	esi,eax
	xor	eax,ebx
	por	xmm6,xmm9
	ror	ebp,7
	mov	edi,edx
	xor	esi,eax
	rol	edx,5
	pshufd	xmm10,xmm5,238
	add	ecx,esi
	xor	edi,ebp
	xor	ebp,eax
	add	ecx,edx
	add	ebx,DWORD PTR[44+rsp]
	and	edi,ebp
	xor	ebp,eax
	ror	edx,7
	mov	esi,ecx
	xor	edi,ebp
	rol	ecx,5
	add	ebx,edi
	xor	esi,edx
	xor	edx,ebp
	add	ebx,ecx
	pxor	xmm7,xmm3
	add	eax,DWORD PTR[48+rsp]
	and	esi,edx
	xor	edx,ebp
	ror	ecx,7
	punpcklqdq	xmm10,xmm6
	mov	edi,ebx
	xor	esi,edx
	pxor	xmm7,xmm0
	rol	ebx,5
	add	eax,esi
	movdqa	xmm9,XMMWORD PTR[32+r11]
	xor	edi,ecx
	paddd	xmm8,xmm6
	xor	ecx,edx
	pxor	xmm7,xmm10
	add	eax,ebx
	add	ebp,DWORD PTR[52+rsp]
	and	edi,ecx
	xor	ecx,edx
	ror	ebx,7
	movdqa	xmm10,xmm7
	mov	esi,eax
	xor	edi,ecx
	movdqa	XMMWORD PTR[32+rsp],xmm8
	rol	eax,5
	add	ebp,edi
	xor	esi,ebx
	pslld	xmm7,2
	xor	ebx,ecx
	add	ebp,eax
	psrld	xmm10,30
	add	edx,DWORD PTR[56+rsp]
	and	esi,ebx
	xor	ebx,ecx
	por	xmm7,xmm10
	ror	eax,7
	mov	edi,ebp
	xor	esi,ebx
	rol	ebp,5
	pshufd	xmm8,xmm6,238
	add	edx,esi
	xor	edi,eax
	xor	eax,ebx
	add	edx,ebp
	add	ecx,DWORD PTR[60+rsp]
	and	edi,eax
	xor	eax,ebx
	ror	ebp,7
	mov	esi,edx
	xor	edi,eax
	rol	edx,5
	add	ecx,edi
	xor	esi,ebp
	xor	ebp,eax
	add	ecx,edx
	pxor	xmm0,xmm4
	add	ebx,DWORD PTR[rsp]
	and	esi,ebp
	xor	ebp,eax
	ror	edx,7
	punpcklqdq	xmm8,xmm7
	mov	edi,ecx
	xor	esi,ebp
	pxor	xmm0,xmm1
	rol	ecx,5
	add	ebx,esi
	movdqa	xmm10,xmm9
	xor	edi,edx
	paddd	xmm9,xmm7
	xor	edx,ebp
	pxor	xmm0,xmm8
	add	ebx,ecx
	add	eax,DWORD PTR[4+rsp]
	and	edi,edx
	xor	edx,ebp
	ror	ecx,7
	movdqa	xmm8,xmm0
	mov	esi,ebx
	xor	edi,edx
	movdqa	XMMWORD PTR[48+rsp],xmm9
	rol	ebx,5
	add	eax,edi
	xor	esi,ecx
	pslld	xmm0,2
	xor	ecx,edx
	add	eax,ebx
	psrld	xmm8,30
	add	ebp,DWORD PTR[8+rsp]
	and	esi,ecx
	xor	ecx,edx
	por	xmm0,xmm8
	ror	ebx,7
	mov	edi,eax
	xor	esi,ecx
	rol	eax,5
	pshufd	xmm9,xmm7,238
	add	ebp,esi
	xor	edi,ebx
	xor	ebx,ecx
	add	ebp,eax
	add	edx,DWORD PTR[12+rsp]
	and	edi,ebx
	xor	ebx,ecx
	ror	eax,7
	mov	esi,ebp
	xor	edi,ebx
	rol	ebp,5
	add	edx,edi
	xor	esi,eax
	xor	eax,ebx
	add	edx,ebp
	pxor	xmm1,xmm5
	add	ecx,DWORD PTR[16+rsp]
	and	esi,eax
	xor	eax,ebx
	ror	ebp,7
	punpcklqdq	xmm9,xmm0
	mov	edi,edx
	xor	esi,eax
	pxor	xmm1,xmm2
	rol	edx,5
	add	ecx,esi
	movdqa	xmm8,xmm10
	xor	edi,ebp
	paddd	xmm10,xmm0
	xor	ebp,eax
	pxor	xmm1,xmm9
	add	ecx,edx
	add	ebx,DWORD PTR[20+rsp]
	and	edi,ebp
	xor	ebp,eax
	ror	edx,7
	movdqa	xmm9,xmm1
	mov	esi,ecx
	xor	edi,ebp
	movdqa	XMMWORD PTR[rsp],xmm10
	rol	ecx,5
	add	ebx,edi
	xor	esi,edx
	pslld	xmm1,2
	xor	edx,ebp
	add	ebx,ecx
	psrld	xmm9,30
	add	eax,DWORD PTR[24+rsp]
	and	esi,edx
	xor	edx,ebp
	por	xmm1,xmm9
	ror	ecx,7
	mov	edi,ebx
	xor	esi,edx
	rol	ebx,5
	pshufd	xmm10,xmm0,238
	add	eax,esi
	xor	edi,ecx
	xor	ecx,edx
	add	eax,ebx
	add	ebp,DWORD PTR[28+rsp]
	and	edi,ecx
	xor	ecx,edx
	ror	ebx,7
	mov	esi,eax
	xor	edi,ecx
	rol	eax,5
	add	ebp,edi
	xor	esi,ebx
	xor	ebx,ecx
	add	ebp,eax
	pxor	xmm2,xmm6
	add	edx,DWORD PTR[32+rsp]
	and	esi,ebx
	xor	ebx,ecx
	ror	eax,7
	punpcklqdq	xmm10,xmm1
	mov	edi,ebp
	xor	esi,ebx
	pxor	xmm2,xmm3
	rol	ebp,5
	add	edx,esi
	movdqa	xmm9,xmm8
	xor	edi,eax
	paddd	xmm8,xmm1
	xor	eax,ebx
	pxor	xmm2,xmm10
	add	edx,ebp
	add	ecx,DWORD PTR[36+rsp]
	and	edi,eax
	xor	eax,ebx
	ror	ebp,7
	movdqa	xmm10,xmm2
	mov	esi,edx
	xor	edi,eax
	movdqa	XMMWORD PTR[16+rsp],xmm8
	rol	edx,5
	add	ecx,edi
	xor	esi,ebp
	pslld	xmm2,2
	xor	ebp,eax
	add	ecx,edx
	psrld	xmm10,30
	add	ebx,DWORD PTR[40+rsp]
	and	esi,ebp
	xor	ebp,eax
	por	xmm2,xmm10
	ror	edx,7
	mov	edi,ecx
	xor	esi,ebp
	rol	ecx,5
	pshufd	xmm8,xmm1,238
	add	ebx,esi
	xor	edi,edx
	xor	edx,ebp
	add	ebx,ecx
	add	eax,DWORD PTR[44+rsp]
	and	edi,edx
	xor	edx,ebp
	ror	ecx,7
	mov	esi,ebx
	xor	edi,edx
	rol	ebx,5
	add	eax,edi
	xor	esi,edx
	add	eax,ebx
	pxor	xmm3,xmm7
	add	ebp,DWORD PTR[48+rsp]
	xor	esi,ecx
	punpcklqdq	xmm8,xmm2
	mov	edi,eax
	rol	eax,5
	pxor	xmm3,xmm4
	add	ebp,esi
	xor	edi,ecx
	movdqa	xmm10,xmm9
	ror	ebx,7
	paddd	xmm9,xmm2
	add	ebp,eax
	pxor	xmm3,xmm8
	add	edx,DWORD PTR[52+rsp]
	xor	edi,ebx
	mov	esi,ebp
	rol	ebp,5
	movdqa	xmm8,xmm3
	add	edx,edi
	xor	esi,ebx
	movdqa	XMMWORD PTR[32+rsp],xmm9
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[56+rsp]
	pslld	xmm3,2
	xor	esi,eax
	mov	edi,edx
	psrld	xmm8,30
	rol	edx,5
	add	ecx,esi
	xor	edi,eax
	ror	ebp,7
	por	xmm3,xmm8
	add	ecx,edx
	add	ebx,DWORD PTR[60+rsp]
	xor	edi,ebp
	mov	esi,ecx
	rol	ecx,5
	add	ebx,edi
	xor	esi,ebp
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[rsp]
	xor	esi,edx
	mov	edi,ebx
	rol	ebx,5
	paddd	xmm10,xmm3
	add	eax,esi
	xor	edi,edx
	movdqa	XMMWORD PTR[48+rsp],xmm10
	ror	ecx,7
	add	eax,ebx
	add	ebp,DWORD PTR[4+rsp]
	xor	edi,ecx
	mov	esi,eax
	rol	eax,5
	add	ebp,edi
	xor	esi,ecx
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[8+rsp]
	xor	esi,ebx
	mov	edi,ebp
	rol	ebp,5
	add	edx,esi
	xor	edi,ebx
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[12+rsp]
	xor	edi,eax
	mov	esi,edx
	rol	edx,5
	add	ecx,edi
	xor	esi,eax
	ror	ebp,7
	add	ecx,edx
	cmp	r9,r10
	je	$L$done_ssse3
	movdqa	xmm6,XMMWORD PTR[64+r11]
	movdqa	xmm9,XMMWORD PTR[((-64))+r11]
	movdqu	xmm0,XMMWORD PTR[r9]
	movdqu	xmm1,XMMWORD PTR[16+r9]
	movdqu	xmm2,XMMWORD PTR[32+r9]
	movdqu	xmm3,XMMWORD PTR[48+r9]
DB	102,15,56,0,198
	add	r9,64
	add	ebx,DWORD PTR[16+rsp]
	xor	esi,ebp
	mov	edi,ecx
DB	102,15,56,0,206
	rol	ecx,5
	add	ebx,esi
	xor	edi,ebp
	ror	edx,7
	paddd	xmm0,xmm9
	add	ebx,ecx
	add	eax,DWORD PTR[20+rsp]
	xor	edi,edx
	mov	esi,ebx
	movdqa	XMMWORD PTR[rsp],xmm0
	rol	ebx,5
	add	eax,edi
	xor	esi,edx
	ror	ecx,7
	psubd	xmm0,xmm9
	add	eax,ebx
	add	ebp,DWORD PTR[24+rsp]
	xor	esi,ecx
	mov	edi,eax
	rol	eax,5
	add	ebp,esi
	xor	edi,ecx
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[28+rsp]
	xor	edi,ebx
	mov	esi,ebp
	rol	ebp,5
	add	edx,edi
	xor	esi,ebx
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[32+rsp]
	xor	esi,eax
	mov	edi,edx
DB	102,15,56,0,214
	rol	edx,5
	add	ecx,esi
	xor	edi,eax
	ror	ebp,7
	paddd	xmm1,xmm9
	add	ecx,edx
	add	ebx,DWORD PTR[36+rsp]
	xor	edi,ebp
	mov	esi,ecx
	movdqa	XMMWORD PTR[16+rsp],xmm1
	rol	ecx,5
	add	ebx,edi
	xor	esi,ebp
	ror	edx,7
	psubd	xmm1,xmm9
	add	ebx,ecx
	add	eax,DWORD PTR[40+rsp]
	xor	esi,edx
	mov	edi,ebx
	rol	ebx,5
	add	eax,esi
	xor	edi,edx
	ror	ecx,7
	add	eax,ebx
	add	ebp,DWORD PTR[44+rsp]
	xor	edi,ecx
	mov	esi,eax
	rol	eax,5
	add	ebp,edi
	xor	esi,ecx
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[48+rsp]
	xor	esi,ebx
	mov	edi,ebp
DB	102,15,56,0,222
	rol	ebp,5
	add	edx,esi
	xor	edi,ebx
	ror	eax,7
	paddd	xmm2,xmm9
	add	edx,ebp
	add	ecx,DWORD PTR[52+rsp]
	xor	edi,eax
	mov	esi,edx
	movdqa	XMMWORD PTR[32+rsp],xmm2
	rol	edx,5
	add	ecx,edi
	xor	esi,eax
	ror	ebp,7
	psubd	xmm2,xmm9
	add	ecx,edx
	add	ebx,DWORD PTR[56+rsp]
	xor	esi,ebp
	mov	edi,ecx
	rol	ecx,5
	add	ebx,esi
	xor	edi,ebp
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[60+rsp]
	xor	edi,edx
	mov	esi,ebx
	rol	ebx,5
	add	eax,edi
	ror	ecx,7
	add	eax,ebx
	add	eax,DWORD PTR[r8]
	add	esi,DWORD PTR[4+r8]
	add	ecx,DWORD PTR[8+r8]
	add	edx,DWORD PTR[12+r8]
	mov	DWORD PTR[r8],eax
	add	ebp,DWORD PTR[16+r8]
	mov	DWORD PTR[4+r8],esi
	mov	ebx,esi
	mov	DWORD PTR[8+r8],ecx
	mov	edi,ecx
	mov	DWORD PTR[12+r8],edx
	xor	edi,edx
	mov	DWORD PTR[16+r8],ebp
	and	esi,edi
	jmp	$L$oop_ssse3

ALIGN	16
$L$done_ssse3::
	add	ebx,DWORD PTR[16+rsp]
	xor	esi,ebp
	mov	edi,ecx
	rol	ecx,5
	add	ebx,esi
	xor	edi,ebp
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[20+rsp]
	xor	edi,edx
	mov	esi,ebx
	rol	ebx,5
	add	eax,edi
	xor	esi,edx
	ror	ecx,7
	add	eax,ebx
	add	ebp,DWORD PTR[24+rsp]
	xor	esi,ecx
	mov	edi,eax
	rol	eax,5
	add	ebp,esi
	xor	edi,ecx
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[28+rsp]
	xor	edi,ebx
	mov	esi,ebp
	rol	ebp,5
	add	edx,edi
	xor	esi,ebx
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[32+rsp]
	xor	esi,eax
	mov	edi,edx
	rol	edx,5
	add	ecx,esi
	xor	edi,eax
	ror	ebp,7
	add	ecx,edx
	add	ebx,DWORD PTR[36+rsp]
	xor	edi,ebp
	mov	esi,ecx
	rol	ecx,5
	add	ebx,edi
	xor	esi,ebp
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[40+rsp]
	xor	esi,edx
	mov	edi,ebx
	rol	ebx,5
	add	eax,esi
	xor	edi,edx
	ror	ecx,7
	add	eax,ebx
	add	ebp,DWORD PTR[44+rsp]
	xor	edi,ecx
	mov	esi,eax
	rol	eax,5
	add	ebp,edi
	xor	esi,ecx
	ror	ebx,7
	add	ebp,eax
	add	edx,DWORD PTR[48+rsp]
	xor	esi,ebx
	mov	edi,ebp
	rol	ebp,5
	add	edx,esi
	xor	edi,ebx
	ror	eax,7
	add	edx,ebp
	add	ecx,DWORD PTR[52+rsp]
	xor	edi,eax
	mov	esi,edx
	rol	edx,5
	add	ecx,edi
	xor	esi,eax
	ror	ebp,7
	add	ecx,edx
	add	ebx,DWORD PTR[56+rsp]
	xor	esi,ebp
	mov	edi,ecx
	rol	ecx,5
	add	ebx,esi
	xor	edi,ebp
	ror	edx,7
	add	ebx,ecx
	add	eax,DWORD PTR[60+rsp]
	xor	edi,edx
	mov	esi,ebx
	rol	ebx,5
	add	eax,edi
	ror	ecx,7
	add	eax,ebx
	add	eax,DWORD PTR[r8]
	add	esi,DWORD PTR[4+r8]
	add	ecx,DWORD PTR[8+r8]
	mov	DWORD PTR[r8],eax
	add	edx,DWORD PTR[12+r8]
	mov	DWORD PTR[4+r8],esi
	add	ebp,DWORD PTR[16+r8]
	mov	DWORD PTR[8+r8],ecx
	mov	DWORD PTR[12+r8],edx
	mov	DWORD PTR[16+r8],ebp
	movaps	xmm6,XMMWORD PTR[((-40-96))+r14]
	movaps	xmm7,XMMWORD PTR[((-40-80))+r14]
	movaps	xmm8,XMMWORD PTR[((-40-64))+r14]
	movaps	xmm9,XMMWORD PTR[((-40-48))+r14]
	movaps	xmm10,XMMWORD PTR[((-40-32))+r14]
	movaps	xmm11,XMMWORD PTR[((-40-16))+r14]
	lea	rsi,QWORD PTR[r14]
	mov	r14,QWORD PTR[((-40))+rsi]
	mov	r13,QWORD PTR[((-32))+rsi]
	mov	r12,QWORD PTR[((-24))+rsi]
	mov	rbp,QWORD PTR[((-16))+rsi]
	mov	rbx,QWORD PTR[((-8))+rsi]
	lea	rsp,QWORD PTR[rsi]
$L$epilogue_ssse3::
	mov	rdi,QWORD PTR[8+rsp]	;WIN64 epilogue
	mov	rsi,QWORD PTR[16+rsp]
	DB	0F3h,0C3h		;repret
$L$SEH_end_sha1_block_data_order_ssse3::
sha1_block_data_order_ssse3	ENDP
ALIGN	64
K_XX_XX::
	DD	05a827999h,05a827999h,05a827999h,05a827999h
	DD	05a827999h,05a827999h,05a827999h,05a827999h
	DD	06ed9eba1h,06ed9eba1h,06ed9eba1h,06ed9eba1h
	DD	06ed9eba1h,06ed9eba1h,06ed9eba1h,06ed9eba1h
	DD	08f1bbcdch,08f1bbcdch,08f1bbcdch,08f1bbcdch
	DD	08f1bbcdch,08f1bbcdch,08f1bbcdch,08f1bbcdch
	DD	0ca62c1d6h,0ca62c1d6h,0ca62c1d6h,0ca62c1d6h
	DD	0ca62c1d6h,0ca62c1d6h,0ca62c1d6h,0ca62c1d6h
	DD	000010203h,004050607h,008090a0bh,00c0d0e0fh
	DD	000010203h,004050607h,008090a0bh,00c0d0e0fh
DB	0fh,0eh,0dh,0ch,0bh,0ah,09h,08h,07h,06h,05h,04h,03h,02h,01h,00h
DB	83,72,65,49,32,98,108,111,99,107,32,116,114,97,110,115
DB	102,111,114,109,32,102,111,114,32,120,56,54,95,54,52,44
DB	32,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60
DB	97,112,112,114,111,64,111,112,101,110,115,115,108,46,111,114
DB	103,62,0
ALIGN	64
EXTERN	__imp_RtlVirtualUnwind:NEAR

ALIGN	16
se_handler	PROC PRIVATE
	push	rsi
	push	rdi
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15
	pushfq
	sub	rsp,64

	mov	rax,QWORD PTR[120+r8]
	mov	rbx,QWORD PTR[248+r8]

	lea	r10,QWORD PTR[$L$prologue]
	cmp	rbx,r10
	jb	$L$common_seh_tail

	mov	rax,QWORD PTR[152+r8]

	lea	r10,QWORD PTR[$L$epilogue]
	cmp	rbx,r10
	jae	$L$common_seh_tail

	mov	rax,QWORD PTR[64+rax]

	mov	rbx,QWORD PTR[((-8))+rax]
	mov	rbp,QWORD PTR[((-16))+rax]
	mov	r12,QWORD PTR[((-24))+rax]
	mov	r13,QWORD PTR[((-32))+rax]
	mov	r14,QWORD PTR[((-40))+rax]
	mov	QWORD PTR[144+r8],rbx
	mov	QWORD PTR[160+r8],rbp
	mov	QWORD PTR[216+r8],r12
	mov	QWORD PTR[224+r8],r13
	mov	QWORD PTR[232+r8],r14

	jmp	$L$common_seh_tail
se_handler	ENDP

ALIGN	16
shaext_handler	PROC PRIVATE
	push	rsi
	push	rdi
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15
	pushfq
	sub	rsp,64

	mov	rax,QWORD PTR[120+r8]
	mov	rbx,QWORD PTR[248+r8]

	lea	r10,QWORD PTR[$L$prologue_shaext]
	cmp	rbx,r10
	jb	$L$common_seh_tail

	lea	r10,QWORD PTR[$L$epilogue_shaext]
	cmp	rbx,r10
	jae	$L$common_seh_tail

	lea	rsi,QWORD PTR[((-8-64))+rax]
	lea	rdi,QWORD PTR[512+r8]
	mov	ecx,8
	DD	0a548f3fch

	jmp	$L$common_seh_tail
shaext_handler	ENDP

ALIGN	16
ssse3_handler	PROC PRIVATE
	push	rsi
	push	rdi
	push	rbx
	push	rbp
	push	r12
	push	r13
	push	r14
	push	r15
	pushfq
	sub	rsp,64

	mov	rax,QWORD PTR[120+r8]
	mov	rbx,QWORD PTR[248+r8]

	mov	rsi,QWORD PTR[8+r9]
	mov	r11,QWORD PTR[56+r9]

	mov	r10d,DWORD PTR[r11]
	lea	r10,QWORD PTR[r10*1+rsi]
	cmp	rbx,r10
	jb	$L$common_seh_tail

	mov	rax,QWORD PTR[152+r8]

	mov	r10d,DWORD PTR[4+r11]
	lea	r10,QWORD PTR[r10*1+rsi]
	cmp	rbx,r10
	jae	$L$common_seh_tail

	mov	rax,QWORD PTR[232+r8]

	lea	rsi,QWORD PTR[((-40-96))+rax]
	lea	rdi,QWORD PTR[512+r8]
	mov	ecx,12
	DD	0a548f3fch

	mov	rbx,QWORD PTR[((-8))+rax]
	mov	rbp,QWORD PTR[((-16))+rax]
	mov	r12,QWORD PTR[((-24))+rax]
	mov	r13,QWORD PTR[((-32))+rax]
	mov	r14,QWORD PTR[((-40))+rax]
	mov	QWORD PTR[144+r8],rbx
	mov	QWORD PTR[160+r8],rbp
	mov	QWORD PTR[216+r8],r12
	mov	QWORD PTR[224+r8],r13
	mov	QWORD PTR[232+r8],r14

$L$common_seh_tail::
	mov	rdi,QWORD PTR[8+rax]
	mov	rsi,QWORD PTR[16+rax]
	mov	QWORD PTR[152+r8],rax
	mov	QWORD PTR[168+r8],rsi
	mov	QWORD PTR[176+r8],rdi

	mov	rdi,QWORD PTR[40+r9]
	mov	rsi,r8
	mov	ecx,154
	DD	0a548f3fch

	mov	rsi,r9
	xor	rcx,rcx
	mov	rdx,QWORD PTR[8+rsi]
	mov	r8,QWORD PTR[rsi]
	mov	r9,QWORD PTR[16+rsi]
	mov	r10,QWORD PTR[40+rsi]
	lea	r11,QWORD PTR[56+rsi]
	lea	r12,QWORD PTR[24+rsi]
	mov	QWORD PTR[32+rsp],r10
	mov	QWORD PTR[40+rsp],r11
	mov	QWORD PTR[48+rsp],r12
	mov	QWORD PTR[56+rsp],rcx
	call	QWORD PTR[__imp_RtlVirtualUnwind]

	mov	eax,1
	add	rsp,64
	popfq
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbp
	pop	rbx
	pop	rdi
	pop	rsi
	DB	0F3h,0C3h		;repret
ssse3_handler	ENDP

.text$	ENDS
.pdata	SEGMENT READONLY ALIGN(4)
ALIGN	4
	DD	imagerel $L$SEH_begin_sha1_block_data_order
	DD	imagerel $L$SEH_end_sha1_block_data_order
	DD	imagerel $L$SEH_info_sha1_block_data_order
	DD	imagerel $L$SEH_begin_sha1_block_data_order_shaext
	DD	imagerel $L$SEH_end_sha1_block_data_order_shaext
	DD	imagerel $L$SEH_info_sha1_block_data_order_shaext
	DD	imagerel $L$SEH_begin_sha1_block_data_order_ssse3
	DD	imagerel $L$SEH_end_sha1_block_data_order_ssse3
	DD	imagerel $L$SEH_info_sha1_block_data_order_ssse3
.pdata	ENDS
.xdata	SEGMENT READONLY ALIGN(8)
ALIGN	8
$L$SEH_info_sha1_block_data_order::
DB	9,0,0,0
	DD	imagerel se_handler
$L$SEH_info_sha1_block_data_order_shaext::
DB	9,0,0,0
	DD	imagerel shaext_handler
$L$SEH_info_sha1_block_data_order_ssse3::
DB	9,0,0,0
	DD	imagerel ssse3_handler
	DD	imagerel $L$prologue_ssse3,imagerel $L$epilogue_ssse3

.xdata	ENDS
END
