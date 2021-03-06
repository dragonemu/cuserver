/*
Copyright (C) 2014 CU Galaxies
*/

#ifndef DUMPOBJECTINFORMATIONCOMMAND_H_
#define DUMPOBJECTINFORMATIONCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class DumpObjectInformationCommand : public QueueCommand {
public:

	DumpObjectInformationCommand(const String& name, ZoneProcessServer* server)
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

#endif //DUMPOBJECTINFORMATIONCOMMAND_H_
