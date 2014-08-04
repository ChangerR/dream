#ifndef __DREAM_REFERENCECOUNTED
#define __DREAM_REFERENCECOUNTED
class IReferenceCounted {
public:
	IReferenceCounted():referenceCounted(1){}

	void addRef(){referenceCounted++;}

	void releaseRef() {
		referenceCounted--;
		if(referenceCounted <= 0)
			delete this;
	}
private:
	unsigned long referenceCounted;	
};
#endif
