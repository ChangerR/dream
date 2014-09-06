#ifndef __DREAM2_EVENT_INCLUDE_
#define __DREAM2_EVENT_INCLUDE_
#include "dstring.h"
#include "Node.h"

// The application will come to foreground.
// This message is posted in cocos/platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxRenderer.cpp.
#define EVENT_COME_TO_FOREGROUND    "event_come_to_foreground"

// The renderer[android:GLSurfaceView.Renderer  WP8:Cocos2dRenderer] was recreated.
// This message is used for reloading resources before renderer is recreated on Android/WP8.
// This message is posted in cocos/platform/android/javaactivity.cpp and cocos\platform\wp8-xaml\cpp\Cocos2dRenderer.cpp.
#define EVENT_RENDERER_RECREATED "event_renderer_recreated"

// The application will come to background.
// This message is used for doing something before coming to background, such as save RenderTexture.
// This message is posted in cocos/platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxRenderer.cpp and cocos\platform\wp8-xaml\cpp\Cocos2dRenderer.cpp.
#define EVENT_COME_TO_BACKGROUND    "event_come_to_background"

class Event : public IReferenceCounted {
public:
	//TYPE
	enum class TYPE {
		TOUCH,
        KEYBOARD,
        ACCELERATION,
        MOUSE,
        FOCUS,
        GAME_CONTROLLER,
        CUSTOM
	};
	//constructor
	Event(TYPE t):_type(t){}

	virtual ~Event() {}

	/** Gets the event type */
	inline Type getType() const { return _type; };
    
    /** Stops propagation for current event */
    inline void stopPropagation() { _isStopped = true; };
    
    /** Checks whether the event has been stopped */
    inline bool isStopped() const { return _isStopped; };
    
    /** @brief Gets current target of the event
     *  @return The target with which the event associates.
     *  @note It onlys be available when the event listener is associated with node. 
     *        It returns 0 when the listener is associated with fixed priority.
     */
    inline Node* getCurrentTarget() { return _currentTarget; };

protected:
	/** Sets current target */
    inline void setCurrentTarget(Node* target) { _currentTarget = target; };

	Node* _currentTarget;				
	TYPE _type;
	bool _isStopped;
};
#endif
