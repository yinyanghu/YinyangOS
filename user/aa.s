	.file	"aa.c"
	.globl	global
	.bss
	.align 4
	.type	global, @object
	.size	global, 4
global:
	.zero	4
	.section	.rodata
.LC0:
	.string	"Hello Yinyanghu!\n"
.LC1:
	.string	"First PID = %d\n"
.LC2:
	.string	"PID @_@ = %d\n"
.LC3:
	.string	"%d %d\n"
	.align 4
.LC4:
	.string	"Child Process, PID = %d, %d, fork() = %d\n"
	.align 4
.LC5:
	.string	"fork() = %d, Child Process, PID = %d, %d\n"
	.align 4
.LC6:
	.string	"Father Process, PID = %d, %d, fork() = %d\n"
	.align 4
.LC7:
	.string	"fork() = %d, Father Process, PID = %d, %d\n"
.LC8:
	.string	"Exiting........\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	andl	$-16, %esp
	subl	$32, %esp
	movl	$.LC0, (%esp)
	.cfi_offset 3, -12
	call	printf
	call	getpid
	movl	%eax, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	call	fork
	movl	%eax, global
	movl	global, %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	movl	$213123, 28(%esp)
	movl	global, %eax
	testl	%eax, %eax
	jne	.L2
	call	getpid
	movl	global, %edx
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$.LC3, (%esp)
	call	printf
	movl	global, %ebx
	call	getpid
	movl	%ebx, 12(%esp)
	movl	28(%esp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC4, (%esp)
	call	printf
	call	getpid
	movl	global, %edx
	movl	28(%esp), %ecx
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$.LC5, (%esp)
	call	printf
	jmp	.L3
.L2:
	call	getpid
	movl	global, %edx
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$.LC3, (%esp)
	call	printf
	movl	global, %ebx
	call	getpid
	movl	%ebx, 12(%esp)
	movl	28(%esp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$.LC6, (%esp)
	call	printf
	call	getpid
	movl	global, %edx
	movl	28(%esp), %ecx
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$.LC7, (%esp)
	call	printf
.L3:
	movl	$.LC8, (%esp)
	call	printf
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	.cfi_restore 3
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.6.3 20120306 (Red Hat 4.6.3-2)"
	.section	.note.GNU-stack,"",@progbits
