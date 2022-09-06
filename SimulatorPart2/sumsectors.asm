	out $zero, $imm2, $zero, $imm1, IRQ, 6	# set irqhandler as IRQ
	out $zero, $zero, $imm1, $imm1, 1, 0	# enable irq1
	add $t1, $zero, $imm1, $zero, 1, 0	# set $t1 = 1
	lw $s1, $zero, $imm1, $zero, 128, 0	# get number of sectors to process from address 128
	lw $a0, $zero, $imm1, $zero, 129, 0	# get sector number from address 129
	jal $ra, $zero, $zero, $imm1, F1, 0	# issue read cmd, sector n
	add $a0, $a0, $imm1, $zero, 1, 0	# increment sector number
	sw $a0, $zero, $imm1, $zero, 129, 0	# write back to address 129
	jal $ra, $zero, $zero, $imm1, F2, 0	# wait for disk to be ready
L1:
	lw $a0, $zero, $imm1, $zero, 129, 0	# get sector number from address 129
	beq $zero, $s1, $a0, $imm1, L2, 0	# jump to L2 if we already read last sector
	jal $ra, $zero, $zero, $imm1, F1, 0	# issue read cmd, sector n
	add $a0, $a0, $imm1, $zero, 1, 0	# increment sector number
	sw $a0, $zero, $imm1, $zero, 129, 0	# write back to address 129
L2:
	lw $a0, $zero, $imm1, $zero, 130, 0	# get sector number to sum from address 130
	jal $ra, $zero, $zero, $imm1, F3, 0	# calc sum of sector n
	add $s0, $s0, $v0, $zero, 0, 0		# add to running sum
	add $a0, $a0, $imm1, $zero, 1, 0	# increment sector address
	sw $a0, $zero, $imm1, $zero, 130, 0	# write back to address 130
	beq $zero, $s1, $a0, $imm1, L3, 0	# jump to L3 if we summed last
	jal $ra, $zero, $zero, $imm1, F2, 0	# wait for disk to be ready
	beq $zero, $zero, $zero, $imm1, L1, 0	# jump to L1
L3:
	sw $s0, $zero, $imm1, $zero, 131, 0	# write sum to address 131
	halt $zero, $zero, $zero, $zero, 0, 0	# halt
F1:
	out $zero, $zero, $imm1, $a0, 15, 0	# out sector number to discksector
	and $t2, $a0, $imm1, $imm1, 1, 0	# $t2 = $a0 & 1
	sll $t2, $imm1, $t2, $zero, 256, 0	# $t2 = 256 for even sectors, 512 for odd
	out $zero, $zero, $imm1, $t2, 16, 0	# out buffer address $t2 to diskbuffer
	add $t0, $zero, $imm1, $zero, 14, 0	# set $t0 = 14
	out $zero, $t0, $zero, $imm1, 1, 0	# initiate read cmd
	beq $zero, $zero, $zero, $ra, 0, 0	# and return
F2:
	in $t0, $zero, $imm1, $zero, 17, 0	# read diskstatus into $t0
	bne $zero, $t0, $zero, $imm1, F2, 0	# wait for status to become 0
	beq $zero, $zero, $zero, $ra, 0, 0	# and return
F3:
	add $v0, $zero, $zero, $zero, 0, 0	# zero sum
	add $t0, $zero, $imm1, $zero, 127, 0	# $t0 = 127, starting to sum from last line to sum in sector
	and $t2, $a0, $imm1, $imm1, 1, 0	# $t2 = $a0 & 1
	sll $t2, $imm1, $t2, $zero, 256, 0	# $t2 = 256 for even sectors, 512 for odd
L4:
	lw $s2, $t0, $t2, $zero, 0, 0		# read current line in sector
	add $v0, $v0, $s2, $zero, 0, 0		# add to sum
	sub $t0, $t0, $t1, $zero, 0, 0		# decrement line counter
	bge $zero, $t0, $zero, $imm1, L4, 0	# loop while we still need to sum all lines
	beq $zero, $zero, $zero, $ra, 0, 0	# and return
IRQ:
	add $gp, $gp, $imm1, $zero, 1, 0	# increment $gp
	out $zero, $zero, $imm1, $zero, 4, 0	# clear irq1 status
	reti $zero, $zero, $zero, $zero, 0, 0	# return from interrupt
	.word 128 4				# number of sectors to sum
