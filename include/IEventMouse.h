#ifndef __DREAM_IEVENT_MOUSE_H
#define __DREAM_IEVENT_MOUSE_H
#include "IEvent.h"
#define MOUSE_BUTTON_LEFT       0
#define MOUSE_BUTTON_RIGHT      1
#define MOUSE_BUTTON_MIDDLE     2
#define MOUSE_BUTTON_4          3
#define MOUSE_BUTTON_5          4
#define MOUSE_BUTTON_6          5
#define MOUSE_BUTTON_7          6
#define MOUSE_BUTTON_8          7

class IEventMouse : public IEvent
{
public:
    /**
    * Different types of MouseEvent
    */
    enum class MouseEventType
    {
        MOUSE_NONE,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        MOUSE_SCROLL,
    };

    EventMouse(MouseEventType mouseEventCode): Event(Type::MOUSE)
		, _mouseEventType(mouseEventCode)
		, _mouseButton(-1)
		, _x(0.0f)
		, _y(0.0f)
		, _scrollX(0.0f)
		, _scrollY(0.0f)
	{
	};
	
    /** Set mouse scroll data */
    inline void setScrollData(float scrollX, float scrollY) { _scrollX = scrollX; _scrollY = scrollY; };
    inline float getScrollX() { return _scrollX; };
    inline float getScrollY() { return _scrollY; };

    inline void setCursorPosition(float x, float y) { _x = x; _y = y; };
    inline void setMouseButton(int button) { _mouseButton = button; };
    inline int getMouseButton() { return _mouseButton; };
    inline float getCursorX() { return _x; };
    inline float getCursorY() { return _y; };

private:
    MouseEventType _mouseEventType;
    int _mouseButton;
    float _x;
    float _y;
    float _scrollX;
    float _scrollY;

    friend class CEventListenerMouse;
};

#endif