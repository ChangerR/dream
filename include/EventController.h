#ifndef __DREAM_IENENT_CONTROLLER_H
#define __DREAM_IENENT_CONTROLLER_H
#include "Event.h"
class EventController : public Event {
public:
	EventController(): Event(Type::GAME_CONTROLLER){}
};
#endif