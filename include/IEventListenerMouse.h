#ifndef _DREAM_IEVENTLISTENERMOUSE_H
#define _DREAM_IEVENTLISTENERMOUSE_H
#include "IEventListener.h"
#include "IEventMouse.h"
class IEventListenerMouse : public IEventListener
{
public:
    static const stringc LISTENER_ID;
    
    static IEventListenerMouse* create();

    /// Overrides
    virtual IEventListenerMouse* clone() override {
		IEventListenerMouse* ret = new IEventListenerMouse();
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

    std::function<void(IEvent* event)> onMouseDown;
    std::function<void(IEvent* event)> onMouseUp;
    std::function<void(IEvent* event)> onMouseMove;
    std::function<void(IEvent* event)> onMouseScroll;

private:
    IEventListenerMouse(): onMouseDown(nullptr)
		, onMouseUp(nullptr)
		, onMouseMove(nullptr)
		, onMouseScroll(nullptr)
		{
		}
		
    bool init()
	{
		auto listener = [this](IEvent* event){
			auto mouseEvent = static_cast<IEventMouse*>(event);
			switch (mouseEvent->_mouseEventType)
			{
				case IEventMouse::MouseEventType::MOUSE_DOWN:
					if(onMouseDown != nullptr)
						onMouseDown(event);
					break;
				case IEventMouse::MouseEventType::MOUSE_UP:
					if(onMouseUp != nullptr)
						onMouseUp(event);
					break;
				case IEventMouse::MouseEventType::MOUSE_MOVE:
					if(onMouseMove != nullptr)
						onMouseMove(event);
					break;
				case IEventMouse::MouseEventType::MOUSE_SCROLL:
					if(onMouseScroll != nullptr)
						onMouseScroll(event);
					break;
				default:
					break;
			}
		};

		if (IEventListener::init(Type::MOUSE, LISTENER_ID, listener))
		{
			return true;
		}

		return false;
	}
};

#endif