/*
 * WeaponObjectImplementation.cpp
 *
 *  Created on: 30/07/2009
 *      Author: victor
 */

#include "server/zone/objects/tangible/weapon/WeaponObject.h"
#include "server/zone/packets/tangible/WeaponObjectMessage3.h"
#include "server/zone/packets/tangible/WeaponObjectMessage6.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/scene/AttributeListMessage.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/templates/tangible/SharedWeaponObjectTemplate.h"
#include "server/zone/managers/templates/TemplateManager.h"
#include "server/zone/objects/manufactureschematic/craftingvalues/CraftingValues.h"
#include "server/zone/objects/tangible/powerup/PowerupObject.h"
#include "server/zone/objects/tangible/component/lightsaber/LightsaberCrystalComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/packets/object/WeaponRanges.h"
#include "server/zone/packets/tangible/TangibleObjectDeltaMessage3.h"
#include "server/zone/objects/player/sessions/SlicingSession.h"
#include "server/zone/Zone.h"



void WeaponObjectImplementation::initializeTransientMembers() {
	TangibleObjectImplementation::initializeTransientMembers();

	weaponTemplate = dynamic_cast<SharedWeaponObjectTemplate*>(TemplateManager::instance()->getTemplate(serverObjectCRC));

	setLoggingName("WeaponObject");

	if(damageSlice > 1.5 || damageSlice < 1) {
		damageSlice = 1;
	}

	if(speedSlice > 1.0 || speedSlice < .5) {
		speedSlice = 1;
	}
}

void WeaponObjectImplementation::loadTemplateData(SharedObjectTemplate* templateData) {
	TangibleObjectImplementation::loadTemplateData(templateData);

	weaponTemplate = dynamic_cast<SharedWeaponObjectTemplate*>(templateData);

	certified = false;

	attackType = weaponTemplate->getAttackType();
	weaponEffect =  weaponTemplate->getWeaponEffect();
	weaponEffectIndex = weaponTemplate->getWeaponEffectIndex();

	damageType = weaponTemplate->getDamageType();

	armorPiercing = weaponTemplate->getArmorPiercing();

	healthAttackCost = weaponTemplate->getHealthAttackCost();
	actionAttackCost = weaponTemplate->getActionAttackCost();
	mindAttackCost = weaponTemplate->getMindAttackCost();
	forceCost = weaponTemplate->getForceCost();

	pointBlankAccuracy = weaponTemplate->getPointBlankAccuracy();
	pointBlankRange = weaponTemplate->getPointBlankRange();

	idealRange = weaponTemplate->getIdealRange();
	idealAccuracy = weaponTemplate->getIdealAccuracy();

	int templateMaxRange = weaponTemplate->getMaxRange();

	if (templateMaxRange > 5 )
		maxRange = templateMaxRange;

	maxRangeAccuracy = weaponTemplate->getMaxRangeAccuracy();

	minDamage = weaponTemplate->getMinDamage();
	maxDamage = weaponTemplate->getMaxDamage();

	woundsRatio = weaponTemplate->getWoundsRatio();

	damageRadius = weaponTemplate->getArea();

	float templateAttackSpeed = weaponTemplate->getAttackSpeed();

	if (templateAttackSpeed > 1)
		attackSpeed = templateAttackSpeed;

	if (!isJediWeapon()) {
		setSliceable(true);
	} else if (isJediWeapon()) {
		setSliceable(false);
	}
}

void WeaponObjectImplementation::sendContainerTo(CreatureObject* player) {

	if (isJediWeapon()) {

		ManagedReference<SceneObject*> saberInv = getSlottedObject("saber_inv");

		if (saberInv != NULL) {
			saberInv->sendDestroyTo(player);
			//saberInv->closeContainerTo(player, true);

			saberInv->sendWithoutContainerObjectsTo(player);
			saberInv->openContainerTo(player);
		}

	}
}

void WeaponObjectImplementation::createChildObjects() {
	// Create any child objects in a weapon.

	ZoneServer* zoneServer = server->getZoneServer();

		for (int i = 0; i < templateObject->getChildObjectsSize(); ++i) {
			ChildObject* child = templateObject->getChildObject(i);

			if (child == NULL)
				continue;

			ManagedReference<SceneObject*> obj = zoneServer->createObject(
					child->getTemplateFile().hashCode(), 1);

			if (obj == NULL)
				continue;

			ContainerPermissions* permissions = obj->getContainerPermissions();
			permissions->setOwner(getObjectID());
			permissions->setInheritPermissionsFromParent(true);
			permissions->setDefaultDenyPermission(ContainerPermissions::MOVECONTAINER);
			permissions->setDenyPermission("owner", ContainerPermissions::MOVECONTAINER);

			if (!transferObject(obj, child->getContainmentType())) {
				obj->destroyObjectFromDatabase(true);
				continue;
			}

			childObjects.put(obj);

			obj->initializeChildObject(_this.getReferenceUnsafeStaticCast());
		}

}

