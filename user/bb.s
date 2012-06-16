	.file	"bb.c"
	.section	.rodata
.LC0:
	.string	"%d, I"
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
	movl	$2, 28(%esp)
	jmp	.L2
.L3:
	call	fork
.L2:
	cmpl	$0, 28(%esp)
	setne	%al
	subl	$1, 28(%esp)
	testb	%al, %al
	jne	.L3
	call	getpid
	movl	%eax, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
.L4:
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.6.3 20120306 (Red Hat 4.6.3-2)"
	.section	.note.GNU-stack,"",@progbits
