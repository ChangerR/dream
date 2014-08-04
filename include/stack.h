#ifndef __DREAM2_INCLUDE_STACK
#define __DREAM2_INCLUDE_STACK
#include "dtype.h"
template <class T>
class stack {
public:
	stack(u32 uSize) {
		m_arrayaddr = new T[uSize];
		m_topaddr = m_arrayaddr + uSize;
		pointer = m_topaddr;
	}
	~stack() {
		delete[] m_arrayaddr;
	}
	u32 push(T data){
		if (pointer <= m_arrayaddr)
		{
			return 0;
		}
		pointer --;
		*pointer = data;
		return 1;
	}
	T pop() {
		if (pointer >= m_topaddr)
		{
			return T(0);
		}
		pointer++;
		return *(pointer -1);
	}
	bool empty() {
		return pointer >= m_topaddr;
	}
public :
	T* m_arrayaddr;
	T* m_topaddr;
	T* pointer;
};

#endif