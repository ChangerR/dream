#ifndef __DREAM2STRING
#define __DREAM2STRING
#include "dtype.h"
#include "irrAllocator.h"
#include <string.h>
enum CHARSET {
	ASCII = 0,
	UNICODE = 1,
	UTF_8 = 2
};

//! Returns a character converted to lower case
static inline u32 _lower ( u32 x )
{
	return x >= 'A' && x <= 'Z' ? x + 0x20 : x;
}

//! Returns a character converted to upper case
static inline u32 _upper ( u32 x )
{
	return x >= 'a' && x <= 'z' ? x + ( 'A' - 'a' ) : x;
}

template <class T,class TALLOC = irrAllocator<T> >
class dstring {
public:
	dstring(){
		allocated = 8;
		used = 1;
		buf = alloc.allocate(allocated);
		memset(buf,0,allocated*sizeof(T));
	}

	dstring(const T* s,u32 len=0xffffffff) {
		used = 1;
		const T* p = s;
		if(p == NULL) {
			dstring();
		}else {
			while(*p++&&len) {
				--len;
				++used;
			}
			allocated = (used+7)&~7;
			buf = alloc.allocate(allocated);
			memcpy(buf,s,used*sizeof(T));
			buf[used-1] = 0;
		}
	}
	
	virtual ~dstring() {
		alloc.deallocate(buf);
	}
	
	dstring(s32 num) {
		allocated = 16;
		used = 0;
		buf = alloc.allocate(allocated);
		T* p = buf+15;
		bool negative = false;
		if (num < 0)
		{
			num *= -1;
			negative = true;
		}
		if(num == 0) {
			buf[0] = '0';
			buf[1] = 0;
			used = 2;
			return;
		}
		while(num) {
			*(u8*)p = (u8)(num % 10 + '0');
			num /= 10;
			--p;
			++used;
		}
		if(negative){
			*p = '-';
			used++;
			--p;
		}
		++p;
		T* p2 = buf;
		for(int i = 0;i < used;i++) {
			*p2++ = *p++;
		}
		*p2 = 0;
		used++;
	}
	
	dstring(u32 num) {
		allocated = 16;
		used = 0;
		buf = alloc.allocate(allocated);
		T* p = buf+15;
		while(num) {
			*(u8*)p = (u8)(num % 10 + '0');
			num /= 10;
			--p;
			++used;
		}
		++p;
		T* p2 = buf;
		for(int i = 0;i < used;i++) {
			*p2++ = *p++;
		}
		*p2 = 0;
		used++;
	}
	
	dstring(const dstring<T,TALLOC>& s){
		allocated = s.allocated;
		used = s.used;
		buf = alloc.allocate(allocated);
		memcpy(buf,s.buf,allocated*sizeof(T));
	}
	
	//! Constructor for copying a string from a pointer with a given length
	template <class B>
	dstring(const B* const c, u32 length)
	: buf(0), allocated(0), used(0)
	{
		if (!c)
		{
			// correctly init the string to an empty one
			*this="";
			return;
		}

		allocated = used = length+1;
		buf = allocator.allocate(used); // new T[used];

		for (u32 l = 0; l<length; ++l)
			buf[l] = (T)c[l];

		buf[length] = 0;
	}


	//! Constructor for unicode and ascii strings
	template <class B>
	dstring(const B* const c)
	: buf(0), allocated(0), used(0)
	{
		*this = c;
	}
	
	dstring<T,TALLOC>& operator = (const dstring<T,TALLOC>& s) {
		if(allocated < s.used) {
			alloc.deallocate(buf);
			buf = alloc.allocate(s.allocated);
			allocated = s.allocated;
		}
		memcpy(buf,s.buf,s.used*sizeof(T));
		used = s.used;
		return *this;
	}
	
	dstring<T,TALLOC>& operator = (const T* s) {
		if(s == NULL) {
			buf[0] = 0;
			used = 1;
			return *this;
		}
		const T* p = s;
		s32 len = 0;
		while(*p++)len++;
		len++;
		if(allocated < len) {
			alloc.deallocate(buf);
			allocated = (len+7)&~7;
			buf = alloc.allocate(allocated);
		}
		memcpy(buf,s,len*sizeof(T));
		used = len;
		return *this;
	}
	
