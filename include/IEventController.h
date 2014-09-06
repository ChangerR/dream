#ifndef __DREAM_IENENT_CONTROLLER_H
#define __DREAM_IENENT_CONTROLLER_H
#include "IEvent.h"
class IEventController : public IEvent {
public:
	IEventController(): IEvent(Type::GAME_CONTROLLER){}
};
#endif