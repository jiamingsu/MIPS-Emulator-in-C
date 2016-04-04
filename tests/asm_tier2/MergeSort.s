.data
nums: .word 10,9,8,7,6,5,4,3,2,1
sorted: .word 0:10
length: .word 10
.text
# nums address -> s0
# sorted address -> s1
# 1st out of bounds nums address -> s2
# 1st out of bounds sorted address -> s7
# (len/2)*4 -> s6
	la	$s0,nums
	la	$s1,sorted
	la	$t0,length
	lw	$s6,($t0)
	sll	$s6,$s6,1
	sll	$s2,$s6,1
	xori	$s7,$s2,0
	addu	$s2,$s2,$s0
	addu	$s7,$s7,$s1
# s3 -> (gap/2)*4
# s4 -> array switch status
	li	$s3,4
	li	$s4,1
# t0 iterates over partitions of size gap (i)
# t8 stores merge destination pointer
GAP:	xori	$t0,$s0,0
	xori	$t8,$s1,0
	sub	$t9,$s1,$s0
	addi	$t9,$t9,-4
# t1 is i + gap/2
# t2 is i + gap
PART:	addu	$t1,$t0,$s3
# case for empty second partition
	blt 	$t1,$s2,L
	addu	$t2,$t1,$s3
	sub	$t3,$t0,$s0
	add	$t3,$t3,$s1
CPY:	lw	$t5,($t0)
	sw	$t5,($t3)
	addiu	$t0,$t0,4
	bne	$t0,$s2,CPY
	addiu	$t3,$t3,4
	j	CLEAN
	nop	
L:	ble	$t2,$s2,P
	nop
	xori	$t2,$s2,0
# start merge
P:	xori	$t4,$t1,0
	add	$t7,$t2,$t9
# case where second half is empty
MERGE:	bne	$t4,$t2,N1
	lw	$t5,($t0)
	sw	$t5,($t8)
	j	ME
	addiu	$t0,$t0,4
# cases where first half is empty
N1:	bne	$t0,$t1,N2
	lw	$t5,($t4)
	sw	$t5,($t8)
	j	ME
	addiu	$t4,$t4,4
# load for comparison
N2:	lw	$t5,($t0)
	lw	$t6,($t4)
# take from second list
	blt	$t5,$t6,N3	
	sw	$t6,($t8)
	j	ME
	addiu	$t4,$t4,4
# take from first list
N3:	sw	$t5,($t8)
	addiu	$t0,$t0,4
# increment and continue merging
ME:	bne	$t8,$t7,MERGE
	addiu	$t8,$t8,4
# end merge
	blt	$t2,$s2,PART
	xori	$t0,$t2,0
# raise gap and swap array addresses
CLEAN:	xor	$s0,$s0,$s1
	xor	$s1,$s0,$s1
	xor	$s0,$s0,$s1
	xor	$s2,$s2,$s7
	xor	$s7,$s2,$s7
	xor	$s2,$s2,$s7
	xori	$s4,$s4,1
	blt 	$s3,$s6,GAP
	sll	$s3,$s3,1
	beqz	$s4,SKIP
# if necessary, make final copy
FIX:	lw	$t0,($s0)
	sw	$t0,($s1)
	addiu	$s0,$s0,4
	bne	$s0,$s2,FIX
	addiu	$s1,$s1,4
#exit program
SKIP:	li	$v0,0xFA1
	syscall
