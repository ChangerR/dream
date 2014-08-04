#ifndef __DREAM2_INCLUDE_MAP_H
#define __DREAM2_INCLUDE_MAP_H
#include "d2type.h"
#include "stack.h"

template <class KeyType,class ValueType>
class RBTree {
public:
	template <class KeyType,class ValueType>
	class _rb_tree_node {
	public:
		KeyType key;
		ValueType value;
		_rb_tree_node* parent;
		_rb_tree_node* left_child,* right_child;
		bool isRed;

		_rb_tree_node(const KeyType k, const ValueType v ):key(k),value(v),
			parent(NULL),left_child(NULL),right_child(NULL){}

		_rb_tree_node():parent(NULL),left_child(NULL),right_child(NULL){}

		void setLeftChild(_rb_tree_node* l) {
			left_child = l;
			if(l)
				l->parent = this;
		}

		void setRightChild(_rb_tree_node* r) {
			right_child = r;
			if(r)
				r->parent = this;
		}

		void setParent(_rb_tree_node* p) {
			parent = p;
		}

		void setValue(ValueType v) {
			value = v;
		}

		void setRed() { isRed = true;}

		void setBlack() { isRed = false; }

		ValueType getValue() { return vaule;}

		KeyType getKey() { return key; }

		bool isRoot() { return parent == 0; }

		bool isLeftChild() { return parent&&parent->left_child == this;}

		bool isRightChild() {return parent&&parent->right_child == this; }

		bool isLeaf() { return left_child == 0&& right_child == 0; }

		s32 getLevel() {
			_rb_tree_node* p = parent;
			s32 ret = 1;
			while(p) {
				++ret;
				p = p->parent;
			}
		}
	};

public:
	typedef _rb_tree_node<KeyType,ValueType> Node;

	RBTree():nil(),sSize(0),root(NULL) {nil.isRed = false;}

	~RBTree() { clear();}

	bool insert(KeyType k,ValueType v) {
		Node* newElement = new Node(k,v);
		if(!insert_with_no_balance(newElement))
		{
			delete newElement;
			return false;
		}
		while(!newElement->isRoot()&&newElement->parent->isRed) {
			Node* pUncle = NULL;
			if(newElement->parent->isLeftChild()) {
				pUncle = newElement->parent->parent->right_child;
				if(pUncle&&pUncle->isRed) {
					newElement->parent->isRed = false;
					pUncle->isRed = false;
					pUncle->parent->isRed = true;
					newElement = pUncle->parent;
				}else {
					if(newElement->isRightChild()) {
						newElement = newElement->parent;
						rotateLeft(newElement);
					}
					newElement->parent->isRed = false;
					newElement->parent->parent->isRed = true;
					rotateRight(newElement->parent->parent);
				}
			}else {
				pUncle = newElement->parent->parent->left_child;
				if(pUncle&&pUncle->isRed) {
					newElement->parent->isRed = false;
					pUncle->isRed = false;
					pUncle->parent->isRed = true;
					newElement = pUncle->parent;
				}else {
					if(newElement->isLeftChild()) {
						newElement = newElement->parent;
						rotateRight(newElement);
					}
					newElement->parent->isRed = false;
					newElement->parent->parent->isRed = true;
					rotateLeft(newElement->parent->parent);
				}
			}

		}

		root->isRed = false;
		return true;
	}

	Node* find(const KeyType key) {
		Node* p = root;
		while(p) {
			if(p->key == key)
				break;
			else if(p->key < key)
				p = p->left_child;
			else
				p = p->right_child;
		}
		return p;
	}

	bool remove(Node* pz) {
		if( pz == NULL)
			return false;
		Node* py,*px;
		if(pz->left_child == NULL||pz->right_child == NULL) 
			py = pz;
		else
			py = getMin(pz);

		if(py->left_child)
			px = py->left_child;
		else
			px = py->right_child;

		if(px)
			px->parent = py->parent;
		else 
			nil.parent = py->parent;

		if(py->parent == NULL)
			setRoot(px);
		else if(py->isLeftChild())
			py->parent->left_child = px;
		else
			py->parent->right_child = px;

		if(py != pz) {
			pz->key = py->key; 
			pz->value = py->value;
		}

		if(!py->isRed)
			remove_fixup(px);
		--sSize	;
		delete py;
		return true;
	}

