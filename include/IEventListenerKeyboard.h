#ifndef __DREAM_IEVENTLISTENER_KEYBOARD
#define __DREAM_IEVENTLISTENER_KEYBOARD
#include "IEventListener.h"
#include "IEventKeyboard.h"

class IEventListenerKeyboard : public IEventListener
{
public:
    static const stringc LISTENER_ID;
    
    static IEventListenerKeyboard* create();
    
    /// Overrides
    virtual IEventListenerKeyboard* clone() override
	{
		auto ret = new IEventListenerKeyboard();
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
			Printer::log("Invalid IEventListenerKeyboard!");
			return false;
		}
		
		return true;
	}
    
    std::function<void(IEventKeyboard::KeyCode, IEvent*)> onKeyPressed;
    std::function<void(IEventKeyboard::KeyCode, IEvent*)> onKeyReleased;
private:
    IEventListenerKeyboard()
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

#endif //__DREAM_IEVENTLISTENER_KEYBOARD