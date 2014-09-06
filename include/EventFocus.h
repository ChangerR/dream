#ifndef __DREAM_IENENT_FOCUS_H
#define __DREAM_IENENT_FOCUS_H
#include "Event.h"
class Widget;
class EventFocus : public Event
{
public:
    EventFocus(Widget* widgetLoseFocus, Widget* widgetGetFocus)
		:Event(Type::FOCUS),
		_widgetLoseFocus(widgetLoseFocus),
		_widgetGetFocus(widgetGetFocus)
		{		
		}
    
private:
    Widget *_widgetGetFocus;
    Widget *_widgetLoseFocus;
};
#endif