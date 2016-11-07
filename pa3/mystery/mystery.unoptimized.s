	.file	"mystery.c"
	.local	num
	.comm	num,800,32
	.text
	.globl	add
	.type	add, @function
add:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %edx
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	add, .-add
	.globl	dothething
	.type	dothething, @function
dothething:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movl	%edi, -20(%rbp)
	cmpl	$199, -20(%rbp)
	jg	.L4
	cmpl	$0, -20(%rbp)
	jns	.L5
.L4:
	movl	$0, %eax
	jmp	.L6
.L5:
	movl	-20(%rbp), %eax
	cltq
	movl	num(,%rax,4), %eax
	cmpl	$-1, %eax
	je	.L7
	movl	-20(%rbp), %eax
	cltq
	movl	num(,%rax,4), %eax
	jmp	.L6
.L7:
	movl	-20(%rbp), %eax
	subl	$2, %eax
	movl	%eax, %edi
	call	dothething
	movl	%eax, %ebx
	movl	-20(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %edi
	call	dothething
	movl	%ebx, %esi
	movl	%eax, %edi
	call	add
	movl	-20(%rbp), %edx
	movslq	%edx, %rdx
	movl	%eax, num(,%rdx,4)
	movl	-20(%rbp), %eax
	cltq
	movl	num(,%rax,4), %eax
.L6:
	addq	$24, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	dothething, .-dothething
	.section	.rodata
.LC0:
	.string	"Value: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L9
.L10:
	movl	-4(%rbp), %eax
	cltq
	movl	$-1, num(,%rax,4)
	addl	$1, -4(%rbp)
.L9:
	cmpl	$199, -4(%rbp)
	jle	.L10
	movl	$1, num+4(%rip)
	movl	num+4(%rip), %eax
	movl	%eax, num(%rip)
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi
	movl	%eax, -12(%rbp)
	cmpl	$200, -12(%rbp)
	jg	.L11
	cmpl	$0, -12(%rbp)
	jg	.L12
.L11:
	movl	$0, -8(%rbp)
	jmp	.L13
.L12:
	movl	-12(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %edi
	call	dothething
	movl	%eax, -8(%rbp)
.L13:
	movl	-8(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
