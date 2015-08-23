/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CENTEROFBEINGCOMMAND_H_
#define CENTEROFBEINGCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/packets/object/ShowFlyText.h"

using server::zone::packets::object::ShowFlyText;


class CenterOfBeingCommand : public QueueCommand {
public:

	CenterOfBeingCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (!creature->isPlayerCreature())
			return GENERALERROR;

		PlayerObject* ghost = creature->getPlayerObject();

		if (creature->hasBuff(STRING_HASHCODE("centerofbeing"))) {
			creature->sendSystemMessage("@combat_effects:already_centered");
			return GENERALERROR;
		}

		WeaponObject* weapon = creature->getWeapon();

		int duration = 0;
		int efficacy = 0;

		if (weapon->isUnarmedWeapon()) {
			duration = creature->getSkillMod("center_of_being_duration_unarmed");
			efficacy = creature->getSkillMod("unarmed_center_of_being_efficacy");
		} else if (weapon->isOneHandMeleeWeapon()) {
			duration = creature->getSkillMod("center_of_being_duration_onehandmelee");
			efficacy = creature->getSkillMod("onehandmelee_center_of_being_efficacy");
		} else if (weapon->isTwoHandMeleeWeapon()) {
			duration = creature->getSkillMod("center_of_being_duration_twohandmelee");
			efficacy = creature->getSkillMod("twohandmelee_center_of_being_efficacy");
		} else if (weapon->isPolearmWeaponObject()) {
			duration = creature->getSkillMod("center_of_being_duration_polearm");
			efficacy = creature->getSkillMod("polearm_center_of_being_efficacy");
		} else
			return GENERALERROR;

		if (duration == 0 || efficacy == 0)
			return GENERALERROR;

		creature->addBuff(STRING_HASHCODE("centerofbeing"));

		return SUCCESS;
	}

};

#endif //CENTEROFBEINGCOMMAND_H_
