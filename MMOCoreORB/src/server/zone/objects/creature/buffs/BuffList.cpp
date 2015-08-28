/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#include "BuffList.h"
#include "server/zone/managers/object/ObjectManager.h"
#include "server/zone/packets/creature/CreatureObjectMessage6.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "BuffDurationEvent.h"

BuffList::BuffList() {
	buffObjectMap.setNoDuplicateInsertPlan();
	addSerializableVariable("buffObjectMap", &buffObjectMap);
}


void BuffList::addBuff(uint32 buffCRC, CreatureObject* player, int duration = 0) {
	int key = instance()->buffObjectMap.find(buffCRC);

	if (key <= 0)
		return;

	Reference<Buff*> buff = instance()->buffObjectMap.get(key);
	Buff* gotBuff = buff.get();

	// TODO: Pass the original buffer if different.
	gotBuff->setBuffer(player->getObjectID());

	// time.
	Time curTime;
	uint64 theTime = curTime.getMiliTime();

	gotBuff->setStartTime(theTime);
	BuffDurationEvent* buffTimer = new BuffDurationEvent(player, buffCRC);
	buffTimer->schedule((uint64) (duration > 0 ? duration : gotBuff->getDuration()) * 1000);

	set(buffCRC, buff);

	BasePacket* message = new CreatureObjectMessage6(player);
	player->broadcastMessage(message, true);

	applyEffects(player, gotBuff);
}

void BuffList::removeBuff(uint32 buffCRC, CreatureObject* creature) {

	if (hasBuff(buffCRC) && creature) {
		drop(buffCRC);

		BasePacket* message = new CreatureObjectMessage6(creature);
		creature->broadcastMessage(message, true);

		Buff* efrBuff = instance()->buffObjectMap.get(buffCRC).get();

		removeEffects(creature, efrBuff);
	}
}

bool BuffList::hasBuff(uint32 buffcrc) {
	return get(buffcrc) != NULL;
}

void BuffList::clearBuffs(CreatureObject* creature) {
	while (size() > 0) {
		ManagedReference<Buff*> buff = getValueAt(0);
		drop(0);
	}

	BasePacket* message = new CreatureObjectMessage6(creature);
	creature->broadcastMessage(message, true);
}

void BuffList::loadClientData() {
	IffStream* iffStream = TemplateManager::instance()->openIffFile("datatables/buff/buff.iff");

	if (iffStream == NULL) {
		Logger::console.error("Could not load buffs.");
		return;
	}

	DataTableIff dtiff;
	dtiff.readObject(iffStream);

	delete iffStream;

	for (int i = 0; i < dtiff.getTotalRows(); ++i) {
		DataTableRow* row = dtiff.getRow(i);

		Reference<Buff*> buffObject = new Buff();
		buffObject.get()->parseDataTableRow(&dtiff, row);

		if (buffObject.get() == NULL)
			continue;

		// Logger::console.info("Loading Buff " + buffObject->getBuffName(), true);

		if (!buffObject.get()->getBuffName().isEmpty())
			buffObjectMap.put(buffObject.get()->getBuffName().toLowerCase().hashCode(), buffObject);
	}

	Logger::console.info("Successfully loaded " + String::valueOf(buffObjectMap.size()) + " buffs from datatables.", true);
}

