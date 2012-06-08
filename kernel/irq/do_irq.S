.globl vec0; vec0: pushl $0; jmp asm_do_irq
.globl vec1; vec1: pushl $1; jmp asm_do_irq
.globl vec2; vec2: pushl $2; jmp asm_do_irq
.globl vec3; vec3: pushl $3; jmp asm_do_irq
.globl vec4; vec4: pushl $4; jmp asm_do_irq
.globl vec5; vec5: pushl $5; jmp asm_do_irq
.globl vec6; vec6: pushl $6; jmp asm_do_irq
.globl vec7; vec7: pushl $7; jmp asm_do_irq
.globl vec8; vec8: pushl $8; jmp asm_do_irq
.globl vec9; vec9: pushl $9; jmp asm_do_irq
.globl vec10; vec10: pushl $10; jmp asm_do_irq
.globl vec11; vec11: pushl $11; jmp asm_do_irq
.globl vec12; vec12: pushl $12; jmp asm_do_irq
.globl vec13; vec13: pushl $13; jmp asm_do_irq
.globl vec14; vec14: pushl $14; jmp asm_do_irq

.globl vecsys; vecsys: push $128; jmp asm_do_int

.globl irq0; irq0: pushl $1000; jmp asm_do_irq
.globl irq1; irq1: pushl $1001; jmp asm_do_irq
.globl irq14; irq14: pushl $1014; jmp asm_do_irq

.globl irq_empty
irq_empty: pushl $-1; jmp asm_do_irq

# current_pcb is the memory address of variable current_pcb
.extern current_pcb
.extern enter_interrupt

.globl asm_do_irq
.globl asm_do_int
.extern irq_handle
.extern int_handle
.extern schedule

asm_do_irq:
	
	movl $1, enter_interrupt
	pushal

	movl %esp, %ebx
	cmpl $0xC0000000, %esp

	jae irq_in_kernel

#switch to kernel stack
	movl (current_pcb), %eax
	movl 4(%eax), %esp

irq_in_kernel:

	pushl %ebx
	call irq_handle

	movl (current_pcb), %eax
	movl %esp, (%eax)

	call schedule

	movl (current_pcb), %eax
	movl (%eax), %esp
	popl %esp

	popal
	addl $4, %esp

	movl $0, enter_interrupt 
	iret

asm_do_int:

	pushal

	movl %esp, %ebx
	cmpl $0xC0000000, %esp

	jae int_in_kernel

#switch to kernel stack
	movl (current_pcb), %eax
	movl 4(%eax), %esp

int_in_kernel:

	pushl %ebx
	call int_handle
	
	movl (current_pcb), %eax
	movl %esp, (%eax)

	call schedule

	movl (current_pcb), %eax
	movl (%eax), %esp
	popl %esp

	popal
	addl $4, %esp

	iret


# YOU NEED TO SWITCH STACK TO current_pcb->esp
# SO YOU NEED TWO ADD TWO LINES OF INTERRUPT CODE
# HINT:
#     1. USE movl INSTRUCTION
#     2. USE (address) CAN REFERENCE MEMORY LOCATION OF address
#     3. YOU MAY FLUSH ANY GENRAL PURPOSE REGISTER AS YOU WISH
#     4. REGISTERS ARE REFERENCED BY "%", SUCH AS %esp
	#addl $4, %esp # when you finish the task, this should be removed


