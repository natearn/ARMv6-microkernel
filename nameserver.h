#ifndef _NAME_SERVER_H_
#define _NAME_SERVER_H_

#define NAME_SIZE 64

/* message types */
#define REGISTER 1
#define LOOKUP   2

/* statuses */
#define SUCCESS 0
#define FAILURE 1
#define INVALID 2

struct NameServerRequest {
	unsigned int type;
	unsigned int pid;
	char name[NAME_SIZE];
};

struct NameServerResponse {
	unsigned int type;
	unsigned int status;
	unsigned int pid;
};

int name_server_task(void);

#endif
