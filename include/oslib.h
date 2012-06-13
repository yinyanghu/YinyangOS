#define Buffer_Size	200

#define INT80_SCHEDULE		0x0
#define INT80_READ			0x1
#define INT80_WRITE			0x2
#define INT80_SLEEP			0x3
#define INT80_EXIT			0x4
#define INT80_GETPID		0x5
#define INT80_WAITPID		0x6
#define INT80_FORK			0x7

extern int syscall_0();
extern int syscall_1();
extern int syscall_2();
extern int syscall_3();

int read(char *, int);
int write(char *, int);
void exit(int);
void sleep(int);
int getpid(void);
int waitpid(int);
int fork(void);


int strlen(char *);
void printf(const char *, ...);

