.data
matrixA: .word 1,2,3,4,5,6
matrixB: .word 5,6,7,8,9,10
sizeA: .word 3,2
sizeB: .word 2,3
result: .word 0:9
.text

# load row and column sizes of A and B
# s0=Ar, s1=Ac, s2=Br, s3=Bc
	la	$t0,sizeA
	lw	$s0,($t0)
	addiu	$t0,$t0,4
	lw	$s1,($t0)

	la	$t0,sizeB
	lw	$s2,($t0)
	addiu	$t0,$t0,4
	lw	$s3,($t0)

# load matrix addresses
# s4 = A, s5 = B, s6 = result
	la	$s4,matrixA
	la	$s5,matrixB
	la	$s6,result
# copy result address to t7
	xori	$t7,$s6,0
	li	$t0,0
CROW:	li	$t1,0
# store starting loc in A in t4
	multu	$t0,$s1
	mflo	$t4
CCOL:	
# store dot in t3
	li	$t3,0
# index is t2
	li	$t2,0
CROSS:	
# load A[t0][t2] to t5
	add	$t5,$t4,$t2
	sll	$t5,$t5,2
	addu	$t5,$t5,$s4
	lw	$t5,($t5)
# load B[t2][t1] to t6
	multu	$t2,$s3
	mflo	$t6
	addu	$t6,$t6,$t1
	sll	$t6,$t6,2
	addu	$t6,$t6,$s5
	lw	$t6,($t6)
# multiply and add
	multu	$t5,$t6
	mflo	$t5
	addu	$t3,$t3,$t5
# loop stuff
	addiu	$t2,$t2,1
	bne	$t2,$s2,CROSS
	nop
# store result in proper memory location
	sw	$t3,($t7)
	add   $v0,$zero,4007
	add   $a0,$zero,$t3
	syscall	
	addiu	$t7,$t7,4
# loop stuff
	addiu	$t1,$t1,1
	bne	$t1,$s3,CCOL
	nop
# loop stuff	
	addiu	$t0,$t0,1
	bne	$t0,$s0,CROW
	nop
# exit
	li	$v0,4001
	syscall
