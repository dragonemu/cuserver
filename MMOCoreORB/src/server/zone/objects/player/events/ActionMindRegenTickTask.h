/*
 * ActionMindRegenTickTask.h
 *
 *  Created on: Nov 12, 2014
 *      Author: dreadlow
 */

#ifndef ACTIONMINDREGENTICKTASK_H_
#define ACTIONMINDREGENTICKTASK_H_

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"

class ActionMindRegenTickTask : public Task {
	ManagedReference<CreatureObject*> creature;
	ManagedReference<PlayerObject*> player;
public:

	ActionMindRegenTickTask(CreatureObject* creo) {
		creature = creo;
		player = creo->getPlayerObject();
	}

	void run() {
		Locker locker(creature);

		if (creature != NULL) {
			if (player != NULL && !player->isOffline()) {
				//if (!creature->isStunned())
				creature->healDamage(creature, CreatureAttribute::ACTION, 5, true, false);

				creature->healDamage(creature, CreatureAttribute::MIND, 5, true, false);

				this->reschedule(500);
			} else
				creature->removePendingTask("ActionMindRegenTickTask");
		}
	}

};

#endif /* ACTIONMINDREGENTICKTASK_H_ */
