#ifndef __DREAM_IIEventLISTENER_CUSTOM
#define __DREAM_IIEventLISTENER_CUSTOM
#include "IEventListener.h"
#include "IEventCustom.h"

/**
 *  Usage:
 *        auto dispatcher = Director::getInstance()->getIEventDispatcher();
 *     Adds a listener:
 *
 *        auto callback = [](IEventCustom* IEvent){ do_some_thing(); };
 *        auto listener = IEventListenerCustom::create(callback);
 *        dispatcher->addIEventListenerWithSceneGraphPriority(listener, one_node);
 *
 *     Dispatchs a custom IEvent:
 *
 *        IEventCustom IEvent("your_IEvent_type");
 *        dispatcher->dispatchIEvent(&IEvent);
 *
 *     Removes a listener
 *
 *        dispatcher->removeIEventListener(listener);
 */
class IEventListenerCustom : public IEventListener
{
public:
    /** Creates an IEvent listener with type and callback.
     *  @param IEventType The type of the IEvent.
     *  @param callback The callback function when the specified IEvent was emitted.
     */
    static IEventListenerCustom* create(const stringc& EventName, const std::function<void(IEventCustom*)>& callback);
    
    /// Overrides
    virtual bool checkAvailable() override
	{
		bool ret = false;
		if (IEventListener::checkAvailable() && _onCustomEvent != nullptr)
		{
			ret = true;
		}
		return ret;
	}
	
    virtual IEventListenerCustom* clone() override
	{
		IEventListenerCustom* ret = new IEventListenerCustom();
		if (ret && ret->init(_listenerID, _onCustomEvent))
		{
			return ret;
		}else
			return NULL;		
	}

    
    /** Constructor */
    IEventListenerCustom()
		: _onCustomEvent(nullptr)
	{
	}
    
    /** Initializes IEvent with type and callback function */
    bool init(const ListenerID& listenerId, const std::function<void(IEventCustom*)>& callback)
	{
		bool ret = false;
		
		_onCustomEvent = callback;
		
		auto listener = [this](Event* event){
			if (_onCustomEvent != nullptr)
			{
				_onCustomEvent(static_cast<IEventCustom*>(event));
			}
		};
		
		if (EventListener::init(EventListener::Type::CUSTOM, listenerId, listener))
		{
			ret = true;
		}
		return ret;
	}
    
protected:
    std::function<void(IEventCustom*)> _onCustomIEvent;
};
#endif //__DREAM_IIEventLISTENER_CUSTOM