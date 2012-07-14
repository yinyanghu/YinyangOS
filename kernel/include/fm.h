#ifndef __FM_H__
#define __FM_H__

#define FM_READ_FILE		0xA0
#define FM_READ				0xA1
#define FM_WRITE			0xA2

extern pid_t	FM;

void FileManagement(void);

#endif
