/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef BLEEDATTACKCOMMAND_H_
#define BLEEDATTACKCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class BleedAttackCommand : public CombatQueueCommand {
public:

	BleedAttackCommand(const String& name, ZoneProcessServer* server)
		: CombatQueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		return doCombatAction(creature, target);
	}

};

#endif //BLEEDATTACKCOMMAND_H_
