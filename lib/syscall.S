.globl	syscall_0
.globl	syscall_1
.globl	syscall_2
.globl	syscall_3

syscall_0:

	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp), %eax

	popl	%ebp

	int		$0x80

	ret

syscall_1:

	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp), %eax
	movl	12(%ebp), %ebx
	
	popl	%ebp

	int		$0x80

	ret

syscall_2:

	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp), %eax
	movl	12(%ebp), %ebx
	movl	16(%ebp), %ecx

	popl	%ebp

	int		$0x80

	ret

syscall_3:

	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp), %eax
	movl	12(%ebp), %ebx
	movl	16(%ebp), %ecx
	movl	20(%ebp), %edx

	int		$0x80

	popl	%ebp

	ret

