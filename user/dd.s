	.file	"dd.c"
	.section	.rodata
.LC0:
	.string	"Child!\n"
.LC1:
	.string	"%d\n"
.LC2:
	.string	"%d %c\n"
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
	andl	$-16, %esp
	subl	$32, %esp
	movl	$.LC0, (%esp)
	call	printf
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	movl	$0, 28(%esp)
	jmp	.L2
.L3:
	movl	28(%esp), %eax
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 8(%esp)
	movl	28(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	addl	$1, 28(%esp)
.L2:
	movl	28(%esp), %eax
	cmpl	8(%ebp), %eax
	jl	.L3
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.0 20120507 (Red Hat 4.7.0-5)"
	.section	.note.GNU-stack,"",@progbits
