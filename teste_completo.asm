.data
str0:
    .asciiz "teste completo"
.text
.globl main
main:
move $fp, $sp
addiu $sp, $sp, -16
li $s0, 10
sw $s0, 4($fp)
li $s0, 20
sw $s0, 8($fp)
li $s0, 120
sw $s0, 12($fp)
li $s0, 121
sw $s0, 16($fp)
lw $s0, 4($fp)
li $v0, 1
move $a0, $s0
syscall
lw $s0, 12($fp)
li $v0, 1
move $a0, $s0
syscall
li $v0, 4
la $a0, str0
syscall
li $v0, 11
li $a0, 10
syscall
lw $s0, 8($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 5
lw $t1, 4($sp)
addiu $sp, $sp, 4
add $s0, $t1, $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 2
lw $t1, 4($sp)
addiu $sp, $sp, 4
sub $s0, $t1, $s0
sw $s0, 8($fp)
lw $s0, 8($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 3
lw $t1, 4($sp)
addiu $sp, $sp, 4
mult $t1, $s0
mflo $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 2
lw $t1, 4($sp)
addiu $sp, $sp, 4
div $t1, $s0
mflo $s0
sw $s0, 8($fp)
lw $s0, 8($fp)
sub $s0, $zero, $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $s0, $t1
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $t1, $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $s0, $t1
bne $s0, $zero, L0
li $s0, 1
b L1
L0:
li $s0, 0
L1:
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $t1, $s0
bne $s0, $zero, L2
li $s0, 1
b L3
L2:
li $s0, 0
L3:
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
beq $t1, $s0, L4
li $s0, 0
b L5
L4:
li $s0, 1
L5:
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
bne $t1, $s0, L6
li $s0, 0
b L7
L6:
li $s0, 1
L7:
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
and $s0, $t1, $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
lw $s0, 8($fp)
lw $t1, 4($sp)
addiu $sp, $sp, 4
or $s0, $t1, $s0
sw $s0, 4($fp)
lw $s0, 4($fp)
beq $s0, $zero, L8
li $s0, 0
b L9
L8:
li $s0, 1
L9:
sw $s0, 4($fp)
li $v0, 5
syscall
sw $v0, 4($fp)
li $v0, 5
syscall
sw $v0, 12($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 0
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $s0, $t1
beq $s0, $zero, L10
lw $s0, 4($fp)
li $v0, 1
move $a0, $s0
syscall
L10:
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 0
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $s0, $t1
beq $s0, $zero, L11
lw $s0, 4($fp)
li $v0, 1
move $a0, $s0
syscall
b L12
L11:
lw $s0, 8($fp)
li $v0, 1
move $a0, $s0
syscall
L12:
L13:
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 0
lw $t1, 4($sp)
addiu $sp, $sp, 4
slt $s0, $s0, $t1
beq $s0, $zero, L14
addiu $sp, $sp, -4
lw $s0, 4($fp)
sw $s0, 20($fp)
lw $s0, 4($fp)
sw $s0, 0($sp)
addiu $sp, $sp, -4
li $s0, 1
lw $t1, 4($sp)
addiu $sp, $sp, 4
sub $s0, $t1, $s0
sw $s0, 4($fp)
addiu $sp, $sp, 4
b L13
L14:
addiu $sp, $sp, -4
li $s0, 122
sw $s0, 20($fp)
lw $s0, 20($fp)
li $v0, 1
move $a0, $s0
syscall
addiu $sp, $sp, 4
addiu $sp, $sp, 16
li $v0, 10
syscall