void WeaponObjectImplementation::sendBaselinesTo(SceneObject* player) {
	info("sending weapon object baselines");

	BaseMessage* weao3 = new WeaponObjectMessage3(_this.getReferenceUnsafeStaticCast());
	player->sendMessage(weao3);

	BaseMessage* weao6 = new WeaponObjectMessage6(_this.getReferenceUnsafeStaticCast());
	player->sendMessage(weao6);

	/*if (player->isCreatureObject()) {
		BaseMessage* ranges = new WeaponRanges(cast<CreatureObject*>(player), _this.getReferenceUnsafeStaticCast());
		player->sendMessage(ranges);
	}*/
}

String WeaponObjectImplementation::getWeaponType() {
	int weaponObjectType = getGameObjectType();

	String weaponType;

	switch (weaponObjectType) {
	case SceneObjectType::ONEHANDMELEEWEAPON:
		weaponType = "onehandmelee";
		break;
	case SceneObjectType::TWOHANDMELEEWEAPON:
		weaponType = "twohandmelee";
		break;
	case SceneObjectType::WEAPON:
	case SceneObjectType::MELEEWEAPON:
		weaponType = "unarmed";
		break;
	case SceneObjectType::RIFLE:
		weaponType = "rifle";
		break;
	case SceneObjectType::PISTOL:
		weaponType = "pistol";
		break;
	case SceneObjectType::CARBINE:
		weaponType = "carbine";
		break;
	case SceneObjectType::POLEARM:
		weaponType = "polearm";
		break;
	case SceneObjectType::THROWNWEAPON:
		weaponType = "thrownweapon";
		break;
	case SceneObjectType::MINE:
	case SceneObjectType::SPECIALHEAVYWEAPON:
	case SceneObjectType::HEAVYWEAPON:
		weaponType = "heavyweapon";
		break;
	default:
		weaponType = "unarmed";
		break;
	}

	if (isJediOneHandedWeapon()) weaponType = "onehandlightsaber";
	if (isJediTwoHandedWeapon()) weaponType = "twohandlightsaber";
	if (isJediPolearmWeapon()) weaponType = "polearmlightsaber";

	return weaponType;
}

void WeaponObjectImplementation::fillAttributeList(AttributeListMessage* alm, CreatureObject* object) {
	TangibleObjectImplementation::fillAttributeList(alm, object);

	for(int i = 0; i < wearableSkillMods.size(); ++i) {
		String key = wearableSkillMods.elementAt(i).getKey();
		String statname = "cat_skill_mod_bonus.@stat_n:" + key;
		int value = wearableSkillMods.get(key);

		if (value > 0)
			alm->insertAttribute(statname, value);
	}

	// Dreadlow - ADK Notice goes here
	//			- "Bio Link:" for some weapons too, if I'm not mistaken
	// Force Cost
	if (getForceCost() > 0)
		alm->insertAttribute("forcecost", getForceCost());
	//alm->insertAttribute("healing_combat_level_required", levelRequired);
	alm->insertAttribute("skillmodmin", "None"); // Pull this

	// Dreadlow - "Required Profession:" here

	//Damage Information
	StringBuffer dmgtxt;

	switch (damageType) {
	case KINETIC:
		dmgtxt << "Kinetic";
		break;
	case ENERGY:
		dmgtxt << "Energy";
		break;
	case ELECTRICITY:
		dmgtxt << "Electricity";
		break;
	case STUN:
		dmgtxt << "Stun";
		break;
	case BLAST:
		dmgtxt << "Blast";
		break;
	case HEAT:
		dmgtxt << "Heat";
		break;
	case COLD:
		dmgtxt << "Cold";
		break;
	case ACID:
		dmgtxt << "Acid";
		break;
	case LIGHTSABER:
		dmgtxt << "Lightsaber";
		break;
	default:
		dmgtxt << "Unknown";
		break;
	}

	alm->insertAttribute("cat_wpn_damage.wpn_damage_type", dmgtxt);
	alm->insertAttribute("cat_wpn_damage.wpn_attack_speed", getAttackSpeed(true));
	//alm->insertAttribute("cat_wpn_damage.wpn_real_speed", getModifiedSpeed(object));
	alm->insertAttribute("cat_wpn_damage.damage", String::valueOf(getMinDamage(true)) + " - " + String::valueOf(getMaxDamage(true)));
	//alm->insertAttribute("cat_wpn_damage.wpn_accuracy", getAccuracyBonus(object));
	alm->insertAttribute("cat_wpn_damage.wpn_wound_chance", getWoundsRatio(true));
	//alm->insertAttribute("cat_wpn_damage.wpn_base_dps", String::valueOf(getBaseDPS(object)) + " / sec");
//	alm->insertAttribute("cat_wpn_damage.wpn_real_dps", String::valueOf(getModifiedDPS(object)) + " / sec");

	alm->insertAttribute("cat_wpn_other.wpn_range", "0 - " + String::valueOf(getMaxRange(true)) + "m");
	//alm->insertAttribute("cat_wpn_other.attackcost", getSpecialActionCost());

	if (hasPowerup())
		powerupObject->fillWeaponAttributeList(alm, _this.get());

	if (sliced == 1)
		alm->insertAttribute("cat_wpn_other.wpn_attr", "@obj_attr_n:hacked1");

}

