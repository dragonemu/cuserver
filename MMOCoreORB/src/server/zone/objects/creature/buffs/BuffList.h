/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef BUFFLIST_H_
#define BUFFLIST_H_

#include "engine/engine.h"

#include "server/zone/objects/creature/buffs/Buff.h"
#include "server/zone/objects/scene/variables/DeltaVectorMap.h"

namespace server {
namespace zone {
namespace objects {
namespace creature {
class CreatureObject;
}
}
}
}

using namespace server::zone::objects::creature;

class BuffList : public Singleton<BuffList>, public DeltaVectorMap<uint32, ManagedReference<Buff*> > {
public:
	VectorMap<uint32, Reference<Buff*> > buffObjectMap;

	BuffList();

	void updateBuffsToDatabase();

	void addBuff(uint32 buffCRC, CreatureObject* player, int duration);
	void removeBuff(uint32 buffCRC, CreatureObject* player);
	bool hasBuff(uint32 buffCRC);
	bool hasSpice();
	Buff* getBuff(uint32 buffCRC);

	void clearBuffs(CreatureObject* creature);

	void loadClientData();

	void applyEffects(CreatureObject* creature, Buff* buff);
	void removeEffects(CreatureObject* creature, Buff* buff);
};


#endif /* BUFFLIST_H_ */
