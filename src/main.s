	.file	"main.c"
	.option nopic
	.attribute arch, "rv32i2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	li	a5,151982080
	addi	a5,a5,-509
	sw	a5,-24(s0)
	li	a5,26
	sb	a5,-20(s0)
	li	a5,-1
	sb	a5,-17(s0)
	sb	zero,-18(s0)
	j	.L2
.L4:
	lbu	a5,-18(s0)
	addi	a5,a5,-16
	add	a5,a5,s0
	lbu	a5,-8(a5)
	lbu	a4,-17(s0)
	bgeu	a4,a5,.L3
	lbu	a5,-18(s0)
	addi	a5,a5,-16
	add	a5,a5,s0
	lbu	a5,-8(a5)
	sb	a5,-17(s0)
.L3:
	lbu	a5,-18(s0)
	addi	a5,a5,1
	sb	a5,-18(s0)
.L2:
	lbu	a4,-18(s0)
	li	a5,4
	bleu	a4,a5,.L4
	li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (gc891d8dc23e) 13.2.0"
