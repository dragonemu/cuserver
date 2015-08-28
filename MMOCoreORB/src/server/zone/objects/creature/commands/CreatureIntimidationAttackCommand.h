/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef CREATUREINTIMIDATIONATTACKCOMMAND_H_
#define CREATUREINTIMIDATIONATTACKCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class CreatureIntimidationAttackCommand : public QueueCommand {
public:

	CreatureIntimidationAttackCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		return SUCCESS;
	}

};

#endif //CREATUREINTIMIDATIONATTACKCOMMAND_H_
