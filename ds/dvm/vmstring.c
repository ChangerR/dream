#include "vmstring.h"
#include <malloc.h>
#include <string.h>
#define _LOWER(x)	((x) >= 'A' && (x) <= 'Z' ? (x) + 0x20 : (x))
#define _UPPER(x)	((x) >= 'a' && (x) <= 'z' ? (x) - 0x20 : (x))

struct vmstring {
	T* buf;
	u32 allocated;
	u32 used;
	u32 refernceCounted;
};




struct vmstring* alloc_vmstring() {
	struct vmstring* p ;
	p = (struct vmstring*)malloc(sizeof(struct vmstring));
	p->buf = NULL;
	p->allocated = 0;
	p->used = 0;
	p->refernceCounted = 1;
	return p;
}

T* get_vmstringBuf(struct vmstring* d) {
	return d->buf;
}

int vmstring_len(struct vmstring* d) {
	return d->used-1;
}

int init_vmstring_l(struct vmstring* s,long num) {
	T* p;	
	T* p2;
	int negative = 0;
	int i;
	s->allocated = 32;
	s->used = 0;
	s->buf = (T*)malloc(s->allocated);	
	p = s->buf+31;

	if (num < 0)
	{
		num *= -1;
		negative = 1;
	}
	if(num == 0) {
		s->buf[0] = '0';
		s->buf[1] = 0;
		s->used = 2;
		return 1;
	}
	while(num) {
		*(u8*)p = (u8)(num % 10 + '0');
		num /= 10;
		--p;
		++s->used;
	}
	if(negative){
		*p = '-';
		s->used++;
		--p;
	}
	++p;
	p2 = s->buf;
	for( i= 0;i < s->used;i++) {
		*p2++ = *p++;
	}
	*p2 = 0;
	s->used++;
	return s->used - 1;
}

int init_vmstring_ul(struct vmstring* s,unsigned long num) {
	T* p;	
	T* p2;
	int i;
	s->allocated = 32;
	s->used = 0;
	s->buf = (T*)malloc(s->allocated);	
	p = s->buf+31;
	if(num == 0) {
		s->buf[0] = '0';
		s->buf[1] = 0;
		s->used = 2;
		return 1;
	}
	while(num) {
		*(u8*)p = (u8)(num % 10 + '0');
		num /= 10;
		--p;
		++s->used;
	}
	++p;
	p2 = s->buf;
	for(i = 0;i < s->used;i++) {
		*p2++ = *p++;
	}
	*p2 = 0;
	s->used++;
	return s->used - 1;
}

int init_vmstring(struct vmstring* s) {
	s->allocated = 8;
	s->used = 1;
	s->buf = (T*)malloc(s->allocated);
	memset(s->buf,0,s->allocated*sizeof(T));
	return 0;
}

void add_vmstring_reference(struct vmstring* s) {s->refernceCounted++;}

void release_vmstring_refernece(struct vmstring* s) {
	--s->refernceCounted;
	if(s->refernceCounted <= 0) {
		if(s->buf)
			free(s->buf);
		free(s);
	}
}

int assign_vmstring_with_len(struct vmstring* d,struct vmstring* s,int len ) {
	return assign_vmstring_s_with_len(d,s->buf,len);
}

int assign_vmstring_s_with_len(struct vmstring* d,const T* s,int len) {
	const T* p = s;
	d->used = 1;	
	if(p == NULL) {
		return init_vmstring(d);
	}else {
		while(*p++&&len) {
			--len;
			++d->used;
		}
		if(d->allocated < d->used) {
			d->allocated = (d->used+7)&~7;
			if(d->buf)
				free(d->buf);
			d->buf = (T*)malloc(d->allocated);
		}		
		memcpy(d->buf,s,d->used*sizeof(T));
		d->buf[d->used-1] = 0;
	}
	return d->used-1;
}
int assign_vmstring(struct vmstring* d,struct vmstring* s) {
	return assign_vmstring_with_len(d,s,0xffff);
}

int assign_vmstring_s(struct vmstring* d,const T* s) {
	return assign_vmstring_s_with_len(d,s,0xffff);
}

static void  reallocate(struct vmstring* d,u32 cnt) {
	T* p;
	if(cnt < d->allocated)
		return;
	p = (T*)malloc(cnt);
	memcpy(p,d->buf,d->used*sizeof(T));
	d->allocated = cnt;
	free(d->buf);
	d->buf = p;
}

int append_vmstring(struct vmstring* d,struct vmstring*s) {
	return append_vmstring_s(d,s->buf);
}

