/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef KNOCKDOWNRECOVERYCOMMAND_H_
#define KNOCKDOWNRECOVERYCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class KnockdownRecoveryCommand : public QueueCommand {
public:

	KnockdownRecoveryCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (!creature->isKnockedDown())
			return INVALIDSTATE;

			creature->setPosture(CreaturePosture::UPRIGHT);

			creature->inflictDamage(creature, CreatureAttribute::ACTION, 160, true, true);
			creature->inflictDamage(creature, CreatureAttribute::MIND, 40, true, true);


		return SUCCESS;
	}

};

#endif //KNOCKDOWNRECOVERYCOMMAND_H_
