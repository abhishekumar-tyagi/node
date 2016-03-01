TITLE	sha512-586.asm
IF @Version LT 800
ECHO MASM version 8.00 or later is strongly recommended.
ENDIF
.686
.XMM
IF @Version LT 800
XMMWORD STRUCT 16
DQ	2 dup (?)
XMMWORD	ENDS
ENDIF

.MODEL	FLAT
OPTION	DOTNAME
IF @Version LT 800
.text$	SEGMENT PAGE 'CODE'
ELSE
.text$	SEGMENT ALIGN(64) 'CODE'
ENDIF
;EXTERN	_OPENSSL_ia32cap_P:NEAR
ALIGN	16
_sha256_block_data_order	PROC PUBLIC
$L_sha256_block_data_order_begin::
	push	ebp
	push	ebx
	push	esi
	push	edi
	mov	esi,DWORD PTR 20[esp]
	mov	edi,DWORD PTR 24[esp]
	mov	eax,DWORD PTR 28[esp]
	mov	ebx,esp
	call	$L000pic_point
$L000pic_point:
	pop	ebp
	lea	ebp,DWORD PTR ($L001K256-$L000pic_point)[ebp]
	sub	esp,16
	and	esp,-64
	shl	eax,6
	add	eax,edi
	mov	DWORD PTR [esp],esi
	mov	DWORD PTR 4[esp],edi
	mov	DWORD PTR 8[esp],eax
	mov	DWORD PTR 12[esp],ebx
	lea	edx,DWORD PTR _OPENSSL_ia32cap_P
	mov	ecx,DWORD PTR [edx]
	mov	ebx,DWORD PTR 4[edx]
	test	ecx,1048576
	jnz	$L002loop
	mov	edx,DWORD PTR 8[edx]
	test	ecx,16777216
	jz	$L003no_xmm
	and	ecx,1073741824
	and	ebx,268435968
	test	edx,536870912
	jnz	$L004shaext
	or	ecx,ebx
	and	ecx,1342177280
	cmp	ecx,1342177280
	test	ebx,512
	jnz	$L005SSSE3
$L003no_xmm:
	sub	eax,edi
	cmp	eax,256
	jae	$L006unrolled
	jmp	$L002loop
ALIGN	16
$L002loop:
	mov	eax,DWORD PTR [edi]
	mov	ebx,DWORD PTR 4[edi]
	mov	ecx,DWORD PTR 8[edi]
	bswap	eax
	mov	edx,DWORD PTR 12[edi]
	bswap	ebx
	push	eax
	bswap	ecx
	push	ebx
	bswap	edx
	push	ecx
	push	edx
	mov	eax,DWORD PTR 16[edi]
	mov	ebx,DWORD PTR 20[edi]
	mov	ecx,DWORD PTR 24[edi]
	bswap	eax
	mov	edx,DWORD PTR 28[edi]
	bswap	ebx
	push	eax
	bswap	ecx
	push	ebx
	bswap	edx
	push	ecx
	push	edx
	mov	eax,DWORD PTR 32[edi]
	mov	ebx,DWORD PTR 36[edi]
	mov	ecx,DWORD PTR 40[edi]
	bswap	eax
	mov	edx,DWORD PTR 44[edi]
	bswap	ebx
	push	eax
	bswap	ecx
	push	ebx
	bswap	edx
	push	ecx
	push	edx
	mov	eax,DWORD PTR 48[edi]
	mov	ebx,DWORD PTR 52[edi]
	mov	ecx,DWORD PTR 56[edi]
	bswap	eax
	mov	edx,DWORD PTR 60[edi]
	bswap	ebx
	push	eax
	bswap	ecx
	push	ebx
	bswap	edx
	push	ecx
	push	edx
	add	edi,64
	lea	esp,DWORD PTR [esp-36]
	mov	DWORD PTR 104[esp],edi
	mov	eax,DWORD PTR [esi]
	mov	ebx,DWORD PTR 4[esi]
	mov	ecx,DWORD PTR 8[esi]
	mov	edi,DWORD PTR 12[esi]
	mov	DWORD PTR 8[esp],ebx
	xor	ebx,ecx
	mov	DWORD PTR 12[esp],ecx
	mov	DWORD PTR 16[esp],edi
	mov	DWORD PTR [esp],ebx
	mov	edx,DWORD PTR 16[esi]
	mov	ebx,DWORD PTR 20[esi]
	mov	ecx,DWORD PTR 24[esi]
	mov	edi,DWORD PTR 28[esi]
	mov	DWORD PTR 24[esp],ebx
	mov	DWORD PTR 28[esp],ecx
	mov	DWORD PTR 32[esp],edi
ALIGN	16
$L00700_15:
	mov	ecx,edx
	mov	esi,DWORD PTR 24[esp]
	ror	ecx,14
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,edx
	xor	esi,edi
	mov	ebx,DWORD PTR 96[esp]
	ror	ecx,5
	and	esi,edx
	mov	DWORD PTR 20[esp],edx
	xor	edx,ecx
	add	ebx,DWORD PTR 32[esp]
	xor	esi,edi
	ror	edx,6
	mov	ecx,eax
	add	ebx,esi
	ror	ecx,9
	add	ebx,edx
	mov	edi,DWORD PTR 8[esp]
	xor	ecx,eax
	mov	DWORD PTR 4[esp],eax
	lea	esp,DWORD PTR [esp-4]
	ror	ecx,11
	mov	esi,DWORD PTR [ebp]
	xor	ecx,eax
	mov	edx,DWORD PTR 20[esp]
	xor	eax,edi
	ror	ecx,2
	add	ebx,esi
	mov	DWORD PTR [esp],eax
	add	edx,ebx
	and	eax,DWORD PTR 4[esp]
	add	ebx,ecx
	xor	eax,edi
	add	ebp,4
	add	eax,ebx
	cmp	esi,3248222580
	jne	$L00700_15
	mov	ecx,DWORD PTR 156[esp]
	jmp	$L00816_63
