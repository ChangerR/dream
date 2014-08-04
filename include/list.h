#ifndef _DREAM2_LIST__H
#define _DREAM2_LIST__H

#include "IrrAllocator.h"
template <class T>
class list
{
public:
	struct node {
		node(const T& e):prev(0),next(0),element(e){}
		node* prev;
		node* next;
		T element;
	};
	list():m_size(0){
		head = alloc.allocate(1);
		tail = alloc.allocate(1);
		head->prev = NULL;
		tail->next = NULL;
		head->next = tail;
		tail->prev = head;
	}

	~list() {
		clear();
		alloc.deallocate(head);
		alloc.deallocate(tail);
	}

	u32 getSize() const {
		return m_size;
	}

	void clear() {
		node* p = head->next;
		while (p != tail)
		{	
			head->next = head->next->next;
			alloc.destruct(p);
			alloc.deallocate(p);
			p = head->next;
			--m_size;
		}
	}

	bool empty() {
		return m_size == 0;
	}

	void push_back(T& element) {
		node *p_node = alloc.allocate(1);
		alloc.construct(p_node,element);
		tail->prev->next = p_node;
		p_node->prev = tail->prev;
		tail->prev = p_node;
		p_node->next = tail;
		m_size++;
	}

	void push_front(T& element) {
		node *p_node = alloc.allocate(1);
		alloc.construct(p_node,element);
		p_node->next = head->next;
		head->next->prev = p_node;
		head->next  = p_node;
		p_node->prev = head;
		m_size++;
	}

	void insert(T& element, s32 i) {
		if (i > m_size)
			return;
		node *p_node = alloc.allocate(1);
		alloc.construct(p_node,element);
		
		node* p  = head;
		for (int index = 0; index < i; index++)
			p = p->next;
		p_node->next = p->next;
		p->next->prev = p_node;
		p_node->prev = p;
		p->next = p_node;
		m_size++;
	}

	node* begin() const{
		return head->next;
	}

	node* end() const {
		return tail;
	}

	void erase(s32 i) {
		if (i >= m_size)
			return;
		node* p = head->next;
		for (int index = 0; index < i;index++)
			p = p->next;
		p->prev->next = p->next;
		p->next->prev = p->prev;
		alloc.destruct(p);
		alloc.deallocate(p);
		m_size--;
	}

	void erase(node* p) {
		p->prev->next = p->next;
		p->next->prev = p->prev;
		alloc.destruct(p);
		alloc.deallocate(p);
		m_size--;
	}
private:
	node* head;
	node* tail;
	u32 m_size;
	irrAllocator<node> alloc;
};

#endif