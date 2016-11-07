	.file	"mystery.c"
	.text
	.globl	add
	.type	add, @function
add:
.LFB20:
	.cfi_startproc
	leal	(%rdi,%rsi), %eax
	ret
	.cfi_endproc
.LFE20:
	.size	add, .-add
	.globl	dothething
	.type	dothething, @function
dothething:
.LFB21:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, %ebx
	movl	$0, %eax
	cmpl	$199, %edi
	ja	.L3
	movslq	%edi, %rax
	movl	num(,%rax,4), %eax
	cmpl	$-1, %eax
	jne	.L3
	leal	-2(%rdi), %edi
	call	dothething
	movl	%eax, %ebp
	leal	-1(%rbx), %edi
	call	dothething
	addl	%ebp, %eax
	movslq	%ebx, %rbx
	movl	%eax, num(,%rbx,4)
.L3:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE21:
	.size	dothething, .-dothething
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Value: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB22:
	.cfi_startproc
	movl	$num, %eax
	movl	$num+800, %edx
.L8:
	movl	$-1, (%rax)
	addq	$4, %rax
	cmpq	%rdx, %rax
	jne	.L8
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$1, num+4(%rip)
	movl	$1, num(%rip)
	movq	8(%rsi), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	leal	-1(%rax), %edx
	movl	$0, %esi
	cmpl	$199, %edx
	ja	.L9
	movl	%edx, %edi
	call	dothething
	movl	%eax, %esi
.L9:
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE22:
	.size	main, .-main
	.local	num
	.comm	num,800,32
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-4)"
	.section	.note.GNU-stack,"",@progbits
