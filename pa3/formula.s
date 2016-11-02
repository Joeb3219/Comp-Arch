	.code32
	.file	"formula_c.c"
	.text

## ================================= ##
## STINGS USED IN PROGRAM EXECUTION  ##
## ================================= ##
.L_INTEGER_STRING:
        .ASCIZ "%d\n"
        .text
.L_HELP_FLAG_STRING:
        .ASCIZ "-h"
        .text
.L_HELP_STRING:
        .ASCIZ "USAGE: formula <positive integer>\n"
        .text
.G_N_TOO_LARGE:
	.ASCIZ "N TOO LARGE: OVERFLOW\n"
	.text
.G_N_NEGATIVE:
	.ASCIZ "N IS LESS THAN ZERO\n"
	.text
.G_FORMAT_FIRST:
	.ASCIZ "1"
	.text
.G_FORMAT_N:
	.ASCIZ " + %d*x^%d"
	.text
.G_NEW_LINE:
	.ASCIZ " \n"
	.text
.G_FACT_DEBUG:
	.ASCIZ "Computing %d!\n"
	.text
.G_DEBUG:
	.ASCIZ "<<%d>>\n"
	.text
.G_RETURN:
	.ASCIZ "MOVING %d INTO EAX\n"
	.text
.G_TWO_INT:
	.ASCIZ "%d,%d / %d\n"
	.text

## ================================= ##
## FACTORIAL FUNCTION                ##
## ================================= ##
	.globl  Factorial
        .type   Factorial, @function
Factorial:
# The value, n, provided by the user will be stored in 8(%ebp)
# The value, val, which holds the result, will be stored in -4(%ebp)
.LFB2:
	pushl	%ebp			# Push base pointer
	movl	%esp, %ebp		# Set the base pointer to the rsp.
	subl	$8, %esp		# We need 4 byes for 1 local int
	movl	$1, -4(%ebp)		# val = 1	
	
#	pushl	8(%ebp)
#	pushl	$.G_FACT_DEBUG
#	call	printf

	jmp	.L_FACTORIAL_W_LOOP
.L_FACTORIAL_INNER_LOOP:
	movl	8(%ebp), %eax		# n -> %eax
	movl	-4(%ebp), %ebx		# val -> %ebx
	imull	%ebx, %eax		# val * n
	jo	.L_FACTORIAL_ERROR	# If overflow, jump to error
	movl	%eax, -4(%ebp)		# Otherwise, move result of val * n -> val
	movl	8(%ebp), %eax		# Move n -> %eax
	addl	$-1, %eax		# n - 1 -> %eax
	movl	%eax, 8(%ebp)		# %eax -> n
.L_FACTORIAL_W_LOOP:
	cmpl	$0, 8(%ebp)		# Compare 0 and n
	jg	.L_FACTORIAL_INNER_LOOP	# If n > 0, jump to inner loop.
	jmp .L_FACTORIAL_RETURN		# Otherwise, let's return the value of the function.
.L_FACTORIAL_ERROR:
	pushl	$.G_N_TOO_LARGE		# Push that N is too large
	call	printf			# Call printf
	movl	$0, %eax		# Push 0 into %eax to indicate error
	popl	%ebp			# Pop the base pointer
	ret				# REturn control to the caller
.L_FACTORIAL_RETURN:
	movl    -4(%ebp), %eax          # Put val into %eax, to be returned to the caller.
	movl	%ebp, %esp
	popl    %ebp                    # Pop the base pointer
        ret				# Return control to the caller

## ================================= ##
## NCR FUNCTION                      ##
## ================================= ##
	.globl  nCr
        .type   nCr, @function
nCr:
# The value, n, which holds the result, will be stored in 8(%ebp)
# The value, r, provided by the user will be stored in 12(%ebp)
# The values, r!, n!, and (r-n)!, will be stored in -4(%ebp), -8(%ebp), and -12(%ebp), respectively.
.LFB3:
	pushl	%ebp			# Push base pointer
	movl	%esp, %ebp		# Set the base pointer to the rsp.
	subl	$16, %esp		# We need 12 bytes for 3 local ints.
	# Compute n!
	movl	8(%ebp), %eax		# n -> %eax
	pushl	%eax			# Push %eax
	call	Factorial		# Call Factorial(n)
	test	%eax, %eax		# Test %eax to set condition variables
	jz	.L_NCR_ERROR		# An error has occurred since Factorial(n) == 0
	movl	%eax, -8(%ebp)		# Factorial(n) -> n!
	# Compute r!
        movl    12(%ebp), %eax          # r -> %eax
        pushl   %eax                    # Push %eax
        call    Factorial               # Call Factorial(r)
	test	%eax, %eax		# Test %eax to set condition variables
	jz	.L_NCR_ERROR		# An error has occurred since Factorial(n) == 0
        movl    %eax, -4(%ebp)          # Factorial(r) -> r!	
	# Compute (r-n)!
        movl    12(%ebp), %eax          # r -> %eax
	subl	8(%ebp), %eax		# r-n -> %eax	
	imull	$-1, %eax		# -1 * %eax -> positive
	pushl   %eax                    # Push %eax
        call    Factorial               # Call Factorial(r-n)
	test	%eax, %eax		# Test %eax to set condition variables
	jz	.L_NCR_ERROR		# An error has occurred since Factorial(n) == 0
        movl    %eax, -12(%ebp)         # Factorial(r-n) -> (r-n)!
	# Compute (n!)/(r!*(r-n)!)
	movl	-4(%ebp), %eax		# r! -> %eax
	movl	-12(%ebp), %ebx		# (r-n)! -> %ebx
	imull	%eax, %ebx		# r! * (r-n)! -> %ebx
	jo	.L_NCR_ERROR		# Overflow
	movl	-8(%ebp), %eax		# n! -> %eax
	movl	$0, %edx

	divl	%ebx
	
