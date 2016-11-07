## ================================= ##
## FACTORIAL FUNCTION                ##
## ================================= ##
        .globl  Factorial
        .type   Factorial, @function
Factorial:
# The value, n, provided by the user will be stored in 8(%ebp)
# The value, val, which holds the result, will be stored in -4(%ebp)
.LFB2:
        pushl   %ebp                    # Push base pointer
        movl    %esp, %ebp              # Set the base pointer to the rsp.
        subl    $8, %esp                # We need 4 byes for 1 local int
        movl    $1, -4(%ebp)            # val = 1
        jmp     .L_FACTORIAL_W_LOOP
.L_FACTORIAL_INNER_LOOP:
        movl    8(%ebp), %eax           # n -> %eax
        movl    -4(%ebp), %ebx          # val -> %ebx
        imull   %ebx, %eax              # val * n
        jo      .L_FACTORIAL_ERROR      # If overflow, jump to error
        movl    %eax, -4(%ebp)          # Otherwise, move result of val * n -> val
        movl    8(%ebp), %eax           # Move n -> %eax
        addl    $-1, %eax               # n - 1 -> %eax
        movl    %eax, 8(%ebp)           # %eax -> n
.L_FACTORIAL_W_LOOP:
        cmpl    $0, 8(%ebp)             # Compare 0 and n
        jg      .L_FACTORIAL_INNER_LOOP # If n > 0, jump to inner loop.
        jmp .L_FACTORIAL_RETURN         # Otherwise, let's return the value of the function.
.L_FACTORIAL_ERROR:
        movl    $0, %eax                # Push 0 into %eax to indicate error
        popl    %ebp                    # Pop the base pointer
        ret                             # REturn control to the caller
.L_FACTORIAL_RETURN:
        movl    -4(%ebp), %eax          # Put val into %eax, to be returned to the caller.
        movl    %ebp, %esp
        popl    %ebp                    # Pop the base pointer
        ret                             # Return control to the caller

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
        pushl   %ebp                    # Push base pointer
        movl    %esp, %ebp              # Set the base pointer to the rsp.
        subl    $16, %esp               # We need 12 bytes for 3 local ints.
        # Compute n!
        movl    8(%ebp), %eax           # n -> %eax
        pushl   %eax                    # Push %eax
        call    Factorial               # Call Factorial(n)
        test    %eax, %eax              # Test %eax to set condition variables
        jz      .L_NCR_ERROR            # An error has occurred since Factorial(n) == 0
        movl    %eax, -8(%ebp)          # Factorial(n) -> n!
        # Compute r!
        movl    12(%ebp), %eax          # r -> %eax
        pushl   %eax                    # Push %eax
        call    Factorial               # Call Factorial(r)
        test    %eax, %eax              # Test %eax to set condition variables
        jz      .L_NCR_ERROR            # An error has occurred since Factorial(n) == 0
        movl    %eax, -4(%ebp)          # Factorial(r) -> r!
        # Compute (r-n)!
        movl    12(%ebp), %eax          # r -> %eax
        subl    8(%ebp), %eax           # r-n -> %eax
        imull   $-1, %eax               # -1 * %eax -> positive
        pushl   %eax                    # Push %eax
        call    Factorial               # Call Factorial(r-n)
        test    %eax, %eax              # Test %eax to set condition variables
        jz      .L_NCR_ERROR            # An error has occurred since Factorial(n) == 0
        movl    %eax, -12(%ebp)         # Factorial(r-n) -> (r-n)!
        # Compute (n!)/(r!*(r-n)!)
        movl    -4(%ebp), %eax          # r! -> %eax
        movl    -12(%ebp), %ebx         # (r-n)! -> %ebx
        imull   %eax, %ebx              # r! * (r-n)! -> %ebx
        jo      .L_NCR_ERROR            # Overflow
        movl    -8(%ebp), %eax          # n! -> %eax
        movl    $0, %edx		# Move 0 into %edx so we can perform EDX:EDA / EBX
        divl    %ebx			# EDX:EDA / EBX
        jo      .L_NCR_ERROR            # Overflow
        jmp .L_NCR_RETURN               # return value
.L_NCR_ERROR:
        movl $0, %eax
        jmp .L_NCR_RETURN
.L_NCR_RETURN:
        movl    %ebp, %esp
        popl    %ebp                    # Pop the base pointer
        ret                             # Return control to the caller
