#ifndef __DREAM2_EVENT_INCLUDE_
#define __DREAM2_EVENT_INCLUDE_
#include "dstring.h"
#include "INode.h"
class IEvent : public IReferenceCounted {
public:
	//TYPE
	enum class TYPE {
		TOUCH,
        KEYBOARD,
        ACCELERATION,
        MOUSE,
        FOCUS,
        GAME_CONTROLLER,
        CUSTOM
	};
	//constructor
	IEvent(TYPE t):_type(t){}

	virtual ~IEvent() {}

	/** Gets the event type */
	inline Type getType() const { return _type; };
    
    /** Stops propagation for current event */
    inline void stopPropagation() { _isStopped = true; };
    
    /** Checks whether the event has been stopped */
    inline bool isStopped() const { return _isStopped; };
    
    /** @brief Gets current target of the event
     *  @return The target with which the event associates.
     *  @note It onlys be available when the event listener is associated with node. 
     *        It returns 0 when the listener is associated with fixed priority.
     */
    inline Node* getCurrentTarget() { return _currentTarget; };

protected:
	/** Sets current target */
    inline void setCurrentTarget(Node* target) { _currentTarget = target; };

	INode* _currentTarget;				
	TYPE _type;
	bool _isStopped;
};
#endif
