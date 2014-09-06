#ifndef _DREAM_EventLISTENERMOUSE_H
#define _DREAM_EventLISTENERMOUSE_H
#include "IEventListener.h"
#include "EventMouse.h"
class EventListenerMouse : public IEventListener
{
public:
    static const stringc LISTENER_ID;
    
    static EventListenerMouse* create();

    /// Overrides
    virtual EventListenerMouse* clone() override {
		EventListenerMouse* ret = new EventListenerMouse();
		if (ret && ret->init())
		{
			ret->onMouseUp = onMouseUp;
			ret->onMouseDown = onMouseDown;
			ret->onMouseMove = onMouseMove;
			ret->onMouseScroll = onMouseScroll;
		}
		return ret;
	}
	
    virtual bool checkAvailable() override {
		return true;
	}

    std::function<void(Event* event)> onMouseDown;
    std::function<void(Event* event)> onMouseUp;
    std::function<void(Event* event)> onMouseMove;
    std::function<void(Event* event)> onMouseScroll;

private:
    EventListenerMouse(): onMouseDown(nullptr)
		, onMouseUp(nullptr)
		, onMouseMove(nullptr)
		, onMouseScroll(nullptr)
		{
		}
		
    bool init()
	{
		auto listener = [this](Event* event){
			auto mouseEvent = static_cast<EventMouse*>(event);
			switch (mouseEvent->_mouseEventType)
			{
				case EventMouse::MouseEventType::MOUSE_DOWN:
					if(onMouseDown != nullptr)
						onMouseDown(event);
					break;
				case EventMouse::MouseEventType::MOUSE_UP:
					if(onMouseUp != nullptr)
						onMouseUp(event);
					break;
				case EventMouse::MouseEventType::MOUSE_MOVE:
					if(onMouseMove != nullptr)
						onMouseMove(event);
					break;
				case EventMouse::MouseEventType::MOUSE_SCROLL:
					if(onMouseScroll != nullptr)
						onMouseScroll(event);
					break;
				default:
					break;
			}
		};

		if (EventListener::init(Type::MOUSE, LISTENER_ID, listener))
		{
			return true;
		}

		return false;
	}
};

#endif