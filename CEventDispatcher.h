#ifndef _DREAM_C_EVENT_DISPATCHER_H
#define _DREAM_C_EVENT_DISPATCHER_H
#include <functional>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include "IEventListener.h"
#include "Event.h"
/**
This class manages Event listener subscriptions
and Event dispatching.

The EventListener list is managed in such a way that
Event listeners can be added and removed even
from within an EventListener, while Events are being
dispatched.
*/
class CEventDispatcher : public IEventDispatcher
{
public:
    // Adds Event listener
    
    /** Adds a Event listener for a specified Event with the priority of scene graph.
     *  @param listener The listener of a specified Event.
     *  @param node The priority of the listener is based on the draw order of this node.
     *  @note  The priority of scene graph will be fixed value 0. So the order of listener item
     *          in the vector will be ' <0, scene graph (0 priority), >0'.
     */
    virtual void addEventListenerWithSceneGraphPriority(IEventListener* listener, Node* node);

    /** Adds a Event listener for a specified Event with the fixed priority.
     *  @param listener The listener of a specified Event.
     *  @param fixedPriority The fixed priority of the listener.
     *  @note A lower priority will be called before the ones that have a higher value.
     *        0 priority is forbidden for fixed priority since it's used for scene graph based priority.
     */
    virtual void addEventListenerWithFixedPriority(IEventListener* listener, int fixedPriority);

    /** Adds a Custom Event listener.
     It will use a fixed priority of 1.
     @return the generated Event. Needed in order to remove the Event from the dispather
     */
    virtual EventListenerCustom* addCustomEventListener(const stringc &EventName, const std::function<void(EventCustom*)>& callback);

    /////////////////////////////////////////////
    
    // Removes Event listener
    
    /** Remove a listener
     *  @param listener The specified Event listener which needs to be removed.
     */
    virtual void removeEventListener(IEventListener* listener);

    /** Removes all listeners with the same Event listener type */
    virtual void removeEventListenersForType(IEventListener::Type listenerType);

    /** Removes all listeners which are associated with the specified target. */
    virtual void removeEventListenersForTarget(Node* target, bool recursive = false);
    
    /** Removes all custom listeners with the same Event name */
    virtual void removeCustomEventListeners(const stringc& customEventName);

    /** Removes all listeners */
    virtual void removeAllEventListeners();

    /////////////////////////////////////////////
    
    // Pauses / Resumes Event listener
    
    /** Pauses all listeners which are associated the specified target. */
    virtual void pauseEventListenersForTarget(Node* target, bool recursive = false);
    
    /** Resumes all listeners which are associated the specified target. */
    virtual void resumeEventListenersForTarget(Node* target, bool recursive = false);
    
    /////////////////////////////////////////////
    
    /** Sets listener's priority with fixed value. */
    virtual void setPriority(IEventListener* listener, int fixedPriority);

    /** Whether to enable dispatching Events */
    virtual void setEnabled(bool isEnabled);

    /** Checks whether dispatching Events is enabled */
    virtual bool isEnabled() const;

    /////////////////////////////////////////////
    
    /** Dispatches the Event
     *  Also removes all EventListeners marked for deletion from the
     *  Event dispatcher list.
     */
    virtual void dispatchEvent(Event* Event);

    /** Dispatches a Custom Event with a Event name an optional user data */
    void dispatchCustomEvent(const stringc &EventName, void *optionalUserData = nullptr);

    /////////////////////////////////////////////
    
    /** Constructor of EventDispatcher */
    CEventDispatcher();
    /** Destructor of EventDispatcher */
    virtual ~CEventDispatcher();

#ifdef _DEBUG
    
    /**
     * To help track down Event listener issues in debug builds.
     * Verifies that the node has no Event listeners associated with it when destroyed.
     */
    virtual void debugCheckNodeHasNoEventListenersOnDestruction(Node* node);
    
#endif

protected:
    friend class Node;
    
    /** Sets the dirty flag for a node. */
    void setDirtyForNode(Node* node);
    
    /**
     *  The vector to store Event listeners with scene graph based priority and fixed priority.
     */
    class EventListenerVector
    {
    public:
        EventListenerVector();
        ~EventListenerVector();
        size_t size() const;
        bool empty() const;
        
        void push_back(IEventListener* item);
        void clearSceneGraphListeners();
        void clearFixedListeners();
        void clear();
        
