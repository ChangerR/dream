#ifndef __DREAM_IENENT_FOCUS_H
#define __DREAM_IENENT_FOCUS_H
#include "IEvent.h"
class Widget;
class IEventFocus : public IEvent
{
public:
    IEventFocus(Widget* widgetLoseFocus, Widget* widgetGetFocus)
		:IEvent(Type::FOCUS),
		_widgetLoseFocus(widgetLoseFocus),
		_widgetGetFocus(widgetGetFocus)
		{		
		}
    
private:
    Widget *_widgetGetFocus;
    Widget *_widgetLoseFocus;
};
#endif