	//! Assignment operator for other string types
	template <class B, class A>
	dstring<T,TAlloc>& operator=(const dstring<B,A>& other)
	{
		*this = other.c_str();
		return *this;
	}
	
	//! Assignment operator for strings, ascii and unicode
	template <class B>
	dstring<T,TAlloc>& operator=(const B* const c)
	{
		if (!c)
		{
			if (!buf)
			{
				buf = allocator.allocate(1); //new T[1];
				allocated = 1;
			}
			used = 1;
			buf[0] = 0x0;
			return *this;
		}

		if ((void*)c == (void*)buf)
			return *this;

		u32 len = 0;
		const B* p = c;
		do
		{
			++len;
		} while(*p++);

		// we'll keep the old string for a while, because the new
		// string could be a part of the current string.
		T* oldbuf = buf;

		used = len;
		if (used>allocated)
		{
			allocated = used;
			buf = allocator.allocate(used); //new T[used];
		}

		for (u32 l = 0; l<len; ++l)
			buf[l] = (T)c[l];

		if (oldbuf != buf)
			allocator.deallocate(oldbuf); // delete [] oldbuf;

		return *this;
	}
	
	bool operator == (const T* s) const {
		const T* p1 = buf,*p2 = s;
		while(*p1&&*p2&&*p1 == *p2) {
			p1++;
			p2++;
		}
		return !((*p1)||(*p2));
	}
	
	bool operator == (const dstring<T,TALLOC>& s) const{
		return *this == s.buf;
	}
	
	bool operator != (const T* s) const {
		return !(*this == s);
	}
	bool operator != (dstring<T,TALLOC>& s) const{
		return !(*this == s.buf);
	}
	
	u32 size() const {
		return used -1;
	}
	
	bool empty() const {
		return used <= 1;
	}
	
	dstring<T,TALLOC>& make_lower() {
		T* p = buf;
		while(*p) {
			*p = (*p <= 'Z'&& *p >= 'A') ?( *p - 'A' + 'a'):*p;
			p++;
		}
		return *this;
	}	
	dstring <T,TALLOC>& make_upper() {
		T* p = buf;
		while(*p) {
			*p = (*p <= 'z'&& *p >= 'a') ?( *p - 'a' + 'A'):*p;
			p++;
		}
		return *this;
	}
	
	bool equals_ignore_case(const dstring<T,TALLOC>& s) const{
		const T* p1 = buf,*p2 = s.buf;
		while(*p1&&*p2) {
			if(_lower(*p1) != _lower(*p2))
				break;
			p1++;
			p2++;
		}
		return !((*p1)||(*p2));
	}
	bool equals_ignore_case(const T* s) const{
		const T* p1 = buf,*p2 = s;
		while(*p1&&*p2) {
			if(_lower(*p1) != _lower(*p2))
				break;
			p1++;
			p2++;
		}
		return !((*p1)||(*p2));
	}
	
	bool equals_substring_ignore_case(const dstring<T,TALLOC>& s,s32 pos) const {
		if(pos >= used-1)
			return false;
		const T* p1 = buf + pos,*p2 = s.buf;
		while(*p1&&*p2) {
			if(_lower(*p1) != _lower(*p2))
				break;
			p1++;
			p2++;
		}
		return !((*p1)||(*p2));
	}

	bool equals_substring_ignore_case(const T* s,s32 pos) const {
		if(pos >= used-1)
			return false;
		const T* p1 = buf + pos;
		const T* p2 = s;
		while(*p1&&*p2) {
			if(_lower(*p1) != _lower(*p2))
				break;
			p1++;
			p2++;
		}
		return !((*p1)||(*p2));
	}

