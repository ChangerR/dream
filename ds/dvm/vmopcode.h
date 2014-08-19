#include "vmcontext.h"

enum EVM_OPCODE {
#define OPDEF(op,name,func)	op,
#include "vmopcode.tbl"
#undef OPDEF
	VMOP_COUNT
};

//base 
//00 immedite
//01 sp
//02 bp
//03 string
/**************************************************
*1 args: 
*	bit		1	2	3	4		5	6		7	8
*			| 00|   X	|base	|   +/-		|len |
*2 args:
*	bit		1	2	3	4		5	6		7	8
*			| 01|   X	|base 0 |	+/-     |len|
*			X	X	X	|base 1 | 	+/-     |len|
*3 args:
*	bit		1	2	3	4		5	6  		7	8
*			1   |base|	+/-    	|len|  		|base|
*			+/- |len |	|base 2 | 	+/- 	|len |
*
*
***************************************************/
#define VMF_BYTE				0x0
#define VMF_ARGLIST 			0x1
