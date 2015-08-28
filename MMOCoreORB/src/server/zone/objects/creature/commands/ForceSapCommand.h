/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef FORCESAPCOMMAND_H_
#define FORCESAPCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class ForceSapCommand : public QueueCommand {
public:

	ForceSapCommand(const String& name, ZoneProcessServer* server)
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

#endif //FORCESAPCOMMAND_H_
