	add $t0, $imm1, $imm1, $imm2, -1, 4		# 0: $t0 = -1-1+4=2
	sub $t1, $t0, $imm1, $imm2, 3, 2		# 1: $t1 = $t0-3-2 = -3
	mac $t2, $t0, $t1, $imm1, 5, 0			# 2: $t2 = $t0*$t1+5 = -6+5 = -1
	and $s0, $t2, $imm1, $imm2, 7, 3		# 3: $s0 = (-1) & 7 & 3 = 3
	or $s1, $s0, $imm1, $imm2, 8, 16		# 4: $s1 = 3 | 8 | 16 = 27	
	xor $s2, $t2, $t2, $t2, 0, 0			# 5: $s2 = (-1) ^ (-1) ^ (-1) = -1
	sll $t0, $s2, $imm1, $zero, 1, 0		# 6: $t0 = $s2 << 1 = -2
	srl $sp, $t0, $imm1, $zero, 1, 0		# 7: $sp = $t0 >> 1 (logical shift) = 0x7fffffff
	sra $t1, $t0, $imm2, $zero, 0, 1		# 8: $t1 = $t0 >> 1 = -1
	beq $zero, $t0, $t1, $imm1, L1, 0		# 9: if ($t0 == $t1) goto L1
L1:
	bne $zero, $t0, $t1, $imm1, L2, 0		# a: if ($t0 != $t1) goto L2
L2:
	blt $zero, $t0, $t1, $imm1, L3, 0		# b: if ($t0 < $t1) goto L3
L3:
	bgt $zero, $t0, $t1, $imm1, L4, 0		# c: if ($t0 > $t1) goto L4
	bge $zero, $t0, $t1, $imm1, L4, 0		# d: if ($t0 >= $t1) goto L4
	jal $ra, $zero, $zero, $imm2, 0, L5		# e: call L5
	ble $zero, $t0, $t1, $imm1, L4, 0		# f: if ($t0 <= $t1) goto L4
L5:
	add $gp, $zero, $zero, $imm2, 0, 9		# 10: $gp = 9
L4:
	halt $zero, $zero, $zero, $zero, 0, 0		# 11: halt
