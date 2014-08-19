#ifndef __VMCONTEXT__H
#define __VMCONTEXT__H
#include "vmtype.h"

struct vmcontext {
	char* context_name;
	long context_id;
	u8* text;
	long text_len;
	u8* stack;
	long stack_len;
	long sp;
	long ip;
	long bp;
};



#endif