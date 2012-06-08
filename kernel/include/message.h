#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define Max_Message_Pool	128
#define ANY			-1
#define MSG_HARD_INTR	-128
//#define Message_Pool_Full	-1

struct Message {
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
		struct {
			uint_32	file_name;
			void	*buf;
			uint_32 offset, length;
		} fm_msg;
		struct {
			struct PCB	*target_pcb;
			uint_32		start, length;
		} mm_msg;
		struct {
			uint_32		file_name;
		} pm_msg;
	};
};


struct Message_Pool_type {
	struct Message			msg;
	struct Message_Pool_type	*prev, *next;
	boolean				flag;
};


struct Message_Pool_type* Find_Empty_Message(struct Message_Pool_type *Pool);

void send(pid_t pid, struct Message *m);
void receive(pid_t pid, struct Message *m);


#endif
