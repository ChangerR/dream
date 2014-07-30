#ifndef __DREAM_ATTRHASHMAP__H
#define __DREAM_ATTRHASHMAP__H
#include "dstring.h"
#include "EAttributes.h"
#include <malloc.h>

#ifdef USE_GNUCX64
#define __ATTR_NODE_BUF 72
#else
#define __ATTR_NODE_BUF 80 
#endif

#ifdef USE_GNUCX64
#define PAGE_COUNT 14
#else
#define PAGE_COUNT 29
#endif
typedef u32 (*hashStrFunc)(const c8*);
typedef struct attr_node_s {
	c8 name[32];
	E_ATTRIBUTE_TYPE attr_type;
	u32 hash_value;
	struct attr_node_s* prev;
	struct attr_node_s* next;
	u8 buf[__ATTR_NODE_BUF];
} attr_node_t;

class AttrNodeAlloc {
public:
	AttrNodeAlloc() {
		init_page();
	}
	virtual ~AttrNodeAlloc() {
		clear_allpage();
	}
	
	attr_node_t* allocate(size_t cnt) {
		attr_node_t* aret;
		if(cnt > sizeof(attr_node_t))
			return NULL;
		if(root->next == NULL) {
			ppNode = &(root->next);
			alloc_page();
		}
		aret = root->next;
		root->next = aret->next;
		memset(aret,0,sizeof(attr_node_t));
		return aret;
	}
	
	void deallocate(attr_node_t* p) {
		if(p == NULL)
			return;
		*ppNode = p;
		ppNode = &(p->next);
		*ppNode = NULL;
	}
	
	void reallocateall(){
		clear_allpage();
		init_page();
	}
private:
	struct page_header {
		struct page_header* next;
		u32 page_count;
		u32 reserve;
		void* page_addr[PAGE_COUNT];
	};
	void add_node(void* addr,size_t len) {
		u32 cnt = len / sizeof(attr_node_t);
		attr_node_t* p = (attr_node_t*)addr;
		for(u32 index = 0;index < cnt;index ++) {
			*ppNode = p;
			ppNode = &(p->next);
			*ppNode = NULL;
			p++;
		}
	}
	void init_page() {
		u8* page = (u8*)malloc(1024*4);
		memset(page,0,1024*4);
		first_page = (struct page_header*)page;
		root = (attr_node_t*)(page + sizeof(page_header));
		ppNode = &(root->next);
		//*********************It's my Fault******************************
		//in the begining use root+sizeof(attr_node_t) 
		add_node(page+sizeof(page_header)+sizeof(attr_node_t),1024*4-sizeof(struct page_header)-sizeof(attr_node_t));
	}

	void alloc_page() {
		struct page_header* p = first_page;
		u8* page = (u8*)malloc(1024*4);
		if(page == NULL) {
			printf("Note : we alloc failed");
		}
		u32 size;
		memset(page,0,1024*4);
		while(p->next)
			p = p->next;
		if(p->page_count == PAGE_COUNT) {
			p->next = (struct page_header*)page;
			size = 1024*4 - sizeof(struct page_header);
			page += sizeof(page_header);
		}else{
			p->page_addr[p->page_count] = page;
			size = 1024*4;
			p->page_count ++;
		}
		add_node(page,size);
	}
	
	void clear_allpage() {
		struct page_header* p = first_page;
		while(p) {
			first_page = first_page->next;
			for(u32 index = 0;index < p->page_count;index++)
				free(p->page_addr[index]);
			free(p);
			p = first_page;
		}
	}
	
	struct page_header* first_page;
	attr_node_t* root;
	attr_node_t** ppNode;
};

class attrHashMap {
public:
	attrHashMap(hashStrFunc f,u32 count = 997) {
		hashf = f;
		basket = (attr_node_t**)malloc(count*sizeof(attr_node_t*));
		memset(basket,0,count*sizeof(attr_node_t*));
		basket_count = count;
		element_count = 0;
	}
	
	~attrHashMap() {
		free(basket);
	}
	
	void check() const{
		for(int i = 0;i < basket_count;i++) {
			if(basket[i]&&basket[i]->hash_value%basket_count != i) {
				printf("error basket %d:%s\n",i,basket[i]->name);
			}
		}
	}

