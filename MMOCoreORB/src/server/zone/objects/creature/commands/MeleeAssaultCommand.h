/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef MELEEASSAULTCOMMAND_H_
#define MELEEASSAULTCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class MeleeAssaultCommand : public CombatQueueCommand {
public:

	MeleeAssaultCommand(const String& name, ZoneProcessServer* server)
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

#endif //MELEEASSAULTCOMMAND_H_
