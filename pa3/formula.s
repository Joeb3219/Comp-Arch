	.file	"formula_c.c"
	.text

## ================================= ##
## STINGS USED IN PROGRAM EXECUTION  ##
## ================================= ##
.L_INTEGER_STRING:
        .string "%d\n"
        .text
.L_HELP_FLAG_STRING:
        .string "-h"
        .text
.L_HELP_STRING:
        .string "USAGE: formula <positive integer>\n"
        .text

## ================================= ##
## FACTORIAL FUNCTION                ##
## ================================= ##
	.globl  Factorial
        .type   Factorial, @function
Factorial:
# The value, n, provided by the user will be stored in -8(%rbp)
# The value, val, which holds the result, will be stored in -4(%rbp)
.LFB2:
	pushq	%rbp			# Push base pointer
	movq	%rsp, %rbp		# Set the base pointer to the rsp.
	movl	$1, -4(%rbp)		# val = 1
	movl	%edi, -8(%rbp)		# n, provided by user.
	jmp	.L_FACTORIAL_W_LOOP
.L_FACTORIAL_INNER_LOOP:
	movl	-8(%rbp), %eax
	leal    -1(%rax), %edx
	movl	%edx, -8(%rbp)
	movl	-4(%rbp), %edx
	imull	%edx, %eax
	movl	%eax, -4(%rbp)
.L_FACTORIAL_W_LOOP:
	cmpl	$0, -8(%rbp)		# Compare 0 and n
	jg	.L_FACTORIAL_INNER_LOOP	# If n > 0, jump to inner loop.
	jmp .L_FACTORIAL_RETURN		# Otherwise, let's return the value of the function.
.L_FACTORIAL_RETURN:
	movl    -4(%rbp), %eax          # Put val into %eax, to be returned to the caller.
        popq    %rbp                    # Pop the base pointer
        ret				# Return control to the caller

## ================================= ##
## NCR FUNCTION                      ##
## ================================= ##
	.globl  nCr
        .type   nCr, @function
nCr:
# The value, n, which holds the result, will be stored in -4(%rbp)
# The value, r, provided by the user will be stored in -8(%rbp)
.LFB3:
	pushq	%rbp			# Push base pointer
	movq	%rsp, %rbp		# Set the base pointer to the rsp.
	movl	%edi, -4(%rbp)		# n
	movl	%esi, -8(%rbp)		# r
	jmp .L_NCR_RETURN		# return value
.L_NCR_RETURN:
	movl $0, %eax			# Push value to %eax
	popq	%rbp			# Pop the base pointer
	ret				# Return control to the caller

## ================================= ##
## MAIN FUNCTION                     ##
## ================================= ##
	.globl  main
        .type   main, @function
main:
# The integer n, a value which we interpret from user input, is stored in -4(%rbp)
# The value argv, passed to main, is stored as -8(%rbp)
# The char** argc, passed to main, is stored as -12(%rbp)
.LFB4:
	pushq	%rbp			# Push base pointer
	movq	%rsp, %rbp		# Set the base pointer to the rsp.
#	subq	$32, %rsp		# Move rbp back 32 bytes
	movl	$0, -4(%rbp)		# Set n to 0.		
	movl	%edi, -8(%rbp)		# Set the value of argv.
	movq	%rsi, -12(%rbp)		# Set the value of argc.
	cmpl $1, -8(%rbp)		# Test 1 != argv
	jne .L_MAIN_NOT_ONE_ARG		# If argv isn't 1, jump to NOT_ONE_ARG
	movq    -12(%rbp), %rax		# Set %rax to argc
        movq    (%rax), %rdi		# Set %rdi to argc[0]
	movl	$.L_HELP_FLAG_STRING, %esi	# Set %esi to HELP_FLAG_STRING
	call strcmp			# Call strcmp(argc[0], HELP_FLAG_STRING)
	cmpl	$0, %eax		# Test that strcmp(argc[0], HELP_FLAG_STRING) == 0, implying they're equal
	je .L_MAIN_HELP_FLAG		# If strcmp returns 0, the strings are equal therefore the user is asking for help.
	jmp .L_MAIN_FORMULA		# Jump to main formula execution
.L_MAIN_FORMULA:
	movl	$10, %edi		# Store value 10 -> %edi
	call	Factorial		# Call Factorial(%edi)
	movl	%eax, %esi		# Store result of Factorial into %esi
	movl	$.L_INTEGER_STRING, %edi	# Put format string into %edi
	movl	$0, %eax		# Zero floating point registers used in %eax
	call	printf			# Call printf with the last three arguments.
	movl $0, %eax			# Push 0 to %eax
	jmp .L_MAIN_RETURN		# Exit from the program.
.L_MAIN_HELP_FLAG:
        movl $.L_HELP_STRING, %edi      # Put format string into %edi
        call puts			# Call puts on last argument
	movl $0, %eax			# Push 0 to %eax -- successful program execution.
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_NOT_ONE_ARG:
	movl $1, %eax			# Our program had a failure, so we push 1 to %eax to return to the caller.
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_RETURN:
	leave				# Our program execution has finished.
	ret				# Return control to the caller.