	void remove_fixup(Node* px) {
		if (root == NULL)
			return;
		if(px == NULL)
			px = &nil;
		Node* pBrother;
		while(px != root && !px->isRed) {
			if((px != &nil&&px->isLeftChild())||px->parent->left_child==NULL) {
				pBrother = px->parent->right_child;
				if(pBrother->isRed) {
					//case 1:
					rotateLeft(px->parent);
				}else {
					if(px == &nil || (pBrother->left_child->isRed == false&&pBrother->right_child->isRed == false)) {
						//case 2:
						px = px->parent;
						pBrother->isRed = true;
					} else {
						if(pBrother->left_child->isRed == true) {
							//case 3:
							pBrother->isRed = true;
							pBrother->left_child->isRed = false;
							rotateRight(pBrother);
						}
						//case 4:
						pBrother->isRed = px->parent->isRed;
						pBrother->isRed = false;
						pBrother->right_child->isRed = false;
						rotateLeft(px->parent);
						px = root;
					} 
				}
			} else {
				pBrother = px->parent->left_child;
				if(pBrother->isRed) {
					//case 1:
					rotateRight(px->parent);
				}else{
					if(px == &nil || (pBrother->left_child->isRed == false&&pBrother->right_child->isRed == false)){
						//case 2:
						px = px->parent;
						pBrother->isRed = true;
					}else {
						//case 3:
						if(pBrother->right_child->isRed == true) {
							pBrother->isRed = true;
							pBrother->right_child->isRed = false;
							rotateLeft(pBrother);
						}
						//case 4:
						pBrother->left_child->isRed = false;
						pBrother->isRed = px->parent->isRed;
						px->parent->isRed = true;
						rotateRight(px->parent);
						px = root;
					}
				}
			}
		}
		px->isRed = false;
	}

	Node* getRoot() {
		return root;
	}

	bool empty() {
		return sSize == 0;
	}

	s32 size() {
		return sSize;
	}

	void clear() {
		stack<Node*>* pStack = new stack<Node*>(sSize);
		Node* p = root;
		pStack->push(p);
		while(pStack->empty()) {
			p = pStack->pop();
			if(p->right_child)
				pStack->push(p->right_child);
			if(p->left_child)
				pStack->push(p->left_child);
			delete p;
		}
		sSize = 0;
		root = NULL;
	}

	void setRoot(Node* newElement)  {
		root = newElement;
		if(root) {
			newElement->isRed = false;
			root -> parent = NULL;
		}
	}
	bool insert_with_no_balance(Node* newElement) {
		if(root == NULL)
			setRoot(newElement);
		else {
			Node* p = root;

			while(p) {
				if(p->key == newElement->key) {
					return false;
				}else if (newElement->key < p->key) {
					if(p->left_child == 0) {
						p->setLeftChild(newElement);
						p = NULL;
					}else
						p = p->left_child;

				}else {
					if(p->right_child == 0) {
						p->setRightChild(newElement);
						p = NULL;
					}else 
						p = p->right_child;
				}				
			}			
		}
		++sSize;
		return true;
	}

	void rotateLeft(Node* px) {
		Node* py = px->right_child;

		px->setRightChild(py->left_child);

		if(px->isLeftChild())
			px->parent->setLeftChild(py);
		else if(px->isRightChild())
			px->parent->setRightChild(py);
		else
			setRoot(py);

		py->setLeftChild(px);
	}

	void rotateRight(Node* py) {
		Node* px = py->left_child;

		py->setLeftChild(px->right_child);

		if(py->isLeftChild())
			py->parent->setLeftChild(px);
		else if(py->isRightChild())
			py->parent->setRightChild(px);
		else
			setRoot(px);

		px->setRightChild(py);
	}

	Node* getMin(Node* n) const
	{
		while(n && n->left_child)
			n = n->left_child;
		return n;
	}

	Node* getMax(Node* n) const
	{
		while(n && n->right_child)
			n = n->right_child;
		return n;
	}


private:
	s32 sSize;
	Node* root;
	Node nil;
};

template <class KeyType,class ValueType>
class map {
public:
	map() {
		tree = new RBTree<KeyType,ValueType>();
	}
	~map() {
		delete tree;
	}
private:
	RBTree<KeyType,ValueType>* tree;
};
#endif