void BuffList::applyEffects(CreatureObject* creature, Buff* buff) {
	Buff* gotBuffObject = buff;

	if (gotBuffObject == NULL) {
		Logger::console.error("Buff was NULL in BuffList::applyEffects()");
		return;
	}

	try {
		if (gotBuffObject->getGroup1().contains("stun"))
			creature->setState(CreatureState::STUNNED, true);
		else if (gotBuffObject->getGroup1().contains("blind"))
			creature->setState(CreatureState::BLINDED, true);
		else if (gotBuffObject->getGroup1().contains("dizzy"))
			creature->setState(CreatureState::DIZZY, true);
		else if (gotBuffObject->getGroup1().contains("cover"))
			creature->setState(CreatureState::COVER, true);
		else if	(gotBuffObject->getGroup1().contains("aim"))
			creature->setState(CreatureState::AIMING, true);
		else if (gotBuffObject->getGroup1().contains("berserk"))
			creature->setState(CreatureState::BERSERK, true);
		else if (gotBuffObject->getGroup1().contains("intimidate"))
			creature->setState(CreatureState::INTIMIDATED, true);
		else if (gotBuffObject->getGroup2().contains("snare")) {
			creature->setSpeedMultiplierMod(0.25f, true);
			creature->setState(CreatureState::IMMOBILIZED, true);
		} else if (gotBuffObject->getGroup2().contains("root")) {
			creature->setSpeedMultiplierMod(0.f, true);
			creature->setState(CreatureState::IMMOBILIZED, true);
		} else if (gotBuffObject->getGroup2().contains("mez"))
			creature->setState(CreatureState::FROZEN, true);
		else if (gotBuffObject->getGroup2().contains("daze")) {
			creature->playEffect("appearance/pt_state_dazed.prt", "");
		} else {
			for (int i=1; i < 5; ++i) { // FIVE EFFECTS IN THE DATATABLE.
				// Attribute
				if (gotBuffObject->getEffectName(i).contains("health") || gotBuffObject->getEffectName(i).contains("constitution") ||
						gotBuffObject->getEffectName(i).contains("action") || gotBuffObject->getEffectName(i).contains("stamina") ||
						gotBuffObject->getEffectName(i).contains("mind") || gotBuffObject->getEffectName(i).contains("willpower")) {

					// Percent of HAM.
					if (gotBuffObject->getEffectName(i).contains("Percent")) {
						String attributeToBuff = gotBuffObject->getEffectName(i).subString(0, gotBuffObject->getEffectName(i).length() - 7); // "percent" = 7 characters.
						int creatureAttribute = CreatureAttribute::getAttribute(attributeToBuff);
						int creatureValue = (creature->getMaxHAM(creatureAttribute)) + (creature->getMaxHAM(creatureAttribute) * (gotBuffObject->getEffectValue(i) / 100));
						creature->setMaxHAM(creatureAttribute, creatureValue, true);
					} else {
						int creatureAttribute = CreatureAttribute::getAttribute(gotBuffObject->getEffectName(i));
						int creatureValue = (creature->getMaxHAM(creatureAttribute)) + (gotBuffObject->getEffectValue(i));
						creature->setMaxHAM(creatureAttribute, creatureValue, true);
					}
				} else if (gotBuffObject->getEffectName(i).contains("movement")) {
					if (gotBuffObject->getBuffName().contains("forcerun")) {
						float speedMod;

						if (gotBuffObject->getBuffName().contains("1"))
							speedMod = 2.f;
						else if (gotBuffObject->getBuffName().contains("2"))
							speedMod = 3.f;
						else speedMod = 1.f;

						creature->setSpeedMultiplierMod(speedMod, true);
					} else
						creature->setSpeedMultiplierMod(2.f, true);
				} else if (gotBuffObject->getEffectName(i).contains("combat_slow")) {
					creature->playEffect("appearance/pt_state_warcry.prt", "");
					creature->addSkillMod(SkillModManager::BUFF, gotBuffObject->getEffectName(i), (int)gotBuffObject->getEffectValue(i), true);
				} else if (!gotBuffObject->getEffectName(i).contains("movement") && !CreatureAttribute::isHAM(CreatureAttribute::getAttribute((gotBuffObject->getEffectName(i))))
					&& !gotBuffObject->getEffectName(i).isEmpty() && !gotBuffObject->getEffectName(i).contains("script_var") && !gotBuffObject->getEffectName(i).contains("Percent")
					&& !gotBuffObject->getEffectName(i).contains("delay_attack") && !gotBuffObject->getEffectName(i).contains("combat_slow") && !gotBuffObject->getEffectName(i).contains("xp_bonus")
					&& !gotBuffObject->getEffectName(i).contains("group") && !gotBuffObject->getEffectName(i).contains("proc_buff") && !gotBuffObject->getEffectName(i).contains("invis")) {
					creature->addSkillMod(SkillModManager::BUFF, gotBuffObject->getEffectName(i), (int)gotBuffObject->getEffectValue(i), true);
				}
			}
		}

		if (!gotBuffObject->getParticleEffect().isEmpty()) {
			if (gotBuffObject->getBuffName().contains("forcerun"))
				creature->playEffect("appearance/pt_force_speed_moves.prt", "ltoe");
			else
				creature->playEffect(gotBuffObject->getParticleEffect(), "spine3");
		}

	} catch (Exception& e) {
		Logger::console.error(e.getMessage());
		e.printStackTrace();
	}
}

