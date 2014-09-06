#ifndef __DREAM_EventLISTENER_KEYBOARD
#define __DREAM_EventLISTENER_KEYBOARD
#include "IEventListener.h"
#include "EventKeyboard.h"

class EventListenerKeyboard : public IEventListener
{
public:
    static const stringc LISTENER_ID;
    
    static EventListenerKeyboard* create();
    
    /// Overrides
    virtual EventListenerKeyboard* clone() override
	{
		auto ret = new EventListenerKeyboard();
		if (ret && ret->init())
		{
			ret->onKeyPressed = onKeyPressed;
			ret->onKeyReleased = onKeyReleased;
		}
		return ret;
	}

    virtual bool checkAvailable() override
	{
		if (onKeyPressed == nullptr && onKeyReleased == nullptr)
		{
			Printer::log("Invalid EventListenerKeyboard!");
			return false;
		}
		
		return true;
	}
    
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
    std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;
private:
    EventListenerKeyboard()
		: onKeyPressed(nullptr)
		, onKeyReleased(nullptr)
	{
	}
	
    bool init()
	{
		auto listener = [this](Event* event){
			auto keyboardEvent = static_cast<EventKeyboard*>(event);
			if (keyboardEvent->_isPressed)
			{
				if (onKeyPressed != nullptr)
					onKeyPressed(keyboardEvent->_keyCode, event);
			}
			else
			{
				if (onKeyReleased != nullptr)
					onKeyReleased(keyboardEvent->_keyCode, event);
			}
		};
		
		if (EventListener::init(Type::KEYBOARD, LISTENER_ID, listener))
		{
			return true;
		}
		
		return false;
	}
};

#endif //__DREAM_EventLISTENER_KEYBOARD