#ifndef __DREAM_EVENTTOUCH_H
#define __DREAM_EVENTTOUCH_H
#include "Node.h"
#include "irrArray.h"
#define TOUCH_PERF_DEBUG 1

class EventTouch : public Event
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

    EventTouch();

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