	bool lower_ignore_case(const dstring<T,TALLOC>& s) const {
		const T* p1 = buf,*p2 = s.buf;
		bool ret = false;
		s32 diff = 0;
		while(*p1&&*p2) {
			diff = _lower(*p1) - _lower(*p2);
			if(diff)
				break;
			p1++;
			p2++;
		}
		if(!diff)
			ret =  (used < s.used);
		else
			ret = (diff < 0);
		return ret;
	}
	
	bool operator < (const dstring<T,TALLOC>& s) const {
		const T* p1 = buf,*p2 = s.buf;
		bool ret = false;
		s32 diff = 0;
		while(*p1&&*p2) {
			diff = *p1 - *p2;
			if(diff)
				break;
			p1++;
			p2++;
		}
		if(!diff)
			ret =  (used < s.used);
		else
			ret = (diff < 0);
		return ret;
	}
	
	bool equalsn(const dstring<T,TALLOC>& s,u32 n) const {
		const T* p1 = buf,*p2 = s.buf;
		while(n&&*p1&&*p2) {
			if(*p1 != *p2)
				break;
			--n;
			++p1;
			++p2;
		}
		return n == 0;
	}
	
	bool equalsn(const T* s,u32 n) {
		const T* p1 = buf,*p2 = s;
		while(n&&*p1&&*p2) {
			if(*p1 != *p2)
				break;
			--n;
			++p1;
			++p2;
		}
		return n == 0;
	}
	
	dstring<T,TALLOC>& append(T c) {
		if(used == allocated)
			reallocate(allocated + 8);
		*(buf + used - 1) = c;
		*(buf + used) = 0;
		used ++;
		return *this;
	}
	
	dstring<T,TALLOC>& append(const T* s) {
		T* p1 = buf + used - 1;
		const T* p2 = s;
		s32 cnt = 0;
		while(*p2++)cnt++;
		if(used + cnt  > allocated) {
			reallocate((used + cnt +7)&~7);
			p1 = buf + used -1;
		}
		used += cnt;
		p2 = s;
		while(cnt){
			*p1++ = *p2++;
			cnt--;
		}
		*p1 = 0;	
		return *this;
	}
	
	dstring<T,TALLOC>& append(const T* s,u32 cnt) {
		T* p1 = buf + used - 1;
		const T* p2 = s;
		s32 cnt2 = 0;
		while(cnt&&*p2++) {
			cnt2++;
			cnt--;
		}
		if(used + cnt2 > allocated) {
			reallocate((used + cnt2 +7)&~7);
			p1 = buf + used -1;
		}
		used += cnt2;
		p2 = s;
		while(cnt2) {
			*p1++ = *p2++;
			cnt2--;
		}
		*p1 = 0;
		return *this;
	}
	
	dstring<T,TALLOC>& append(const dstring<T,TALLOC>& s) {
		this->append(s.buf);
		return *this;
	}
	
	dstring<T,TALLOC>& append(const dstring<T,TALLOC>& s,u32 len) {
		this->append(s.buf,len);
		return *this;
	}
	
	void reserve(u32 count) {
		if(count > allocated)
			reallocate(count);
	}
	
	//! gets the last char of a string or null
	T lastChar() const
	{
		return used > 1 ? buf[used-2] : 0;
	}
	
	s32 findFirst(const T c) const{
		s32 index = 0;
		const T* p = buf;
		while(*p) {
			if(*p == c)
				return index;
			p++;
			index++;
		}
		return -1;
	}
	
	s32 findLast(const T c) const {
		if(used <= 1) 
			return -1;
		const T* p = buf + used -2;
		s32 index = used -2;
		while(p >= buf) {
			if(*p == c) 
				return index;
			--p;
			--index;
		}
		return -1;
	}
	
	s32 findNext(const T c,u32 pos) const{
		for(u32 i = pos; i < used-1;i++) {
			if(buf[i] == c)
				return i;
		}
		return -1;
	}

	s32 findFirstChar(const T* s) const {
		s32 index = 0;
		const T* p1 = buf,*p2 = s;
		while(*p1) {
			while(*p2) {
				if(*p1 == *p2)
					return index;
				p2++;
			}
			p2 = s;
			p1++;
			index++;
		}
		return -1;
	}
	
