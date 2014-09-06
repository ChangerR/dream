#ifndef __DREAM_IEVENTDISPATCHER_H
#define __DREAM_IEVENTDISPATCHER_H
#include "dstring.h"
class IEvent;
class INode;
class IEventCustom;
class IEventListenerCustom;

class IEventDispatcher : public IReferenceCounted
{
public:
    // Adds event listener
    
    /** Adds a event listener for a specified event with the priority of scene graph.
     *  @param listener The listener of a specified event.
     *  @param node The priority of the listener is based on the draw order of this node.
     *  @note  The priority of scene graph will be fixed value 0. So the order of listener item
     *          in the vector will be ' <0, scene graph (0 priority), >0'.
     */
    virtual void addEventListenerWithSceneGraphPriority(IEventListener* listener, INode* node) = 0;

    /** Adds a event listener for a specified event with the fixed priority.
     *  @param listener The listener of a specified event.
     *  @param fixedPriority The fixed priority of the listener.
     *  @note A lower priority will be called before the ones that have a higher value.
     *        0 priority is forbidden for fixed priority since it's used for scene graph based priority.
     */
    virtual void addEventListenerWithFixedPriority(IEventListener* listener, int fixedPriority) = 0;

    /** Adds a Custom event listener.
     It will use a fixed priority of 1.
     @return the generated event. Needed in order to remove the event from the dispather
     */
    virtual IEventListenerCustom* addCustomEventListener(const stringc &eventName, const std::function<void(IEventCustom*)>& callback) = 0;

    /////////////////////////////////////////////
    
    // Removes event listener
    
    /** Remove a listener
     *  @param listener The specified event listener which needs to be removed.
     */
    virtual void removeEventListener(IEventListener* listener) = 0;

    /** Removes all listeners with the same event listener type */
    virtual void removeEventListenersForType(IEventListener::Type listenerType) = 0;

    /** Removes all listeners which are associated with the specified target. */
    virtual void removeEventListenersForTarget(INode* target, bool recursive = false) = 0;
    
    /** Removes all custom listeners with the same event name */
    virtual void removeCustomEventListeners(const stringc& customEventName) = 0;

    /** Removes all listeners */
    virtual void removeAllEventListeners() = 0;

    /////////////////////////////////////////////
    
    // Pauses / Resumes event listener
    
    /** Pauses all listeners which are associated the specified target. */
    virtual void pauseEventListenersForTarget(INode* target, bool recursive = false) = 0;
    
    /** Resumes all listeners which are associated the specified target. */
    virtual void resumeEventListenersForTarget(INode* target, bool recursive = false) = 0;
    
    /////////////////////////////////////////////
    
    /** Sets listener's priority with fixed value. */
    virtual void setPriority(IEventListener* listener, int fixedPriority) = 0;

    /** Whether to enable dispatching events */
    virtual void setEnabled(bool isEnabled) = 0;

    /** Checks whether dispatching events is enabled */
    virtual bool isEnabled() const = 0;

    /////////////////////////////////////////////
    
    /** Dispatches the event
     *  Also removes all EventListeners marked for deletion from the
     *  event dispatcher list.
     */
    virtual void dispatchEvent(IEvent* event) = 0;

    /** Dispatches a Custom Event with a event name an optional user data */
    virtual void dispatchCustomEvent(const stringc &eventName, void *optionalUserData = nullptr) = 0;

    /////////////////////////////////////////////
    
    /** Constructor of EventDispatcher */
    IEventDispatcher(){}
    /** Destructor of EventDispatcher */
    virtual ~EventDispatcher(){};

#ifdef _DEBUG  
    /**
     * To help track down event listener issues in debug builds.
     * Verifies that the node has no event listeners associated with it when destroyed.
     */
    virtual void debugCheckNodeHasNoEventListenersOnDestruction(INode* node) = 0;
    
#endif
};

#endif //__DREAM_IEVENTDISPATCHER_H