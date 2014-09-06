#ifndef __DREAM_EVENTTOUCH_H
#define __DREAM_EVENTTOUCH_H
#include "INode.h"
#include "irrArray.h"
#define TOUCH_PERF_DEBUG 1

class IEventTouch : public IEvent
{
public:
    static const int MAX_TOUCHES = 5;
    
    enum class EventCode
    {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED
    };

    IEventTouch();

    inline EventCode getEventCode() const { return _eventCode; };
    inline const array<Touch*>& getTouches() const { return _touches; };

#if TOUCH_PERF_DEBUG
    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    void setTouches(const array<Touch*>& touches) { _touches = touches; };
#endif
    
private:
    EventCode _eventCode;
    array<Touch*> _touches;

    friend class GLViewProtocol;
};

#endif