int append_vmstring_s(struct vmstring* d,const char* s) {
	T* p1 = d->buf + d->used - 1;
	const T* p2 = s;
	s32 cnt = 0;
	while(*p2++)cnt++;
	if(d->used + cnt  > d->allocated) {
		reallocate(d,(d->used + cnt +7)&~7);
		p1 = d->buf + d->used -1;
	}
	d->used += cnt;
	p2 = s;
	while(cnt){
		*p1++ = *p2++;
		cnt--;
	}
	*p1 = 0;	
	return d->used - 1;
}

int add_vmstring(struct vmstring* a,struct vmstring* b,struct vmstring* c) {
	assign_vmstring(c,a);
	return append_vmstring(c,b);
}

int append_vmstring_l(struct vmstring* d,long num) {
	int ret;
	struct vmstring* s = alloc_vmstring();
	init_vmstring_l(s,num);
	ret = append_vmstring(d,s);
	release_vmstring_refernece(s);
	return ret;
}

void trim_vmstring(struct vmstring* d) {
	T* p1 = d->buf;
	const char* whitespace = "\t\n\r ";
	const char * p2 = whitespace;
	int isSkip;
	T* p = d->buf;
	if(d->used<=1)
		return;	
	while(*p) {
		isSkip = 0;
		p2 = whitespace;
		while(*p2) {
			if(*p2++ == *p) {
				isSkip = 1;
				break;
			}
		}
		if(isSkip) {
			p++;
			d->used--;
		}else
			break;
	}
	while(*p)
		*p1++ = *p++;
	*p1 = 0;
	p = d->buf + d->used -2;
	while(p >= d->buf) {
		isSkip = 0;
		p2 = whitespace;
		while(*p2) {
			if(*p2++ == *p){
				isSkip = 1;
				break;
			}
		}
		if(isSkip) {
			--p;
			d->used--;
		}else 
			break;
	}
	*++p = 0;
}

void replace_vmstring_s(struct vmstring* d,const T* toReplace,const T* replaceWith) {
	const T* p1 = toReplace;
	const T* p2 = replaceWith;
	T* p = d->buf;
	T* p3;
	s32 cnt1 = 0,cnt2 = 0;
	s32 diff;
	s32 count;
	T* newbuf;
	T* p4;
	while(*p1) {
		cnt1++;
		p1++;
	}
	while(*p2) {
		cnt2++;
		p2++;
	}
	diff = cnt1 - cnt2;
	if(diff == 0) {
		while(*p) {
			p1 = toReplace;
			p3 = p;
			while(*p&&*p1&&*p++ == *p1++);
			if(!*p1) {
				p2 = replaceWith;
				while(*p2)
					*p3++ = *p2++;
			}else
				p = p3+1;
			p1 = toReplace;
		}		
	} else if(diff > 0) {
		while(*p) {
			p1 = toReplace;
			p3 = p;
			while(*p&&*p1&&*p1++ == *p++);
			if(!*p1) {
				p2 = replaceWith;
				while(*p2) {
					*p3++ = *p2++;
				}
				p2 = p3;
				while(*p) {
					*p3++ = *p++;
				}
				*p3 = 0;
				p = (T*)p2;
				d->used -= diff;
			} else
				p = p3 + 1;
		}
	} else {
		count = 0;
		while(*p) {
			p3 = p;
			p1 = toReplace;
			while(*p&&*p1&&*p++==*p1++);
			if(*p1==0) {
				count++;
			}else
				p = p3+1;
		}
		d->allocated = (d->used - diff*count + 7)&~7;
		newbuf = (T*)malloc(d->allocated);
		p = d->buf;
		p4 = newbuf;
		while(*p) {
			p3 = p;
			p1 = toReplace;
			while(*p&&*p1&&*p++==*p1++);
			if(*p1 == 0) {
				p2 = replaceWith;
				while(*p2)*p4++ = *p2++;
			} else {
				*p4++ = *p3++;
				p = p3;
			}
		}
		*p4 = 0;
		free(d->buf);
		d->buf = newbuf;
		d->used -= diff*count;
	}
}

void replace_vmstring(struct vmstring* d,struct vmstring* toReplace,struct vmstring* replaceWith) {
	replace_vmstring_s(d,toReplace->buf,replaceWith->buf);
}

int cmp_vmstring(struct vmstring* d,struct vmstring* s) {
	const T* p1 = d->buf,*p2 = s->buf;
	int diff;
	while(*p1&&*p2) {
		if(_LOWER(*p1) != _LOWER(*p2))
			break;
		p1++;
		p2++;
	}
	diff = _LOWER(*p1) - _LOWER(*p2);
	diff = (diff == 0?0:(diff > 0?1:-1));
	return diff;
}

void to_lower_case(struct vmstring* d) {
	T* p = d->buf;
	if(p) {
		while(*p) {
			*p = _LOWER(*p);
			p++;
		}
	}
}

void to_upper_case(struct vmstring* d) {
	T* p = d->buf;
	if(p) {
		while(*p) {
			*p = _UPPER(*p);
			p++;
		}
	}
}