#ifndef __DREAM_REFERENCECOUNTED
#define __DREAM_REFERENCECOUNTED
class IReferenceCounted {
public:
	IReferenceCounted():referenceCounted(1){}
	virtual ~IReferenceCounted() {
	}
	
	virtual unsigned long  addRef(){
		referenceCounted++;
		return referenceCounted;
	}

	virtual bool releaseRef() {
		referenceCounted--;
		if(referenceCounted <= 0) {
			delete this;
			return true;
		}
		return false;
	}
private:
	unsigned long referenceCounted;	
};
#endif