	s32 findLastChar(const T* s)const {
		if(used <= 1)
			return -1;
		const T* p1 = buf + used - 2,*p2 = s;
		s32 index = used - 2;
		while( p1 >= buf) {
			while(*p2) {
				if(*p1 == *p2)
					return index;
				p2++;
			}
			--p1;
			--index;
			p2 = s;
		}
		return -1;
	}
	
	s32 findFirstCharNotInList(const T* list) const {
		const T* p1 = buf,*p2 = list;
		s32 index = 0;
		while(*p1) {
			while(*p2&&*p2 != *p1) {
				p2++;
			}
			if(!*p2)
				return index;
			++p1;
			++index;
			p2 = list;
		}
		return -1;
	}
	
	s32 findLastCharNotInList(const T* list) const {
		if(used <= 1)
			return -1;
		const T* p1 = buf + used - 2,*p2 = list;
		s32 index = used - 2;
		while(p1 >= buf) {
			while(*p2&&*p2 != *p1) {
				p2++;
			}
			if(!*p2)
				return index;
			--p1;
			--index;
			p2 = list;
		}
		return -1;
	}
	
	s32 find(const T* s,u32 start = 0) const {
		for(u32 i = start;i < used - 1;i++){
			if(buf[i] == *s ) {
				const T* p = s+1;
				for(u32 j = i + 1;*p&&j < used - 1;j++) {
					if(*p != buf[j])
						break;
					p++;
				}
				if(!*p)
					return i;
			}
		}
		return -1;
	}
	
	dstring<T,TALLOC> subString(u32 pos,u32 end,bool _lower = false) const {
		dstring<T,TALLOC> o(buf+pos,end - pos);
		if(_lower)
			o.make_lower();
		return o;
	}
	
	const dstring<T,TALLOC>& operator + (const dstring<T,TALLOC>& s) const {
		dstring<T,TALLOC> tmp(*this);
		tmp.append(s);
		return tmp;
	}
	
	const dstring<T,TALLOC> operator + (const T* s) const {
		dstring<T,TALLOC> tmp(*this);
		tmp.append(s);
		return tmp;
	}
	
	dstring<T,TALLOC>& operator += (const dstring<T,TALLOC>& s) {
		this->append(s.buf);
		return *this;
	}
	
	dstring<T,TALLOC>& operator +=(const T* s) {
		this->append(s);
		return *this;
	}

	dstring<T,TALLOC>& operator += (const T c) {
		this->append(c);
		return *this;
	}
	
	dstring<T,TALLOC>& operator += (const s32 num) {
		dstring<T,TALLOC> n(num);
		this->append(n);
		return *this;
	}
	
	dstring<T,TALLOC>& operator += (const u32 num) {
		dstring<T,TALLOC> n(num);
		this->append(n);
		return *this;
	}
	
	T& operator [](const s32 i) const {
		if(i < 0||i > used -1) {
			return buf[used-1];
		}
		return buf[i];
	}
	
	void validate() {
		const T* p = buf;
		used = 1;
		while(*p && used < allocated) {
			p++;
			used++;
		}
	}
	
	dstring<T,TALLOC>& replace(const T toReplace,const T replaceWith) {
		T* p = buf;
		while(*p) {
			if(*p == toReplace) {
				*p = replaceWith;
			}
			p++;
		}
		return *this;
	}

