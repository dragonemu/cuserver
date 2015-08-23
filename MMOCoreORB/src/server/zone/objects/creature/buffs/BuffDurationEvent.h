/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef BUFFDURATIONEVENT_H_
#define BUFFDURATIONEVENT_H_

class BuffDurationEvent : public Task {
	ManagedReference<CreatureObject*> creatureObject;
	uint32 crc;

public:
	BuffDurationEvent(CreatureObject* creature, uint32 CRC) : Task() {
		creatureObject = creature;
		crc = CRC;
	}

	void run() {
		try {

			if (creatureObject == NULL)
				return;

			if (creatureObject->isOnline())
				creatureObject->removeBuff(crc);
		} catch (Exception& e) {
			Logger::console.error("unreported exception caught in BuffDurationEvent::activate");
		}
	}

};

#endif /* BUFFDURATIONEVENT_H_ */
