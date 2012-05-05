#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define Max_Message_Pool	128
#define ANY			-1
#define PID_Hardware_IRQ	-128
//#define Message_Pool_Full	-1

struct Message_type {
	pid_t	src, dest;
	uint_32	type;
	union {
		struct {
			int_32	p1, p2, p3;
		} int_msg;
		struct {
			pid_t	pid;
			void	*buf;
			uint_32	offset, length;
		} dev_io;

	};
};


struct Message_Pool_type {
	struct Message_type		msg;
	struct Message_Pool_type	*prev, *next;
	boolean				flag;
};


struct Message_Pool_type* Find_Empty_Message(struct Message_Pool_type *Pool);

void send(pid_t pid, struct Message_type *m);
void receive(pid_t pid, struct Message_type *m);


#endif