int WeaponObjectImplementation::getPointBlankAccuracy(bool withPup) {

	if(powerupObject != NULL && withPup)
		return pointBlankAccuracy + (abs(pointBlankAccuracy) * powerupObject->getPowerupStat("pointBlankAccuracy"));

	return pointBlankAccuracy;
}

int WeaponObjectImplementation::getPointBlankRange(bool withPup) {

	if(powerupObject != NULL && withPup)
		return pointBlankRange + (abs(pointBlankRange) * powerupObject->getPowerupStat("pointBlankRange"));

	return pointBlankRange;
}

int WeaponObjectImplementation::getIdealRange(bool withPup) {

	if(powerupObject != NULL && withPup)
		return idealRange + (abs(idealRange) * powerupObject->getPowerupStat("idealRange"));

	return idealRange;
}

int WeaponObjectImplementation::getMaxRange(bool withPup) {

	if(powerupObject != NULL && withPup)
		return maxRange + (abs(maxRange) * powerupObject->getPowerupStat("maxRange"));

	return maxRange;
}

int WeaponObjectImplementation::getIdealAccuracy(bool withPup) {

	if(powerupObject != NULL && withPup)
		return idealAccuracy + (abs(idealAccuracy) * powerupObject->getPowerupStat("idealAccuracy"));

	return idealAccuracy;
}


int WeaponObjectImplementation::getMaxRangeAccuracy(bool withPup) {

	if(powerupObject != NULL && withPup)
		return maxRangeAccuracy + (abs(maxRangeAccuracy) * powerupObject->getPowerupStat("maxRangeAccuracy"));

	return maxRangeAccuracy;
}

float WeaponObjectImplementation::getAttackSpeed(bool withPup) {

	float speed = attackSpeed;

	if(sliced)
		speed *= speedSlice;

	if(powerupObject != NULL && withPup)
		speed -= (speed * powerupObject->getPowerupStat("attackSpeed"));

	float calcSpeed = speed + getConditionReduction(speed);

	if(calcSpeed < 0.1)
		calcSpeed = 0.1;

	return calcSpeed;
}


float WeaponObjectImplementation::getMaxDamage(bool withPup) {

	float damage = maxDamage;

	if(sliced)
		damage *= damageSlice;

	if(powerupObject != NULL && withPup) {
		damage += (damage * powerupObject->getPowerupStat("maxDamage"));
		return damage - getConditionReduction(damage);
	}

	return damage - getConditionReduction(damage);
}

float WeaponObjectImplementation::getMinDamage(bool withPup) {

	float damage = minDamage;

	if(sliced)
		damage *= damageSlice;

	if(powerupObject != NULL && withPup) {
		damage += (damage * powerupObject->getPowerupStat("minDamage"));
		return damage - getConditionReduction(damage);
	}

	return damage - getConditionReduction(damage);
}

float WeaponObjectImplementation::getWoundsRatio(bool withPup) {

	if(powerupObject != NULL && withPup)
		return woundsRatio + (woundsRatio * powerupObject->getPowerupStat("woundsRatio"));

	return woundsRatio;
}

