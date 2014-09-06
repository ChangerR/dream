#ifndef __DREAM_IENENT_CUSTOM_H_
#define __DREAM_IENENT_CUSTOM_H_
#include "IEvent.h"
#include "dstring.h"

class IEventCustom : public IEvent
{
public:
    /** Constructor */
    IEventCustom(const stringc& eventName): Event(Type::CUSTOM)
		, _userData(nullptr)
		, _eventName(eventName)
	{
	}
    /** Sets user data */
    inline void setUserData(void* data) { _userData = data; };
    
    /** Gets user data */
    inline void* getUserData() const { return _userData; };
    
    /** Gets event name */
    inline const stringc& getEventName() const { return _eventName; };
protected:
    void* _userData;       ///< User data
    stringc _eventName;
};

#endif