#	idivl	%edx, %eax		# n! / (r!*(r-n)!) -> %eax
#	idivl	%edx			# n! / (r!*(r-n)!) -> %eax
	jo	.L_NCR_ERROR		# Overflow
	jmp .L_NCR_RETURN		# return value
.L_NCR_ERROR:
	movl $0, %eax
	jmp .L_NCR_RETURN	
.L_NCR_RETURN:
	movl	%ebp, %esp
	popl	%ebp			# Pop the base pointer
	ret				# Return control to the caller

## ================================= ##
## MAIN FUNCTION                     ##
## ================================= ##
	.globl  main
        .type   main, @function
main:
# The integer n, a value which we interpret from user input, is stored in -4(%ebp)
# The value argv, passed to main, is stored as 8(%ebp)
# The char** argc, passed to main, is stored as 12(%ebp)
# The value, count, will be stored in -8(%ebp)
.LFB4:
	pushl	%ebp			# Push base pointer
	movl	%esp, %ebp		# Set the base pointer to the rsp.
	subl	$8, %esp		# We need 8 byes for 2 local ints
	cmpl $2, 8(%ebp)		# Test 2 != argv
	jne .L_MAIN_NOT_ONE_ARG		# If argv isn't 2, jump to NOT_ONE_ARG
	movl    12(%ebp), %eax		# Set %eax to argc
	movl	4(%eax), %eax		# Set %eax to argc[1]
        pushl   %eax			# Push argc[1]
	pushl	$.L_HELP_FLAG_STRING	# Set %esi to HELP_FLAG_STRING
	call	strcmp			# Call strcmp(argc[1], HELP_FLAG_STRING)
	cmpl	$0, %eax		# Test that strcmp(argc[1], HELP_FLAG_STRING) == 0, implying they're equal
	je .L_MAIN_HELP_FLAG		# If strcmp returns 0, the strings are equal therefore the user is asking for help.
	jmp .L_MAIN_FORMULA		# Jump to main formula execution
.L_MAIN_FORMULA:
	movl	$0, -4(%ebp)		# 0 -> n
	movl	12(%ebp), %eax		# Set %eax to argc
	movl    4(%eax), %eax           # Set %eax to argc[1]
	pushl   %eax                    # Push argc[1]
	call	atoi			# Call atoi(argc[1])
	movl	%eax, -4(%ebp)		# atoi(argc[1]) -> n
	cmpl	$0, %eax		# Set condition flags for $eax
	jl	.L_MAIN_NEGATIVE_N	# if n < 0, jump to negative n
	pushl	$.G_FORMAT_FIRST	# Push the format for the first element
	call	printf			# Print
	movl	$1, -8(%ebp)		# Set count to 1
	jmp	.L_MAIN_CALCULATE	# Jump into a loop	
.L_MAIN_CALCULATE:
	movl	-8(%ebp), %eax		# Move count -> %eax
	movl	-8(%ebp), %eax		# Move count -> %eax
	movl	-4(%ebp), %ebx		# Move n -> %ebx
	cmpl	%ebx, %eax		# Compare %ebx & %eax
	jg	.L_SUCCESSFUL_EXIT	# Success, let's go home boys
	pushl	%eax			# Push count
	pushl	%ebx			# Push n
	call	nCr			# Call nCr
	test	%eax, %eax		# Set flags for %eax
	jz	.L_MAIN_OVERFLOW	# Jump to overflow condition
	pushl	-8(%ebp)		# Push count onto stack
	pushl	%eax			# Push result onto stack
	#jmp .L_SUCCESSFUL_EXIT
	pushl	$.G_FORMAT_N		# Push format onto stack
	call printf			# Print
	movl	-8(%ebp), %eax		# Move count -> %eax
	addl	$1, %eax		# count ++
	movl	%eax, -8(%ebp)		# count ++ -> %eax
	jmp	.L_MAIN_CALCULATE
.L_SUCCESSFUL_EXIT:
	pushl	$.G_NEW_LINE		# End our output
	call	printf			# printf
	movl	$0, %eax		# Push 0 to %eax -- successful execution
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_OVERFLOW:
	pushl	$.G_N_TOO_LARGE		# Push that n is too large
	call	printf			# Print
	jmp .L_MAIN_NOT_ONE_ARG		# Error condition
.L_MAIN_HELP_FLAG:
        pushl $.L_HELP_STRING		# Put format string into %edi
        call puts			# Call puts on last argument
	movl $0, %eax			# Push 0 to %eax -- successful program execution.
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_NOT_ONE_ARG:
	movl $1, %eax			# Our program had a failure, so we push 1 to %eax to return to the caller.
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_NEGATIVE_N:
	pushl $.G_N_NEGATIVE		# Tell user that n is negative
	call printf			# Print
	movl $1, %eax			# Push 1 to indicate an error
	jmp .L_MAIN_RETURN		# Return
.L_MAIN_RETURN:
	leave				# Our program execution has finished.
	ret				# Return control to the caller.