float WeaponObjectImplementation::getDamageRadius(bool withPup) {

	if(powerupObject != NULL && withPup)
		return damageRadius + (damageRadius * powerupObject->getPowerupStat("damageRadius"));

	return damageRadius;
}


int WeaponObjectImplementation::getHealthAttackCost(bool withPup) {

	if(powerupObject != NULL && withPup)
		return healthAttackCost - (abs(healthAttackCost) * powerupObject->getPowerupStat("healthAttackCost"));

	return healthAttackCost;
}

int WeaponObjectImplementation::getActionAttackCost(bool withPup) {

	if(powerupObject != NULL && withPup)
		return actionAttackCost - (abs(actionAttackCost) * powerupObject->getPowerupStat("actionAttackCost"));

	return actionAttackCost;
}

int WeaponObjectImplementation::getMindAttackCost(bool withPup) {

	if(powerupObject != NULL && withPup)
		return mindAttackCost - (abs(mindAttackCost) * powerupObject->getPowerupStat("mindAttackCost"));

	return mindAttackCost;
}

void WeaponObjectImplementation::updateCraftingValues(CraftingValues* values, bool firstUpdate) {
	/*
	 * Incoming Values:					Ranges:
	 * mindamage						Differs between weapons
	 * maxdamage
	 * attackspeed
	 * woundchance
	 * roundsused
	 * hitpoints
	 * zerorangemod
	 * maxrange
	 * maxrangemod
	 * midrange
	 * midrangemod
	 * charges
	 * attackhealthcost
	 * attackactioncost
	 * attackmindcost
	 */
	float value = 0.f;
	setMinDamage(MAX(values->getCurrentValue("mindamage"), 0));
	setMaxDamage(MAX(values->getCurrentValue("maxdamage"), 0));

	setAttackSpeed(values->getCurrentValue("attackspeed"));
	setHealthAttackCost((int)values->getCurrentValue("attackhealthcost"));
	setActionAttackCost((int)values->getCurrentValue("attackactioncost"));
	setMindAttackCost((int)values->getCurrentValue("attackmindcost"));

	if (isJediWeapon()) {
		setForceCost((int)values->getCurrentValue("forcecost"));
		setBladeColor(31);
	}

	value = values->getCurrentValue("woundchance");
	if(value != CraftingValues::VALUENOTFOUND)
		setWoundsRatio(value);

	//value = craftingValues->getCurrentValue("roundsused");
	//if(value != DraftSchematicValuesImplementation::VALUENOTFOUND)
		//_this.getReferenceUnsafeStaticCast()->set_______(value);

	value = values->getCurrentValue("zerorangemod");
	if(value != CraftingValues::VALUENOTFOUND)
		setPointBlankAccuracy((int)value);

	value = values->getCurrentValue("maxrange");
	if(value != CraftingValues::VALUENOTFOUND)
		setMaxRange((int)value);

	value = values->getCurrentValue("maxrangemod");
	if(value != CraftingValues::VALUENOTFOUND)
		setMaxRangeAccuracy((int)value);

	value = values->getCurrentValue("midrange");
	if(value != CraftingValues::VALUENOTFOUND)
		setIdealRange((int)value);

	value = values->getCurrentValue("midrangemod");
	if(value != CraftingValues::VALUENOTFOUND)
		setIdealAccuracy((int)value);

	//value = craftingValues->getCurrentValue("charges");
	//if(value != CraftingValues::VALUENOTFOUND)
	//	setUsesRemaining((int)value);

	value = values->getCurrentValue("hitpoints");
	if(value != CraftingValues::VALUENOTFOUND)
		setMaxCondition((int)value);

	setConditionDamage(0);
}

bool WeaponObjectImplementation::isCertifiedFor(CreatureObject* object) {
	ManagedReference<PlayerObject*> ghost = object->getPlayerObject();

	if (ghost == NULL)
		return false;

	Vector<String>* certificationsRequired = weaponTemplate->getCertificationsRequired();

	for (int i = 0; i < certificationsRequired->size(); ++i) {
		String cert = certificationsRequired->get(i);

		if (!ghost->hasAbility(cert) && !object->hasSkill(cert)) {
			return false;
		}
	}

	return true;
}

