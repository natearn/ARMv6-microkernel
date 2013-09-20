#include "nameserver.h"
#include "syscall.h"
#include "kernel.h"
#include "string.h"

/* XXX: calling write can block the nameserver, which would be bad for all the other processes */
int name_server_task(void) {
	char names[NUM_STACKS][NAME_SIZE];
	struct NameServerRequest req;
	struct NameServerResponse res;
	size_t amount, i;
	while(1) {
		amount = read(sizeof(req),(char*)&req);
		/* only proceed for "valid" messages */
		if(amount == sizeof(req)) {
			switch(req.type) {
				case REGISTER:
					for(i=0;i<NUM_STACKS;i++) {
						if(!strcmp(req.name,names[i])) {
							res.type = req.type;
							res.status = FAILURE;
							write(req.pid,sizeof(res),(char*)&res);
							break;
						}
					}
					if(i == NUM_STACKS) {
						strlcpy(names[req.pid],req.name,NAME_SIZE);
						res.type = req.type;
						res.status = SUCCESS;
						write(req.pid,sizeof(res),(char*)&res);
					}
					break;
				case LOOKUP:
					for(i=0;i<NUM_STACKS;i++) {
						if(!strcmp(req.name,names[i])) {
							res.type = req.type;
							res.status = SUCCESS;
							res.pid = i;
							write(req.pid,sizeof(res),(char*)&res);
							break;
						}
					}
					if(i == NUM_STACKS) {
						res.type = req.type;
						res.status = FAILURE;
						write(req.pid,sizeof(res),(char*)&res);
					}
					break;
				default:
					res.type = req.type;
					res.status = INVALID;
					write(req.pid,sizeof(res),(char*)&res);
					break;
			}
		}
	}
	return 0;
}