void BuffList::removeEffects(CreatureObject* creature, Buff* buff) {
	Buff* gotBuffObject = buff;

	if (gotBuffObject == NULL) {
		Logger::console.error("Buff was NULL in BuffList::removeEffects()");
		return;
	}

	Reference<CooldownTimerMap*> cooldownTimerMap = creature->getCooldownTimerMap();

	try {
		if (gotBuffObject->getGroup1().contains("stun"))
			creature->clearState(CreatureState::STUNNED, true);
		else if (gotBuffObject->getGroup1().contains("blind"))
			creature->clearState(CreatureState::BLINDED, true);
		else if (gotBuffObject->getGroup1().contains("dizzy"))
			creature->clearState(CreatureState::DIZZY, true);
		else if (gotBuffObject->getGroup1().contains("cover"))
			creature->clearState(CreatureState::COVER, true);
		else if	(gotBuffObject->getGroup1().contains("aim"))
			creature->clearState(CreatureState::AIMING, true);
		else if (gotBuffObject->getGroup1().contains("berserk"))
			creature->clearState(CreatureState::BERSERK, true);
		else if (gotBuffObject->getGroup1().contains("intimidate"))
			creature->clearState(CreatureState::INTIMIDATED, true);
		else if (gotBuffObject->getGroup2().contains("snare")) {
			creature->clearState(CreatureState::IMMOBILIZED, true);
			creature->setSpeedMultiplierMod(creature->getSpeedMultiplierBase(), true);
			creature->addBuff(String("player_snare_immunity").hashCode());
		} else if (gotBuffObject->getGroup2().contains("root")) {
			creature->clearState(CreatureState::IMMOBILIZED, true);
			creature->setSpeedMultiplierMod(creature->getSpeedMultiplierBase(), true);
			creature->addBuff(String("player_root_immunity").hashCode());
		}else if (gotBuffObject->getGroup2().contains("mez")) {
			creature->clearState(CreatureState::FROZEN, true);
			creature->addBuff(String("player_mez_immunity").hashCode());
		} else {
			for (int i=1; i < 5; ++i) { // FIVE EFFECTS IN THE DATATABLE.
				// Attribute
				if (gotBuffObject->getEffectName(i).contains("health") || gotBuffObject->getEffectName(i).contains("constitution") ||
						gotBuffObject->getEffectName(i).contains("action") || gotBuffObject->getEffectName(i).contains("stamina") ||
						gotBuffObject->getEffectName(i).contains("mind") || gotBuffObject->getEffectName(i).contains("willpower")) {

					// Percent of HAM.
					if (gotBuffObject->getEffectName(i).contains("Percent")) {
						String attributeToBuff = gotBuffObject->getEffectName(i).subString(0, gotBuffObject->getEffectName(i).length() - 7); // "percent" = 7 characters.
						int creatureAttribute = CreatureAttribute::getAttribute(attributeToBuff);
						int creatureValue = (creature->getMaxHAM(creatureAttribute)) - (creature->getMaxHAM(creatureAttribute) * (gotBuffObject->getEffectValue(i) / 100));
						creature->setMaxHAM(creatureAttribute, creatureValue, true);
					} else {
						int creatureAttribute = CreatureAttribute::getAttribute(gotBuffObject->getEffectName(i));
						int creatureValue = (creature->getMaxHAM(creatureAttribute)) - (gotBuffObject->getEffectValue(i));
						creature->setMaxHAM(creatureAttribute, creatureValue, true);
					}
				} else if (gotBuffObject->getEffectName(i).contains("movement"))
					creature->setSpeedMultiplierMod(creature->getSpeedMultiplierBase(), true);
				else if (gotBuffObject->getEffectName(i).contains("combat_slow")) {
					//creature->removeEffect("state_warcried");
					creature->addSkillMod(SkillModManager::BUFF, gotBuffObject->getEffectName(i), -(int)gotBuffObject->getEffectValue(i), true);
					creature->addBuff(String("player_slow_immunity").hashCode());
				} else if (!gotBuffObject->getEffectName(i).contains("movement") && !CreatureAttribute::isHAM(CreatureAttribute::getAttribute((gotBuffObject->getEffectName(i))))
					&& !gotBuffObject->getEffectName(i).isEmpty() && !gotBuffObject->getEffectName(i).contains("script_var") && !gotBuffObject->getEffectName(i).contains("Percent")
					&& !gotBuffObject->getEffectName(i).contains("delay_attack") && !gotBuffObject->getEffectName(i).contains("combat_slow") && !gotBuffObject->getEffectName(i).contains("xp_bonus")
					&& !gotBuffObject->getEffectName(i).contains("group") && !gotBuffObject->getEffectName(i).contains("proc_buff") && !gotBuffObject->getEffectName(i).contains("invis")) {
					creature->addSkillMod(SkillModManager::BUFF, gotBuffObject->getEffectName(i), -(int)gotBuffObject->getEffectValue(i), true);
					if (gotBuffObject->getEffectName(i).contains("armor_break"))
						creature->addBuff(String("player_armor_break_immunity").hashCode());
				}
			}
		}

		if (!gotBuffObject->getParticleEffect().isEmpty()) {
			if (gotBuffObject->getBuffName().contains("forcerun")) {
				// nuffin
			}
		}

	} catch (Exception& e) {
		Logger::console.error(e.getMessage());
		e.printStackTrace();
	}
}

bool BuffList::hasSpice() {
	return false;
}