void WeaponObjectImplementation::decreasePowerupUses(CreatureObject* player) {
	if (hasPowerup()) {
		powerupObject->decreaseUses();
		if (powerupObject->getUses() < 1) {
			Locker locker(_this.getReferenceUnsafeStaticCast());
			StringIdChatParameter message("powerup", "prose_pup_expire"); //The powerup on your %TT has expired.
			message.setTT(getDisplayedName());

			player->sendSystemMessage(message);

			powerupObject = NULL;
		}
		sendAttributeListTo(player);
	}
}

String WeaponObjectImplementation::repairAttempt(int repairChance) {

	String message = "@error_message:";

	if(repairChance < 25) {
		message += "sys_repair_failed";
		setMaxCondition(1, true);
		setConditionDamage(0, true);
	} else if(repairChance < 50) {
		message += "sys_repair_imperfect";
		setMaxCondition(getMaxCondition() * .65f, true);
		setConditionDamage(0, true);
	} else if(repairChance < 75) {
		setMaxCondition(getMaxCondition() * .80f, true);
		setConditionDamage(0, true);
		message += "sys_repair_slight";
	} else {
		setMaxCondition(getMaxCondition() * .95f, true);
		setConditionDamage(0, true);
		message += "sys_repair_perfect";
	}

	return message;
}

void WeaponObjectImplementation::decay(CreatureObject* user) {
	if (_this.getReferenceUnsafeStaticCast() == user->getSlottedObject("default_weapon") || user->isAiAgent() || hasAntiDecayKit()) {
		return;
	}

	int chance = System::random(100);

	if (chance < 5 || ((isSliced() || hasPowerup()) && chance < 15)) {
		if (isJediWeapon()) {
			ManagedReference<SceneObject*> saberInv = getSlottedObject("saber_inv");

			// TODO: is this supposed to be every crystal, or random crystal(s)?
			for (int i = 0; i < saberInv->getContainerObjectsSize(); i++) {
				ManagedReference<LightsaberCrystalComponent*> crystal = saberInv->getContainerObject(i).castTo<LightsaberCrystalComponent*>();

				if (crystal != NULL) {
					crystal->inflictDamage(crystal, 0, 1, true, true);
				}
			}
		} else {
			inflictDamage(_this.getReferenceUnsafeStaticCast(), 0, 1, true, true);

			if (((float)conditionDamage - 1 / (float)maxCondition < 0.75) && ((float)conditionDamage / (float)maxCondition > 0.75))
				user->sendSystemMessage("@combat_effects:weapon_quarter");
			if (((float)conditionDamage - 1 / (float)maxCondition < 0.50) && ((float)conditionDamage / (float)maxCondition > 0.50))
				user->sendSystemMessage("@combat_effects:weapon_half");
		}
	}
}

bool WeaponObjectImplementation::isEquipped() {
	ManagedReference<SceneObject*> parent = getParent().get();
	if (parent != NULL && parent->isPlayerCreature())
		return true;

	return false;
}

void WeaponObjectImplementation::applySkillModsTo(CreatureObject* creature) {
	if (creature == NULL) {
		return;
	}

	for (int i = 0; i < wearableSkillMods.size(); ++i) {
		String name = wearableSkillMods.elementAt(i).getKey();
		int value = wearableSkillMods.get(name);

		if (!SkillModManager::instance()->isWearableModDisabled(name))
			creature->addSkillMod(SkillModManager::WEARABLE, name, value, true);
	}

	SkillModManager::instance()->verifyWearableSkillMods(creature);
}

void WeaponObjectImplementation::removeSkillModsFrom(CreatureObject* creature) {
	if (creature == NULL) {
		return;
	}

	for (int i = 0; i < wearableSkillMods.size(); ++i) {
		String name = wearableSkillMods.elementAt(i).getKey();
		int value = wearableSkillMods.get(name);

		if (!SkillModManager::instance()->isWearableModDisabled(name))
			creature->removeSkillMod(SkillModManager::WEARABLE, name, value, true);
	}

	SkillModManager::instance()->verifyWearableSkillMods(creature);
}

bool WeaponObjectImplementation::applyPowerup(CreatureObject* player, PowerupObject* pup) {
	if(hasPowerup())
		return false;

	addMagicBit(true);

	powerupObject = pup;

	if(pup->getParent() != NULL) {
		Locker clocker(pup, player);
		pup->destroyObjectFromWorld(true);
	}

	sendAttributeListTo(player);

	return true;
}

PowerupObject* WeaponObjectImplementation::removePowerup() {
	if(!hasPowerup())
		return NULL;

	PowerupObject* pup = powerupObject;
	powerupObject = NULL;

	removeMagicBit(true);

	return pup;
}