	void add(stringc& str,E_ATTRIBUTE_TYPE t,void* data,s32 size_data) {
		if(str.size() >= 36)
			return;
		str.make_lower();

		attr_node_t* node = find(str);
		if (node != NULL)
		{
			node->attr_type = t;
			if(size_data > __ATTR_NODE_BUF)
				size_data = __ATTR_NODE_BUF;
			if (data != NULL && size_data > 0)
				memcpy(node->buf,data,size_data);
			return;
		}		
		node = alloc.allocate(1);
		str.copy_string_to_buf(node->name,36);
		node->attr_type = t;
		if(size_data > __ATTR_NODE_BUF)
			size_data = __ATTR_NODE_BUF;
		if (data != NULL && size_data > 0)
			memcpy(node->buf,data,size_data);
		node->hash_value = hashf(node->name);
		s32 basketId = node->hash_value % basket_count;
		node->next = NULL;
		if(basket[basketId] == NULL)
			basket[basketId] = node;
		else {
			attr_node_t* p = basket[basketId];
			while(p->next)
				p = p->next;
			p->next = node;
		}
		element_count++;
	}

	void remove(stringc& str) {
		if(str.size() >= 36)
			return;
		str.make_lower();
		u32 hash_value = hashf(str.c_str());
		s32 basketid = hash_value % basket_count; 
		attr_node_t* node = basket[basketid];
		if(node == NULL)
			return;
		else {
			if(node->hash_value == hash_value && str == node->name)
				basket[basketid] = node->next;
			else {
				attr_node_t* p = node ->next;
				while(p) {
					if(p->hash_value == hash_value && str == p->name)
						break;
					node = p;
					p = node->next;
				}
				if(p == NULL)
					return;
				else {
					node->next = p->next;
					node = p;
				}
			}
		}
		alloc.deallocate(node);
		element_count--;	
	}
		
	void add(const stringc& str,E_ATTRIBUTE_TYPE t,void* data,s32 size_data) {
		stringc tmp(str);
		add(tmp,t,data,size_data);
	}

	void remove(const stringc& str) {
		stringc tmp(str);
		remove(tmp);
	}

	s32 getElementbuf(stringc &str,void* buf,s32 len) {
		attr_node_t* node = find(str);
		if (node == NULL)
			return 0;
		memcpy(buf,(void*)node->buf,len > __ATTR_NODE_BUF?__ATTR_NODE_BUF:len);
		return 1;
	}
	
	s32 getElementbuf(const stringc &str,void* buf,s32 len) const {
		stringc tmp(str);
		attr_node_t* node = find(tmp);
		if (node == NULL)
			return 0;
		memcpy(buf,static_cast<void*>(node->buf),len > __ATTR_NODE_BUF?__ATTR_NODE_BUF:len);
		return 1;
	}
	
	E_ATTRIBUTE_TYPE getElementType(stringc &str) {
		attr_node_t* node = find(str);
		if(node == NULL) 
			return EAT_UNKNOWN;
		return node->attr_type;
	}
	
	E_ATTRIBUTE_TYPE getElementType(const stringc &str) const {
		stringc tmp(str);
		attr_node_t* node = find(tmp);
		if(node == NULL) 
			return EAT_UNKNOWN;
		return node->attr_type;
	}
	
	void clear() {
		element_count = 0;
		memset(basket,0,basket_count*sizeof(void*));
		alloc.reallocateall();
	}
	
	s32 size() const {
		return element_count;
	}
	
	bool exist(const stringc& str) {
		stringc tmp(str);
		attr_node_t* node = find(tmp);
		return node == NULL? false:true;
	}
	
	bool exist(stringc & str) {
		attr_node_t* node = find(str);
		return node == NULL? false:true;
	}
private:
	attr_node_t* find(stringc& str) const{
		if(str.size() >= 36)
			return NULL;
		str.make_lower();
		s32 hash_value = hashf(str.c_str());
		s32 basketid = hash_value % basket_count; 
		attr_node_t* node = basket[basketid];
		while(node) {
			if(node->hash_value == hash_value && str == node->name)
				break;
				if(node&&node->hash_value % basket_count!= basketid)
					printf("error occur\n");
			node = node->next;
		}
		return node;
	}
	AttrNodeAlloc alloc;
	u32 basket_count;
	attr_node_t** basket;
	hashStrFunc hashf;
	s32 element_count;
};
#endif
