#ifndef COOLDOWNTASK_H_
#define COOLDOWNTASK_H_


#include "server/zone/objects/creature/CreatureObject.h"

class CoolDownTask : public Task {
	ManagedReference<CreatureObject*> creature;
	int counter;
	int count;
	String say;
public:

	CoolDownTask(CreatureObject* creo, int time, String name) {
		creature = creo;
		counter = 0;
		count = time * 1000;
		say = name;
	}

	void run() {
		Locker lockerC(creature);
		Locker lockerCT(creature);
		if(creature != NULL) {
			int amountOfTicks = count;
			if (counter < amountOfTicks ) {

				counter+= 100;
				this->reschedule(100);
			}
			else {
				creature->removePendingTask(say);
			}
		}
	}
};


#endif /* COOLDOWNTASK_H_ */
