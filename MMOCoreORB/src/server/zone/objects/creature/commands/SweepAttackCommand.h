/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef SWEEPATTACKCOMMAND_H_
#define SWEEPATTACKCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class SweepAttackCommand : public CombatQueueCommand {
public:

	SweepAttackCommand(const String& name, ZoneProcessServer* server)
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

#endif //SWEEPATTACKCOMMAND_H_
