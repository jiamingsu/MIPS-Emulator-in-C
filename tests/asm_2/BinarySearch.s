.data
length: .word 10
nums: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
target: .word 7
notfound: .asciiz "Target not found"
.text 

# set s1 (low) to 0
	xori	$s1,$zero,0
# set s2 (high) to n-1
	la	$t0,length
	lw	$s2,($t0)
	addiu	$s2,$s2,-1
# load array start address
	la	$s3,nums
# load target value to s4
	la	$t0,target
	lw	$s4,($t0)
WHILE: blt	$s2,$s1,NOTFOUND
# store address of mid in t0
	addu	$t0,$s1,$s2
	srl	$t0,$t0,1
	
# store array index address in t2 and load to t1
	xor	$t2,$t0,$zero
	sll	$t2,$t2,2
	addu	$t2,$t2,$s3
	lw	$t1,0($t2)
# if it is found, print and end
	
	bne	$t1,$s4,S1
	nop
	xor	$a0,$t0,$zero
        li      $v0,0xFA7             #syscall int out to file out              
	syscall
# exit
	li	$v0, 0XFA1
	syscall	
# if it is greater
S1:	blt	$t1,$s4,S2
	nop
	addiu	$s2,$t0,-1
	j WHILE
	nop
# if it is less
S2:	addiu	$s1,$t0,1
	j	WHILE
	nop
NOTFOUND:
        li      $v0,0xFA4             #syscall file out              
	li	$a0, 1                #Use stdout
	la	$a1, notfound         #string address
	li      $a2, 18               #Length of output
	syscall
	li	$v0,0xFA1
	syscall
