#ifndef __DREAM_EVENTLISTENER_CUSTOM
#define __DREAM_EVENTLISTENER_CUSTOM
#include "IEventListener.h"
#include "EventCustom.h"

/**
 *  Usage:
 *        auto dispatcher = Director::getInstance()->getEventDispatcher();
 *     Adds a listener:
 *
 *        auto callback = [](EventCustom* Event){ do_some_thing(); };
 *        auto listener = EventListenerCustom::create(callback);
 *        dispatcher->addEventListenerWithSceneGraphPriority(listener, one_node);
 *
 *     Dispatchs a custom Event:
 *
 *        EventCustom Event("your_Event_type");
 *        dispatcher->dispatchEvent(&Event);
 *
 *     Removes a listener
 *
 *        dispatcher->removeEventListener(listener);
 */
class EventListenerCustom : public IEventListener
{
public:
    /** Creates an Event listener with type and callback.
     *  @param EventType The type of the Event.
     *  @param callback The callback function when the specified Event was emitted.
     */
    static EventListenerCustom* create(const stringc& EventName, const std::function<void(EventCustom*)>& callback);
    
    /// Overrides
    virtual bool checkAvailable() override
	{
		bool ret = false;
		if (EventListener::checkAvailable() && _onCustomEvent != nullptr)
		{
			ret = true;
		}
		return ret;
	}
	
    virtual EventListenerCustom* clone() override
	{
		EventListenerCustom* ret = new EventListenerCustom();
		if (ret && ret->init(_listenerID, _onCustomEvent))
		{
			return ret;
		}else
			return NULL;		
	}

    
    /** Constructor */
    EventListenerCustom()
		: _onCustomEvent(nullptr)
	{
	}
    
    /** Initializes Event with type and callback function */
    bool init(const ListenerID& listenerId, const std::function<void(EventCustom*)>& callback)
	{
		bool ret = false;
		
		_onCustomEvent = callback;
		
		auto listener = [this](Event* event){
			if (_onCustomEvent != nullptr)
			{
				_onCustomEvent(static_cast<EventCustom*>(event));
			}
		};
		
		if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
		{
			ret = true;
		}
		return ret;
	}
    
protected:
    std::function<void(EventCustom*)> _onCustomEvent;
};
#endif //__DREAM_IEventLISTENER_CUSTOM