	dstring<T,TALLOC>& replace(const T* toReplace,const T* replaceWith) {
		const T* p1 = toReplace;
		const T* p2 = replaceWith;
		T* p = buf;
		T* p3;
		s32 cnt1 = 0,cnt2 = 0;
		while(*p1) {
			cnt1++;
			p1++;
		}
		while(*p2) {
			cnt2++;
			p2++;
		}
		s32 diff = cnt1 - cnt2;
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
					p = const_cast<T*>(p2);
					used -= diff;
				} else
					p = p3 + 1;
			}
		} else {
			s32 count = 0;
			while(*p) {
				p3 = p;
				p1 = toReplace;
				while(*p&&*p1&&*p++==*p1++);
				if(*p1==0) {
					count++;
				}else
					p = p3+1;
			}
			allocated = (used - diff*count + 7)&~7;
			T* newbuf = alloc.allocate(allocated);
			p = buf;
			T* p4 = newbuf;
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
			alloc.deallocate(buf);
			buf = newbuf;
			used -= diff*count;
		}
		return *this;
	}
	
	dstring<T,TALLOC>& replace(const dstring<T,TALLOC>& toReplace,const dstring<T,TALLOC>& replaceWith) {
		return this->replace(toReplace.buf,replaceWith.buf);
	}

	dstring<T,TALLOC>& remove(const T* s) {
		T* p1 = buf;
		const T *p2 = s,*p = s;
		s32 cnt = 0;
		while(*p++)cnt++;
		while(*p1) {
			p = p1;
			p2 = s;
			while(*p&&*p2&&*p++ == *p2++);
			if(*p2 == 0) {
				p2 = p1;
				while(*p)*p1++ = *p++;
				*p1 = 0;
				used-=cnt;
				p1 = const_cast<T*>(p2);
			}else
				p1 ++;
		}
		return *this;	
	}

	dstring<T,TALLOC>& remove(const T c) {
		T* p = buf;
		const T* p1;
		while(*p) {
			if(*p == c) {
				p1 = p;
				p++;
				while(*p) {
					*(p - 1) = *p;
					p++;
				}
				*(p-1) = 0;
				p = const_cast<T*>(p1);
				used--;
			}else
				p++;
		}
		return *this;		
	}
	
	dstring<T,TALLOC>& remove(const dstring<T,TALLOC>& s) {
		return this->remove(s.buf);
	}

	dstring<T,TALLOC>& removeChars(const T* s) {
		T* p = buf;
		T* p1;
		const T* p2 = s;
		bool replace;
		while(*p) {
			replace = false;
			p2 = s;
			while(*p2) {
				if(*p2++ == *p) {
					replace = true;
					break;
				}
			}
			if(replace) {
				p1 = p;
				p1++;
				while(*p1) {
					*(p1 -1) = *p1;
					p1++;
				}
				*(p1-1) = 0;
				used--;
			}else
				p++;
		}
		return *this;
	}

	dstring<T,TALLOC>& trim(const char * whitespace = "\t\n\r ") {
		if(used<=1)
			return *this;
		T* p1 = buf;
		const char * p2 = whitespace;
		bool isSkip;
		T* p = buf;
		while(*p) {
			isSkip = false;
			p2 = whitespace;
			while(*p2) {
				if(*p2++ == *p) {
					isSkip = true;
					break;
				}
			}
			if(isSkip) {
				p++;
				used--;
			}else
				break;
		}
		while(*p)
			*p1++ = *p++;
		*p1 = 0;
		p = buf + used -2;
		while(p >= buf) {
			isSkip = false;
			p2 = whitespace;
			while(*p2) {
				if(*p2++ == *p){
					isSkip = true;
					break;
				}
			}
			if(isSkip) {
				--p;
				used--;
			}else 
				break;
		}
		*++p = 0;
		return *this;
	}

	const T* c_str() const {
		return buf;
	}
	
	void copy_string_to_buf(T* s,s32 len) {
		if(len > used ) 
			len = used;
		memcpy(s,buf,len*sizeof(T));
		buf[len - 1] = 0;
	}
private:
	void reallocate(u32 cnt) {
		if(cnt < allocated)
			return;
		T* p = alloc.allocate(cnt);
		memcpy(p,buf,used*sizeof(T));
		allocated = cnt;
		alloc.deallocate(buf);
		buf = p;
	}
	T* buf;
	u32 allocated;
	u32 used;
	TALLOC alloc;
};

typedef dstring< char,irrAllocator<char> > stringc;
typedef dstring< wchar_t,irrAllocator<wchar_t> > stringw;

template <class TALLOC>
class StringUTF8{


};

#endif
