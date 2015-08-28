/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef LEGHITCOMMAND_H_
#define LEGHITCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class LegHitCommand : public QueueCommand {
public:

	LegHitCommand(const String& name, ZoneProcessServer* server)
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

#endif //LEGHITCOMMAND_H_