        inline std::vector<IEventListener*>* getFixedPriorityListeners() const { return _fixedListeners; };
        inline std::vector<IEventListener*>* getSceneGraphPriorityListeners() const { return _sceneGraphListeners; };
        inline ssize_t getGt0Index() const { return _gt0Index; };
        inline void setGt0Index(ssize_t index) { _gt0Index = index; };
    private:
        std::vector<IEventListener*>* _fixedListeners;
        std::vector<IEventListener*>* _sceneGraphListeners;
        ssize_t _gt0Index;
    };
    
    /** Adds an Event listener with item
     *  @note if it is dispatching Event, the added operation will be delayed to the end of current dispatch
     *  @see forceAddEventListener
     */
    void addEventListener(IEventListener* listener);
    
    /** Force adding an Event listener
     *  @note force add an Event listener which will ignore whether it's in dispatching.
     *  @see addEventListener
     */
    void forceAddEventListener(IEventListener* listener);
    
    /** Gets Event the listener list for the Event listener type. */
    EventListenerVector* getListeners(const IEventListener::ListenerID& listenerID);
    
    /** Update dirty flag */
    void updateDirtyFlagForSceneGraph();
    
    /** Removes all listeners with the same Event listener ID */
    void removeEventListenersForListenerID(const IEventListener::ListenerID& listenerID);
    
    /** Sort Event listener */
    void sortEventListeners(const IEventListener::ListenerID& listenerID);
    
    /** Sorts the listeners of specified type by scene graph priority */
    void sortEventListenersOfSceneGraphPriority(const IEventListener::ListenerID& listenerID, Node* rootNode);
    
    /** Sorts the listeners of specified type by fixed priority */
    void sortEventListenersOfFixedPriority(const IEventListener::ListenerID& listenerID);
    
    /** Updates all listeners
     *  1) Removes all listener items that have been marked as 'removed' when dispatching Event.
     *  2) Adds all listener items that have been marked as 'added' when dispatching Event.
     */
    void updateListeners(Event* Event);

    /** Touch Event needs to be processed different with other Events since it needs support ALL_AT_ONCE and ONE_BY_NONE mode. */
    void dispatchTouchEvent(EventTouch* Event);
    
    /** Associates node with Event listener */
    void associateNodeAndEventListener(Node* node, IEventListener* listener);
    
    /** Dissociates node with Event listener */
    void dissociateNodeAndEventListener(Node* node, IEventListener* listener);
    
    /** Dispatches Event to listeners with a specified listener type */
    void dispatchEventToListeners(EventListenerVector* listeners, const std::function<bool(IEventListener*)>& onEvent);
    
    /// Priority dirty flag
    enum class DirtyFlag
    {
        NONE = 0,
        FIXED_PRIORITY = 1 << 0,
        SCENE_GRAPH_PRIORITY = 1 << 1,
        ALL = FIXED_PRIORITY | SCENE_GRAPH_PRIORITY
    };
    
    /** Sets the dirty flag for a specified listener ID */
    void setDirty(const IEventListener::ListenerID& listenerID, DirtyFlag flag);
    
    /** Walks though scene graph to get the draw order for each node, it's called before sorting Event listener with scene graph priority */
    void visitTarget(Node* node, bool isRootNode);
    
    /** Listeners map */
    std::unordered_map<IEventListener::ListenerID, EventListenerVector*> _listenerMap;
    
    /** The map of dirty flag */
    std::unordered_map<IEventListener::ListenerID, DirtyFlag> _priorityDirtyFlagMap;
    
    /** The map of node and Event listeners */
    std::unordered_map<Node*, std::vector<IEventListener*>*> _nodeListenersMap;
    
    /** The map of node and its Event priority */
    std::unordered_map<Node*, int> _nodePriorityMap;
    
    /** key: Global Z Order, value: Sorted Nodes */
    std::unordered_map<float, std::vector<Node*>> _globalZOrderNodeMap;
    
    /** The listeners to be added after dispatching Event */
    std::vector<IEventListener*> _toAddedListeners;
    
    /** The nodes were associated with scene graph based priority listeners */
    std::set<Node*> _dirtyNodes;
    
    /** Whether the dispatcher is dispatching Event */
    int _inDispatch;
    
    /** Whether to enable dispatching Event */
    bool _isEnabled;
    
    int _nodePriorityIndex;
    
    std::set<stringc> _internalCustomListenerIDs;
};

#endif //_DREAM_C_Event_DISPATCHER_H