ALIGN	16
$L00816_63:
	mov	ebx,ecx
	mov	esi,DWORD PTR 104[esp]
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 160[esp]
	shr	edi,10
	add	ebx,DWORD PTR 124[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 24[esp]
	ror	ecx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,edx
	xor	esi,edi
	mov	DWORD PTR 96[esp],ebx
	ror	ecx,5
	and	esi,edx
	mov	DWORD PTR 20[esp],edx
	xor	edx,ecx
	add	ebx,DWORD PTR 32[esp]
	xor	esi,edi
	ror	edx,6
	mov	ecx,eax
	add	ebx,esi
	ror	ecx,9
	add	ebx,edx
	mov	edi,DWORD PTR 8[esp]
	xor	ecx,eax
	mov	DWORD PTR 4[esp],eax
	lea	esp,DWORD PTR [esp-4]
	ror	ecx,11
	mov	esi,DWORD PTR [ebp]
	xor	ecx,eax
	mov	edx,DWORD PTR 20[esp]
	xor	eax,edi
	ror	ecx,2
	add	ebx,esi
	mov	DWORD PTR [esp],eax
	add	edx,ebx
	and	eax,DWORD PTR 4[esp]
	add	ebx,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 156[esp]
	add	ebp,4
	add	eax,ebx
	cmp	esi,3329325298
	jne	$L00816_63
	mov	esi,DWORD PTR 356[esp]
	mov	ebx,DWORD PTR 8[esp]
	mov	ecx,DWORD PTR 16[esp]
	add	eax,DWORD PTR [esi]
	add	ebx,DWORD PTR 4[esi]
	add	edi,DWORD PTR 8[esi]
	add	ecx,DWORD PTR 12[esi]
	mov	DWORD PTR [esi],eax
	mov	DWORD PTR 4[esi],ebx
	mov	DWORD PTR 8[esi],edi
	mov	DWORD PTR 12[esi],ecx
	mov	eax,DWORD PTR 24[esp]
	mov	ebx,DWORD PTR 28[esp]
	mov	ecx,DWORD PTR 32[esp]
	mov	edi,DWORD PTR 360[esp]
	add	edx,DWORD PTR 16[esi]
	add	eax,DWORD PTR 20[esi]
	add	ebx,DWORD PTR 24[esi]
	add	ecx,DWORD PTR 28[esi]
	mov	DWORD PTR 16[esi],edx
	mov	DWORD PTR 20[esi],eax
	mov	DWORD PTR 24[esi],ebx
	mov	DWORD PTR 28[esi],ecx
	lea	esp,DWORD PTR 356[esp]
	sub	ebp,256
	cmp	edi,DWORD PTR 8[esp]
	jb	$L002loop
	mov	esp,DWORD PTR 12[esp]
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
ALIGN	64
$L001K256:
DD	1116352408,1899447441,3049323471,3921009573
DD	961987163,1508970993,2453635748,2870763221
DD	3624381080,310598401,607225278,1426881987
DD	1925078388,2162078206,2614888103,3248222580
DD	3835390401,4022224774,264347078,604807628
DD	770255983,1249150122,1555081692,1996064986
DD	2554220882,2821834349,2952996808,3210313671
DD	3336571891,3584528711,113926993,338241895
DD	666307205,773529912,1294757372,1396182291
DD	1695183700,1986661051,2177026350,2456956037
DD	2730485921,2820302411,3259730800,3345764771
DD	3516065817,3600352804,4094571909,275423344
DD	430227734,506948616,659060556,883997877
DD	958139571,1322822218,1537002063,1747873779
DD	1955562222,2024104815,2227730452,2361852424
DD	2428436474,2756734187,3204031479,3329325298
DD	66051,67438087,134810123,202182159
DB	83,72,65,50,53,54,32,98,108,111,99,107,32,116,114,97
DB	110,115,102,111,114,109,32,102,111,114,32,120,56,54,44,32
DB	67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97
DB	112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103
DB	62,0
ALIGN	16
$L006unrolled:
	lea	esp,DWORD PTR [esp-96]
	mov	eax,DWORD PTR [esi]
	mov	ebp,DWORD PTR 4[esi]
	mov	ecx,DWORD PTR 8[esi]
	mov	ebx,DWORD PTR 12[esi]
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,ecx
	mov	DWORD PTR 8[esp],ecx
	mov	DWORD PTR 12[esp],ebx
	mov	edx,DWORD PTR 16[esi]
	mov	ebx,DWORD PTR 20[esi]
	mov	ecx,DWORD PTR 24[esi]
	mov	esi,DWORD PTR 28[esi]
	mov	DWORD PTR 20[esp],ebx
	mov	DWORD PTR 24[esp],ecx
	mov	DWORD PTR 28[esp],esi
	jmp	$L009grand_loop
ALIGN	16
$L009grand_loop:
	mov	ebx,DWORD PTR [edi]
	mov	ecx,DWORD PTR 4[edi]
	bswap	ebx
	mov	esi,DWORD PTR 8[edi]
	bswap	ecx
	mov	DWORD PTR 32[esp],ebx
	bswap	esi
	mov	DWORD PTR 36[esp],ecx
	mov	DWORD PTR 40[esp],esi
	mov	ebx,DWORD PTR 12[edi]
	mov	ecx,DWORD PTR 16[edi]
	bswap	ebx
	mov	esi,DWORD PTR 20[edi]
	bswap	ecx
	mov	DWORD PTR 44[esp],ebx
	bswap	esi
	mov	DWORD PTR 48[esp],ecx
	mov	DWORD PTR 52[esp],esi
	mov	ebx,DWORD PTR 24[edi]
	mov	ecx,DWORD PTR 28[edi]
	bswap	ebx
	mov	esi,DWORD PTR 32[edi]
	bswap	ecx
	mov	DWORD PTR 56[esp],ebx
	bswap	esi
	mov	DWORD PTR 60[esp],ecx
	mov	DWORD PTR 64[esp],esi
	mov	ebx,DWORD PTR 36[edi]
	mov	ecx,DWORD PTR 40[edi]
	bswap	ebx
	mov	esi,DWORD PTR 44[edi]
	bswap	ecx
	mov	DWORD PTR 68[esp],ebx
	bswap	esi
	mov	DWORD PTR 72[esp],ecx
	mov	DWORD PTR 76[esp],esi
	mov	ebx,DWORD PTR 48[edi]
	mov	ecx,DWORD PTR 52[edi]
	bswap	ebx
	mov	esi,DWORD PTR 56[edi]
	bswap	ecx
	mov	DWORD PTR 80[esp],ebx
	bswap	esi
	mov	DWORD PTR 84[esp],ecx
	mov	DWORD PTR 88[esp],esi
	mov	ebx,DWORD PTR 60[edi]
	add	edi,64
	bswap	ebx
	mov	DWORD PTR 100[esp],edi
	mov	DWORD PTR 92[esp],ebx
	mov	ecx,edx
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 32[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1116352408[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 36[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1899447441[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 40[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3049323471[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 44[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3921009573[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 48[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 961987163[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 52[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1508970993[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 56[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2453635748[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 60[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2870763221[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 64[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3624381080[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 68[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 310598401[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 72[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 607225278[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 76[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1426881987[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 80[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1925078388[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 84[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2162078206[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	ecx,edx
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	ebx,DWORD PTR 88[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2614888103[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	esi,edx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	ebx,DWORD PTR 92[esp]
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3248222580[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 36[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 88[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 32[esp]
	shr	edi,10
	add	ebx,DWORD PTR 68[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 32[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3835390401[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 40[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 92[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 36[esp]
	shr	edi,10
	add	ebx,DWORD PTR 72[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 36[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 4022224774[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 44[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 32[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 40[esp]
	shr	edi,10
	add	ebx,DWORD PTR 76[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 40[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 264347078[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 48[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 36[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 44[esp]
	shr	edi,10
	add	ebx,DWORD PTR 80[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 44[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 604807628[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 52[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 40[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 48[esp]
	shr	edi,10
	add	ebx,DWORD PTR 84[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 48[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 770255983[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 56[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 44[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 52[esp]
	shr	edi,10
	add	ebx,DWORD PTR 88[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 52[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1249150122[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 60[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 48[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 56[esp]
	shr	edi,10
	add	ebx,DWORD PTR 92[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	DWORD PTR 56[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1555081692[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 64[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 52[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 60[esp]
	shr	edi,10
	add	ebx,DWORD PTR 32[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	DWORD PTR 60[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1996064986[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 68[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 56[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 64[esp]
	shr	edi,10
	add	ebx,DWORD PTR 36[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 64[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2554220882[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 72[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 60[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 68[esp]
	shr	edi,10
	add	ebx,DWORD PTR 40[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 68[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2821834349[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 76[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 64[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 72[esp]
	shr	edi,10
	add	ebx,DWORD PTR 44[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 72[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2952996808[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 80[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 68[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 76[esp]
	shr	edi,10
	add	ebx,DWORD PTR 48[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 76[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3210313671[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 84[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 72[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 80[esp]
	shr	edi,10
	add	ebx,DWORD PTR 52[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 80[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3336571891[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 88[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 76[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 84[esp]
	shr	edi,10
	add	ebx,DWORD PTR 56[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 84[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3584528711[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 92[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 80[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 88[esp]
	shr	edi,10
	add	ebx,DWORD PTR 60[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	DWORD PTR 88[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 113926993[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 32[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 84[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 92[esp]
	shr	edi,10
	add	ebx,DWORD PTR 64[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	DWORD PTR 92[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 338241895[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 36[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 88[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 32[esp]
	shr	edi,10
	add	ebx,DWORD PTR 68[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 32[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 666307205[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 40[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 92[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 36[esp]
	shr	edi,10
	add	ebx,DWORD PTR 72[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 36[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 773529912[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 44[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 32[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 40[esp]
	shr	edi,10
	add	ebx,DWORD PTR 76[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 40[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1294757372[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 48[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 36[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 44[esp]
	shr	edi,10
	add	ebx,DWORD PTR 80[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 44[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1396182291[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 52[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 40[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 48[esp]
	shr	edi,10
	add	ebx,DWORD PTR 84[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 48[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1695183700[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 56[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 44[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 52[esp]
	shr	edi,10
	add	ebx,DWORD PTR 88[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 52[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1986661051[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 60[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 48[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 56[esp]
	shr	edi,10
	add	ebx,DWORD PTR 92[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	DWORD PTR 56[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2177026350[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 64[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 52[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 60[esp]
	shr	edi,10
	add	ebx,DWORD PTR 32[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	DWORD PTR 60[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2456956037[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 68[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 56[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 64[esp]
	shr	edi,10
	add	ebx,DWORD PTR 36[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 64[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2730485921[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 72[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 60[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 68[esp]
	shr	edi,10
	add	ebx,DWORD PTR 40[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 68[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2820302411[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 76[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 64[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 72[esp]
	shr	edi,10
	add	ebx,DWORD PTR 44[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 72[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3259730800[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 80[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 68[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 76[esp]
	shr	edi,10
	add	ebx,DWORD PTR 48[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 76[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3345764771[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 84[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 72[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 80[esp]
	shr	edi,10
	add	ebx,DWORD PTR 52[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 80[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3516065817[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 88[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 76[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 84[esp]
	shr	edi,10
	add	ebx,DWORD PTR 56[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 84[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3600352804[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 92[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 80[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 88[esp]
	shr	edi,10
	add	ebx,DWORD PTR 60[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	DWORD PTR 88[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 4094571909[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 32[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 84[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 92[esp]
	shr	edi,10
	add	ebx,DWORD PTR 64[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	DWORD PTR 92[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 275423344[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 36[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 88[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 32[esp]
	shr	edi,10
	add	ebx,DWORD PTR 68[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 32[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 430227734[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 40[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 92[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 36[esp]
	shr	edi,10
	add	ebx,DWORD PTR 72[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 36[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 506948616[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 44[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 32[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 40[esp]
	shr	edi,10
	add	ebx,DWORD PTR 76[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 40[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 659060556[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 48[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 36[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 44[esp]
	shr	edi,10
	add	ebx,DWORD PTR 80[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 44[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 883997877[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 52[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 40[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 48[esp]
	shr	edi,10
	add	ebx,DWORD PTR 84[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 48[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 958139571[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 56[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 44[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 52[esp]
	shr	edi,10
	add	ebx,DWORD PTR 88[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 52[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1322822218[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 60[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 48[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 56[esp]
	shr	edi,10
	add	ebx,DWORD PTR 92[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	mov	DWORD PTR 56[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1537002063[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 64[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 52[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 60[esp]
	shr	edi,10
	add	ebx,DWORD PTR 32[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	mov	DWORD PTR 60[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 1747873779[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 68[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 56[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 64[esp]
	shr	edi,10
	add	ebx,DWORD PTR 36[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 20[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	DWORD PTR 64[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 28[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 4[esp]
	xor	ecx,eax
	mov	DWORD PTR [esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 1955562222[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 72[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 12[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 60[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 68[esp]
	shr	edi,10
	add	ebx,DWORD PTR 40[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 16[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 20[esp]
	xor	edx,esi
	mov	DWORD PTR 68[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 12[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 24[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR [esp]
	xor	esi,ebp
	mov	DWORD PTR 28[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2024104815[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 76[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,esi
	mov	esi,DWORD PTR 64[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 72[esp]
	shr	edi,10
	add	ebx,DWORD PTR 44[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 12[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	DWORD PTR 72[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 20[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 28[esp]
	xor	ecx,eax
	mov	DWORD PTR 24[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2227730452[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 80[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 4[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 68[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 76[esp]
	shr	edi,10
	add	ebx,DWORD PTR 48[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 8[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 12[esp]
	xor	edx,esi
	mov	DWORD PTR 76[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 4[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 16[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 24[esp]
	xor	esi,ebp
	mov	DWORD PTR 20[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2361852424[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 84[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,esi
	mov	esi,DWORD PTR 72[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 80[esp]
	shr	edi,10
	add	ebx,DWORD PTR 52[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 4[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	DWORD PTR 80[esp],ebx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 12[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 20[esp]
	xor	ecx,eax
	mov	DWORD PTR 16[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 2428436474[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 88[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 28[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 76[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 84[esp]
	shr	edi,10
	add	ebx,DWORD PTR 56[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR [esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 4[esp]
	xor	edx,esi
	mov	DWORD PTR 84[esp],ebx
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 28[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR 8[esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 16[esp]
	xor	esi,ebp
	mov	DWORD PTR 12[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 2756734187[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	mov	ecx,DWORD PTR 92[esp]
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,esi
	mov	esi,DWORD PTR 80[esp]
	mov	ebx,ecx
	ror	ecx,11
	mov	edi,esi
	ror	esi,2
	xor	ecx,ebx
	shr	ebx,3
	ror	ecx,7
	xor	esi,edi
	xor	ebx,ecx
	ror	esi,17
	add	ebx,DWORD PTR 88[esp]
	shr	edi,10
	add	ebx,DWORD PTR 60[esp]
	mov	ecx,edx
	xor	edi,esi
	mov	esi,DWORD PTR 28[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR [esp]
	xor	edx,ecx
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	add	ebx,DWORD PTR 4[esp]
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	ebx,edi
	ror	ecx,9
	mov	esi,eax
	mov	edi,DWORD PTR 12[esp]
	xor	ecx,eax
	mov	DWORD PTR 8[esp],eax
	xor	eax,edi
	ror	ecx,11
	and	ebp,eax
	lea	edx,DWORD PTR 3204031479[edx*1+ebx]
	xor	ecx,esi
	xor	ebp,edi
	mov	esi,DWORD PTR 32[esp]
	ror	ecx,2
	add	ebp,edx
	add	edx,DWORD PTR 20[esp]
	add	ebp,ecx
	mov	ecx,DWORD PTR 84[esp]
	mov	ebx,esi
	ror	esi,11
	mov	edi,ecx
	ror	ecx,2
	xor	esi,ebx
	shr	ebx,3
	ror	esi,7
	xor	ecx,edi
	xor	ebx,esi
	ror	ecx,17
	add	ebx,DWORD PTR 92[esp]
	shr	edi,10
	add	ebx,DWORD PTR 64[esp]
	mov	esi,edx
	xor	edi,ecx
	mov	ecx,DWORD PTR 24[esp]
	ror	edx,14
	add	ebx,edi
	mov	edi,DWORD PTR 28[esp]
	xor	edx,esi
	xor	ecx,edi
	ror	edx,5
	and	ecx,esi
	mov	DWORD PTR 20[esp],esi
	xor	edx,esi
	add	ebx,DWORD PTR [esp]
	xor	edi,ecx
	ror	edx,6
	mov	esi,ebp
	add	ebx,edi
	ror	esi,9
	mov	ecx,ebp
	mov	edi,DWORD PTR 8[esp]
	xor	esi,ebp
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	ror	esi,11
	and	eax,ebp
	lea	edx,DWORD PTR 3329325298[edx*1+ebx]
	xor	esi,ecx
	xor	eax,edi
	ror	esi,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,esi
	mov	esi,DWORD PTR 96[esp]
	xor	ebp,edi
	mov	ecx,DWORD PTR 12[esp]
	add	eax,DWORD PTR [esi]
	add	ebp,DWORD PTR 4[esi]
	add	edi,DWORD PTR 8[esi]
	add	ecx,DWORD PTR 12[esi]
	mov	DWORD PTR [esi],eax
	mov	DWORD PTR 4[esi],ebp
	mov	DWORD PTR 8[esi],edi
	mov	DWORD PTR 12[esi],ecx
	mov	DWORD PTR 4[esp],ebp
	xor	ebp,edi
	mov	DWORD PTR 8[esp],edi
	mov	DWORD PTR 12[esp],ecx
	mov	edi,DWORD PTR 20[esp]
	mov	ebx,DWORD PTR 24[esp]
	mov	ecx,DWORD PTR 28[esp]
	add	edx,DWORD PTR 16[esi]
	add	edi,DWORD PTR 20[esi]
	add	ebx,DWORD PTR 24[esi]
	add	ecx,DWORD PTR 28[esi]
	mov	DWORD PTR 16[esi],edx
	mov	DWORD PTR 20[esi],edi
	mov	DWORD PTR 24[esi],ebx
	mov	DWORD PTR 28[esi],ecx
	mov	DWORD PTR 20[esp],edi
	mov	edi,DWORD PTR 100[esp]
	mov	DWORD PTR 24[esp],ebx
	mov	DWORD PTR 28[esp],ecx
	cmp	edi,DWORD PTR 104[esp]
	jb	$L009grand_loop
	mov	esp,DWORD PTR 108[esp]
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
ALIGN	32
$L004shaext:
	sub	esp,32
	movdqu	xmm1,XMMWORD PTR [esi]
	lea	ebp,DWORD PTR 128[ebp]
	movdqu	xmm2,XMMWORD PTR 16[esi]
	movdqa	xmm7,XMMWORD PTR 128[ebp]
	pshufd	xmm0,xmm1,27
	pshufd	xmm1,xmm1,177
	pshufd	xmm2,xmm2,27
DB	102,15,58,15,202,8
	punpcklqdq	xmm2,xmm0
	jmp	$L010loop_shaext
ALIGN	16
$L010loop_shaext:
	movdqu	xmm3,XMMWORD PTR [edi]
	movdqu	xmm4,XMMWORD PTR 16[edi]
	movdqu	xmm5,XMMWORD PTR 32[edi]
DB	102,15,56,0,223
	movdqu	xmm6,XMMWORD PTR 48[edi]
	movdqa	XMMWORD PTR 16[esp],xmm2
	movdqa	xmm0,XMMWORD PTR [ebp-128]
	paddd	xmm0,xmm3
DB	102,15,56,0,231
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	nop
	movdqa	XMMWORD PTR [esp],xmm1
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-112]
	paddd	xmm0,xmm4
DB	102,15,56,0,239
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	lea	edi,DWORD PTR 64[edi]
DB	15,56,204,220
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-96]
	paddd	xmm0,xmm5
DB	102,15,56,0,247
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm6
DB	102,15,58,15,253,4
	nop
	paddd	xmm3,xmm7
DB	15,56,204,229
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-80]
	paddd	xmm0,xmm6
DB	15,56,205,222
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm3
DB	102,15,58,15,254,4
	nop
	paddd	xmm4,xmm7
DB	15,56,204,238
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-64]
	paddd	xmm0,xmm3
DB	15,56,205,227
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm4
DB	102,15,58,15,251,4
	nop
	paddd	xmm5,xmm7
DB	15,56,204,243
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-48]
	paddd	xmm0,xmm4
DB	15,56,205,236
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm5
DB	102,15,58,15,252,4
	nop
	paddd	xmm6,xmm7
DB	15,56,204,220
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-32]
	paddd	xmm0,xmm5
DB	15,56,205,245
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm6
DB	102,15,58,15,253,4
	nop
	paddd	xmm3,xmm7
DB	15,56,204,229
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp-16]
	paddd	xmm0,xmm6
DB	15,56,205,222
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm3
DB	102,15,58,15,254,4
	nop
	paddd	xmm4,xmm7
DB	15,56,204,238
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR [ebp]
	paddd	xmm0,xmm3
DB	15,56,205,227
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm4
DB	102,15,58,15,251,4
	nop
	paddd	xmm5,xmm7
DB	15,56,204,243
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 16[ebp]
	paddd	xmm0,xmm4
DB	15,56,205,236
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm5
DB	102,15,58,15,252,4
	nop
	paddd	xmm6,xmm7
DB	15,56,204,220
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 32[ebp]
	paddd	xmm0,xmm5
DB	15,56,205,245
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm6
DB	102,15,58,15,253,4
	nop
	paddd	xmm3,xmm7
DB	15,56,204,229
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 48[ebp]
	paddd	xmm0,xmm6
DB	15,56,205,222
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm3
DB	102,15,58,15,254,4
	nop
	paddd	xmm4,xmm7
DB	15,56,204,238
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 64[ebp]
	paddd	xmm0,xmm3
DB	15,56,205,227
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm4
DB	102,15,58,15,251,4
	nop
	paddd	xmm5,xmm7
DB	15,56,204,243
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 80[ebp]
	paddd	xmm0,xmm4
DB	15,56,205,236
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	movdqa	xmm7,xmm5
DB	102,15,58,15,252,4
DB	15,56,203,202
	paddd	xmm6,xmm7
	movdqa	xmm0,XMMWORD PTR 96[ebp]
	paddd	xmm0,xmm5
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
DB	15,56,205,245
	movdqa	xmm7,XMMWORD PTR 128[ebp]
DB	15,56,203,202
	movdqa	xmm0,XMMWORD PTR 112[ebp]
	paddd	xmm0,xmm6
	nop
DB	15,56,203,209
	pshufd	xmm0,xmm0,14
	cmp	eax,edi
	nop
DB	15,56,203,202
	paddd	xmm2,XMMWORD PTR 16[esp]
	paddd	xmm1,XMMWORD PTR [esp]
	jnz	$L010loop_shaext
	pshufd	xmm2,xmm2,177
	pshufd	xmm7,xmm1,27
	pshufd	xmm1,xmm1,177
	punpckhqdq	xmm1,xmm2
DB	102,15,58,15,215,8
	mov	esp,DWORD PTR 44[esp]
	movdqu	XMMWORD PTR [esi],xmm1
	movdqu	XMMWORD PTR 16[esi],xmm2
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
ALIGN	32
$L005SSSE3:
	lea	esp,DWORD PTR [esp-96]
	mov	eax,DWORD PTR [esi]
	mov	ebx,DWORD PTR 4[esi]
	mov	ecx,DWORD PTR 8[esi]
	mov	edi,DWORD PTR 12[esi]
	mov	DWORD PTR 4[esp],ebx
	xor	ebx,ecx
	mov	DWORD PTR 8[esp],ecx
	mov	DWORD PTR 12[esp],edi
	mov	edx,DWORD PTR 16[esi]
	mov	edi,DWORD PTR 20[esi]
	mov	ecx,DWORD PTR 24[esi]
	mov	esi,DWORD PTR 28[esi]
	mov	DWORD PTR 20[esp],edi
	mov	edi,DWORD PTR 100[esp]
	mov	DWORD PTR 24[esp],ecx
	mov	DWORD PTR 28[esp],esi
	movdqa	xmm7,XMMWORD PTR 256[ebp]
	jmp	$L011grand_ssse3
ALIGN	16
$L011grand_ssse3:
	movdqu	xmm0,XMMWORD PTR [edi]
	movdqu	xmm1,XMMWORD PTR 16[edi]
	movdqu	xmm2,XMMWORD PTR 32[edi]
	movdqu	xmm3,XMMWORD PTR 48[edi]
	add	edi,64
DB	102,15,56,0,199
	mov	DWORD PTR 100[esp],edi
DB	102,15,56,0,207
	movdqa	xmm4,XMMWORD PTR [ebp]
DB	102,15,56,0,215
	movdqa	xmm5,XMMWORD PTR 16[ebp]
	paddd	xmm4,xmm0
DB	102,15,56,0,223
	movdqa	xmm6,XMMWORD PTR 32[ebp]
	paddd	xmm5,xmm1
	movdqa	xmm7,XMMWORD PTR 48[ebp]
	movdqa	XMMWORD PTR 32[esp],xmm4
	paddd	xmm6,xmm2
	movdqa	XMMWORD PTR 48[esp],xmm5
	paddd	xmm7,xmm3
	movdqa	XMMWORD PTR 64[esp],xmm6
	movdqa	XMMWORD PTR 80[esp],xmm7
	jmp	$L012ssse3_00_47
ALIGN	16
$L012ssse3_00_47:
	add	ebp,64
	mov	ecx,edx
	movdqa	xmm4,xmm1
	ror	edx,14
	mov	esi,DWORD PTR 20[esp]
	movdqa	xmm7,xmm3
	xor	edx,ecx
	mov	edi,DWORD PTR 24[esp]
DB	102,15,58,15,224,4
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
DB	102,15,58,15,250,4
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	movdqa	xmm5,xmm4
	ror	edx,6
	mov	ecx,eax
	movdqa	xmm6,xmm4
	add	edx,edi
	mov	edi,DWORD PTR 4[esp]
	psrld	xmm4,3
	mov	esi,eax
	ror	ecx,9
	paddd	xmm0,xmm7
	mov	DWORD PTR [esp],eax
	xor	ecx,eax
	psrld	xmm6,7
	xor	eax,edi
	add	edx,DWORD PTR 28[esp]
	ror	ecx,11
	and	ebx,eax
	pshufd	xmm7,xmm3,250
	xor	ecx,esi
	add	edx,DWORD PTR 32[esp]
	pslld	xmm5,14
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm4,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 12[esp]
	psrld	xmm6,11
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm4,xmm5
	mov	esi,DWORD PTR 16[esp]
	xor	edx,ecx
	pslld	xmm5,11
	mov	edi,DWORD PTR 20[esp]
	xor	esi,edi
	ror	edx,5
	pxor	xmm4,xmm6
	and	esi,ecx
	mov	DWORD PTR 12[esp],ecx
	movdqa	xmm6,xmm7
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	pxor	xmm4,xmm5
	mov	ecx,ebx
	add	edx,edi
	psrld	xmm7,10
	mov	edi,DWORD PTR [esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm0,xmm4
	mov	DWORD PTR 28[esp],ebx
	xor	ecx,ebx
	psrlq	xmm6,17
	xor	ebx,edi
	add	edx,DWORD PTR 24[esp]
	ror	ecx,11
	pxor	xmm7,xmm6
	and	eax,ebx
	xor	ecx,esi
	psrlq	xmm6,2
	add	edx,DWORD PTR 36[esp]
	xor	eax,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	pshufd	xmm7,xmm7,128
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 12[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 16[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	psrldq	xmm7,8
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	paddd	xmm0,xmm7
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 28[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 24[esp],eax
	pshufd	xmm7,xmm0,80
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 20[esp]
	movdqa	xmm6,xmm7
	ror	ecx,11
	psrld	xmm7,10
	and	ebx,eax
	psrlq	xmm6,17
	xor	ecx,esi
	add	edx,DWORD PTR 40[esp]
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 4[esp]
	psrlq	xmm6,2
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm7,xmm6
	mov	esi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 12[esp]
	pshufd	xmm7,xmm7,8
	xor	esi,edi
	ror	edx,5
	movdqa	xmm6,XMMWORD PTR [ebp]
	and	esi,ecx
	mov	DWORD PTR 4[esp],ecx
	pslldq	xmm7,8
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 24[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm0,xmm7
	mov	DWORD PTR 20[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 16[esp]
	paddd	xmm6,xmm0
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 44[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,ecx
	movdqa	XMMWORD PTR 32[esp],xmm6
	mov	ecx,edx
	movdqa	xmm4,xmm2
	ror	edx,14
	mov	esi,DWORD PTR 4[esp]
	movdqa	xmm7,xmm0
	xor	edx,ecx
	mov	edi,DWORD PTR 8[esp]
DB	102,15,58,15,225,4
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
DB	102,15,58,15,251,4
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	xor	edi,esi
	movdqa	xmm5,xmm4
	ror	edx,6
	mov	ecx,eax
	movdqa	xmm6,xmm4
	add	edx,edi
	mov	edi,DWORD PTR 20[esp]
	psrld	xmm4,3
	mov	esi,eax
	ror	ecx,9
	paddd	xmm1,xmm7
	mov	DWORD PTR 16[esp],eax
	xor	ecx,eax
	psrld	xmm6,7
	xor	eax,edi
	add	edx,DWORD PTR 12[esp]
	ror	ecx,11
	and	ebx,eax
	pshufd	xmm7,xmm0,250
	xor	ecx,esi
	add	edx,DWORD PTR 48[esp]
	pslld	xmm5,14
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm4,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 28[esp]
	psrld	xmm6,11
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm4,xmm5
	mov	esi,DWORD PTR [esp]
	xor	edx,ecx
	pslld	xmm5,11
	mov	edi,DWORD PTR 4[esp]
	xor	esi,edi
	ror	edx,5
	pxor	xmm4,xmm6
	and	esi,ecx
	mov	DWORD PTR 28[esp],ecx
	movdqa	xmm6,xmm7
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	pxor	xmm4,xmm5
	mov	ecx,ebx
	add	edx,edi
	psrld	xmm7,10
	mov	edi,DWORD PTR 16[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm1,xmm4
	mov	DWORD PTR 12[esp],ebx
	xor	ecx,ebx
	psrlq	xmm6,17
	xor	ebx,edi
	add	edx,DWORD PTR 8[esp]
	ror	ecx,11
	pxor	xmm7,xmm6
	and	eax,ebx
	xor	ecx,esi
	psrlq	xmm6,2
	add	edx,DWORD PTR 52[esp]
	xor	eax,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	pshufd	xmm7,xmm7,128
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 28[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR [esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	psrldq	xmm7,8
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	paddd	xmm1,xmm7
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 12[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 8[esp],eax
	pshufd	xmm7,xmm1,80
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 4[esp]
	movdqa	xmm6,xmm7
	ror	ecx,11
	psrld	xmm7,10
	and	ebx,eax
	psrlq	xmm6,17
	xor	ecx,esi
	add	edx,DWORD PTR 56[esp]
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 20[esp]
	psrlq	xmm6,2
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm7,xmm6
	mov	esi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 28[esp]
	pshufd	xmm7,xmm7,8
	xor	esi,edi
	ror	edx,5
	movdqa	xmm6,XMMWORD PTR 16[ebp]
	and	esi,ecx
	mov	DWORD PTR 20[esp],ecx
	pslldq	xmm7,8
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 8[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm1,xmm7
	mov	DWORD PTR 4[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR [esp]
	paddd	xmm6,xmm1
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 60[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,ecx
	movdqa	XMMWORD PTR 48[esp],xmm6
	mov	ecx,edx
	movdqa	xmm4,xmm3
	ror	edx,14
	mov	esi,DWORD PTR 20[esp]
	movdqa	xmm7,xmm1
	xor	edx,ecx
	mov	edi,DWORD PTR 24[esp]
DB	102,15,58,15,226,4
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
DB	102,15,58,15,248,4
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	movdqa	xmm5,xmm4
	ror	edx,6
	mov	ecx,eax
	movdqa	xmm6,xmm4
	add	edx,edi
	mov	edi,DWORD PTR 4[esp]
	psrld	xmm4,3
	mov	esi,eax
	ror	ecx,9
	paddd	xmm2,xmm7
	mov	DWORD PTR [esp],eax
	xor	ecx,eax
	psrld	xmm6,7
	xor	eax,edi
	add	edx,DWORD PTR 28[esp]
	ror	ecx,11
	and	ebx,eax
	pshufd	xmm7,xmm1,250
	xor	ecx,esi
	add	edx,DWORD PTR 64[esp]
	pslld	xmm5,14
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm4,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 12[esp]
	psrld	xmm6,11
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm4,xmm5
	mov	esi,DWORD PTR 16[esp]
	xor	edx,ecx
	pslld	xmm5,11
	mov	edi,DWORD PTR 20[esp]
	xor	esi,edi
	ror	edx,5
	pxor	xmm4,xmm6
	and	esi,ecx
	mov	DWORD PTR 12[esp],ecx
	movdqa	xmm6,xmm7
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	pxor	xmm4,xmm5
	mov	ecx,ebx
	add	edx,edi
	psrld	xmm7,10
	mov	edi,DWORD PTR [esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm2,xmm4
	mov	DWORD PTR 28[esp],ebx
	xor	ecx,ebx
	psrlq	xmm6,17
	xor	ebx,edi
	add	edx,DWORD PTR 24[esp]
	ror	ecx,11
	pxor	xmm7,xmm6
	and	eax,ebx
	xor	ecx,esi
	psrlq	xmm6,2
	add	edx,DWORD PTR 68[esp]
	xor	eax,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	pshufd	xmm7,xmm7,128
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 12[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 16[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	psrldq	xmm7,8
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	paddd	xmm2,xmm7
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 28[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 24[esp],eax
	pshufd	xmm7,xmm2,80
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 20[esp]
	movdqa	xmm6,xmm7
	ror	ecx,11
	psrld	xmm7,10
	and	ebx,eax
	psrlq	xmm6,17
	xor	ecx,esi
	add	edx,DWORD PTR 72[esp]
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 4[esp]
	psrlq	xmm6,2
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm7,xmm6
	mov	esi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 12[esp]
	pshufd	xmm7,xmm7,8
	xor	esi,edi
	ror	edx,5
	movdqa	xmm6,XMMWORD PTR 32[ebp]
	and	esi,ecx
	mov	DWORD PTR 4[esp],ecx
	pslldq	xmm7,8
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 24[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm2,xmm7
	mov	DWORD PTR 20[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 16[esp]
	paddd	xmm6,xmm2
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 76[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,ecx
	movdqa	XMMWORD PTR 64[esp],xmm6
	mov	ecx,edx
	movdqa	xmm4,xmm0
	ror	edx,14
	mov	esi,DWORD PTR 4[esp]
	movdqa	xmm7,xmm2
	xor	edx,ecx
	mov	edi,DWORD PTR 8[esp]
DB	102,15,58,15,227,4
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
DB	102,15,58,15,249,4
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	xor	edi,esi
	movdqa	xmm5,xmm4
	ror	edx,6
	mov	ecx,eax
	movdqa	xmm6,xmm4
	add	edx,edi
	mov	edi,DWORD PTR 20[esp]
	psrld	xmm4,3
	mov	esi,eax
	ror	ecx,9
	paddd	xmm3,xmm7
	mov	DWORD PTR 16[esp],eax
	xor	ecx,eax
	psrld	xmm6,7
	xor	eax,edi
	add	edx,DWORD PTR 12[esp]
	ror	ecx,11
	and	ebx,eax
	pshufd	xmm7,xmm2,250
	xor	ecx,esi
	add	edx,DWORD PTR 80[esp]
	pslld	xmm5,14
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm4,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 28[esp]
	psrld	xmm6,11
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm4,xmm5
	mov	esi,DWORD PTR [esp]
	xor	edx,ecx
	pslld	xmm5,11
	mov	edi,DWORD PTR 4[esp]
	xor	esi,edi
	ror	edx,5
	pxor	xmm4,xmm6
	and	esi,ecx
	mov	DWORD PTR 28[esp],ecx
	movdqa	xmm6,xmm7
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	pxor	xmm4,xmm5
	mov	ecx,ebx
	add	edx,edi
	psrld	xmm7,10
	mov	edi,DWORD PTR 16[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm3,xmm4
	mov	DWORD PTR 12[esp],ebx
	xor	ecx,ebx
	psrlq	xmm6,17
	xor	ebx,edi
	add	edx,DWORD PTR 8[esp]
	ror	ecx,11
	pxor	xmm7,xmm6
	and	eax,ebx
	xor	ecx,esi
	psrlq	xmm6,2
	add	edx,DWORD PTR 84[esp]
	xor	eax,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	pshufd	xmm7,xmm7,128
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 28[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR [esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	psrldq	xmm7,8
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	paddd	xmm3,xmm7
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 12[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 8[esp],eax
	pshufd	xmm7,xmm3,80
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 4[esp]
	movdqa	xmm6,xmm7
	ror	ecx,11
	psrld	xmm7,10
	and	ebx,eax
	psrlq	xmm6,17
	xor	ecx,esi
	add	edx,DWORD PTR 88[esp]
	xor	ebx,edi
	ror	ecx,2
	pxor	xmm7,xmm6
	add	ebx,edx
	add	edx,DWORD PTR 20[esp]
	psrlq	xmm6,2
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	pxor	xmm7,xmm6
	mov	esi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 28[esp]
	pshufd	xmm7,xmm7,8
	xor	esi,edi
	ror	edx,5
	movdqa	xmm6,XMMWORD PTR 48[ebp]
	and	esi,ecx
	mov	DWORD PTR 20[esp],ecx
	pslldq	xmm7,8
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 8[esp]
	mov	esi,ebx
	ror	ecx,9
	paddd	xmm3,xmm7
	mov	DWORD PTR 4[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR [esp]
	paddd	xmm6,xmm3
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 92[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,ecx
	movdqa	XMMWORD PTR 80[esp],xmm6
	cmp	DWORD PTR 64[ebp],66051
	jne	$L012ssse3_00_47
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 20[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 24[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 4[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR [esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 28[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 32[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 12[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 20[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 12[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR [esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 28[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 24[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 36[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 12[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 16[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 28[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 24[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 20[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 40[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 4[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 12[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 4[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 24[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 20[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 16[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 44[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 4[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 8[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 20[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 16[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 12[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 48[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 28[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR [esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 4[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 28[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 16[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 12[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 8[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 52[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 28[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR [esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 12[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 8[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 4[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 56[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 20[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 28[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 20[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 8[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 4[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR [esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 60[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 20[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 24[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 16[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 4[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR [esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 28[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 64[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 12[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 16[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 20[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 12[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR [esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 28[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 24[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 68[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 8[esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 12[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 16[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 8[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 28[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 24[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 20[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 72[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 4[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 8[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 12[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 4[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 24[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 20[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 16[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 76[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR [esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 4[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 8[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR [esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 20[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 16[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 12[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 80[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 28[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR [esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 4[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 28[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 16[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 12[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR 8[esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 84[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 24[esp]
	add	eax,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 28[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR [esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 24[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,eax
	add	edx,edi
	mov	edi,DWORD PTR 12[esp]
	mov	esi,eax
	ror	ecx,9
	mov	DWORD PTR 8[esp],eax
	xor	ecx,eax
	xor	eax,edi
	add	edx,DWORD PTR 4[esp]
	ror	ecx,11
	and	ebx,eax
	xor	ecx,esi
	add	edx,DWORD PTR 88[esp]
	xor	ebx,edi
	ror	ecx,2
	add	ebx,edx
	add	edx,DWORD PTR 20[esp]
	add	ebx,ecx
	mov	ecx,edx
	ror	edx,14
	mov	esi,DWORD PTR 24[esp]
	xor	edx,ecx
	mov	edi,DWORD PTR 28[esp]
	xor	esi,edi
	ror	edx,5
	and	esi,ecx
	mov	DWORD PTR 20[esp],ecx
	xor	edx,ecx
	xor	edi,esi
	ror	edx,6
	mov	ecx,ebx
	add	edx,edi
	mov	edi,DWORD PTR 8[esp]
	mov	esi,ebx
	ror	ecx,9
	mov	DWORD PTR 4[esp],ebx
	xor	ecx,ebx
	xor	ebx,edi
	add	edx,DWORD PTR [esp]
	ror	ecx,11
	and	eax,ebx
	xor	ecx,esi
	add	edx,DWORD PTR 92[esp]
	xor	eax,edi
	ror	ecx,2
	add	eax,edx
	add	edx,DWORD PTR 16[esp]
	add	eax,ecx
	mov	esi,DWORD PTR 96[esp]
	xor	ebx,edi
	mov	ecx,DWORD PTR 12[esp]
	add	eax,DWORD PTR [esi]
	add	ebx,DWORD PTR 4[esi]
	add	edi,DWORD PTR 8[esi]
	add	ecx,DWORD PTR 12[esi]
	mov	DWORD PTR [esi],eax
	mov	DWORD PTR 4[esi],ebx
	mov	DWORD PTR 8[esi],edi
	mov	DWORD PTR 12[esi],ecx
	mov	DWORD PTR 4[esp],ebx
	xor	ebx,edi
	mov	DWORD PTR 8[esp],edi
	mov	DWORD PTR 12[esp],ecx
	mov	edi,DWORD PTR 20[esp]
	mov	ecx,DWORD PTR 24[esp]
	add	edx,DWORD PTR 16[esi]
	add	edi,DWORD PTR 20[esi]
	add	ecx,DWORD PTR 24[esi]
	mov	DWORD PTR 16[esi],edx
	mov	DWORD PTR 20[esi],edi
	mov	DWORD PTR 20[esp],edi
	mov	edi,DWORD PTR 28[esp]
	mov	DWORD PTR 24[esi],ecx
	add	edi,DWORD PTR 28[esi]
	mov	DWORD PTR 24[esp],ecx
	mov	DWORD PTR 28[esi],edi
	mov	DWORD PTR 28[esp],edi
	mov	edi,DWORD PTR 100[esp]
	movdqa	xmm7,XMMWORD PTR 64[ebp]
	sub	ebp,192
	cmp	edi,DWORD PTR 104[esp]
	jb	$L011grand_ssse3
	mov	esp,DWORD PTR 108[esp]
	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
_sha256_block_data_order ENDP
.text$	ENDS
.bss	SEGMENT 'BSS'
COMM	_OPENSSL_ia32cap_P:DWORD:4
.bss	ENDS
END
