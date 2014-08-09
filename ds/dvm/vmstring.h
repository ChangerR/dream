#ifndef __DREAM_VM_STRING
#define __DREAM_VM_STRING
#include "vmtype.h"

struct vmstring;

T* get_vmstringBuf(struct vmstring* d);

int vmstring_len(struct vmstring* d);

struct vmstring* alloc_vmstring();

int init_vmstring_l(struct vmstring* s,long num);

int init_vmstring_ul(struct vmstring* s,unsigned long num);

int init_vmstring(struct vmstring* s);

void add_vmstring_reference(struct vmstring* s);

void release_vmstring_refernece(struct vmstring* s);

int assign_vmstring_with_len(struct vmstring* d,struct vmstring* s,int len);

int assign_vmstring_s_with_len(struct vmstring* d,const T* s,int len);

int assign_vmstring(struct vmstring* d,struct vmstring* s);

int assign_vmstring_s(struct vmstring* d,const T* s);

int append_vmstring(struct vmstring* d,struct vmstring*s);

int append_vmstring_s(struct vmstring* d,const T* s);

int add_vmstring(struct vmstring* a,struct vmstring* b,struct vmstring* c);

int append_vmstring_l(struct vmstring* d,long num);

void trim_vmstring(struct vmstring* d);

void replace_vmstring_s(struct vmstring* d,const T* toReplace,const T* replaceWith);

void replace_vmstring(struct vmstring* d,struct vmstring* toReplace,struct vmstring* replaceWith);

int cmp_vmstring(struct vmstring* d,struct vmstring* s);

void to_lower_case(struct vmstring* d);

void to_upper_case(struct vmstring* d);

#endif