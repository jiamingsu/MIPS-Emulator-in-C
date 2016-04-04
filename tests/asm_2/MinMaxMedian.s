.data
nums: .word 92, 31, 92, 6, 54, 54, 62, 33, 8, 52
length: .word 10
min: .word 0
max: .word 0
median: .word 0
.text
# array address -> s0
# length -> s1
	la	$s0,nums
	la	$t0,length
	lw	$s1,($t0)
	xori	$t0,$zero,0
# bubblesort
L1:	beq	$t0,$s1,B1
	xori	$t1,$zero,0
L2:	beq	$t1,$s1,B2
# swap
	addiu	$t2,$t1,-1
	sll	$t2,$t2,2
	sll	$t3,$t1,2
	addu	$t2,$t2,$s0
	addu	$t3,$t3,$s0
	lw	$t4,($t2)
	lw	$t5,($t3)
	blt	$t4,$t5,NOSW
	nop
	sw	$t4,($t3)
	sw	$t5,($t2)
NOSW:	addiu	$t1,$t1,1
	j	L2
	nop
B2:	addiu	$t0,$t0,1	
	j	L1
	nop
B1:	la	$t0,min
	la	$t1,max
	la	$t2,median
# save min
	lw	$t3,($s0)
	sw	$t3,($t0)
	add   $v0,$zero,4007
	add   $a0,$zero,$t3
	syscall
# save max
	addiu	$t4,$s1,-1
	sll	$t4,$t4,2
	addu	$t4,$t4,$s0
	lw	$t3,($t4)
	sw	$t3,($t1)
	add   $v0,$zero,4007
	add   $a0,$zero,$t3
	syscall
# median
	xori	$t4,$s1,0
	srl	$t4,$t4,1
	sll	$t4,$t4,2
	addu	$t4,$t4,$s0
	lw	$t3,($t4)
	sw	$t3,($t2)
	add   $v0,$zero,4007
	add   $a0,$zero,$t3
	syscall
	
# exit program
	li	$v0,4001
	syscall
