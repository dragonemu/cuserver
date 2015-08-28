/*
 * BuffObject.h
 *
 *  Created on: Nov 2, 2014
 *      Author: valkyra
 */

#ifndef BUFF_H_
#define BUFF_H_

#include "server/zone/templates/datatables/DataTableRow.h"
#include "server/zone/templates/datatables/DataTableIff.h"

class Buff : public Object {
protected:

	String emptyString;

	String buffName;
	String group1;
	String group2;
	int priority;
	float duration;

	String effect1Name;
	float effect1Value;
	String effect2Name;
	float effect2Value;
	String effect3Name;
	float effect3Value;
	String effect4Name;
	float effect4Value;
	String effect5Name;
	float effect5Value;

	String callBack;
	String particleEffect;

	bool debuff;

	uint64 startTime;

	uint32 buffer;

public:
	Buff() : Object() {
		buffName = "";
		group1 = "";
		group2 = "";
		priority = 0;
		duration = 0;
		effect1Name = "";
		effect2Name = "";
		effect3Name = "";
		effect4Name = "";
		effect5Name = "";

		effect1Value = 0;
		effect2Value = 0;
		effect3Value = 0;
		effect4Value = 0;
		effect5Value = 0;

		debuff = false;

		emptyString = "";

		startTime = 0;
		buffer = 0;
	}

	void parseDataTableRow(DataTableIff* iff, DataTableRow* row) {
		row->getValue(iff->getColumnIndexByName("NAME"), buffName);
		row->getValue(iff->getColumnIndexByName("GROUP1"), group1);
		row->getValue(iff->getColumnIndexByName("GROUP2"), group2);
		row->getValue(iff->getColumnIndexByName("PRIORITY"), priority);
		row->getValue(iff->getColumnIndexByName("DURATION"), duration);
		row->getValue(iff->getColumnIndexByName("EFFECT1_PARAM"), effect1Name);
		row->getValue(iff->getColumnIndexByName("EFFECT1_VALUE"), effect1Value);
		row->getValue(iff->getColumnIndexByName("EFFECT2_PARAM"), effect2Name);
		row->getValue(iff->getColumnIndexByName("EFFECT2_VALUE"), effect2Value);
		row->getValue(iff->getColumnIndexByName("EFFECT3_PARAM"), effect3Name);
		row->getValue(iff->getColumnIndexByName("EFFECT3_VALUE"), effect3Value);
		row->getValue(iff->getColumnIndexByName("EFFECT4_PARAM"), effect4Name);
		row->getValue(iff->getColumnIndexByName("EFFECT4_VALUE"), effect4Value);
		row->getValue(iff->getColumnIndexByName("EFFECT5_PARAM"), effect5Name);
		row->getValue(iff->getColumnIndexByName("EFFECT5_VALUE"), effect5Value);
		row->getValue(iff->getColumnIndexByName("CALLBACK"), callBack);
		row->getValue(iff->getColumnIndexByName("PARTICLE"), particleEffect);
		row->getValue(iff->getColumnIndexByName("DEBUFF"), debuff);
	}

	inline void setStartTime(uint64 timestamp) {
		startTime = timestamp;
	}

	inline void setBuffer(uint32 creoID) {
		buffer = creoID;
	}


	inline String& getBuffName() {
		return buffName;
	}

	inline String& getGroup1() {
		return group1;
	}

	inline String& getGroup2() {
		return group2;
	}

	inline int getPriority() {
		return priority;
	}

	inline float getDuration() {
		return duration;
	}

	inline String& getEffectName(int effectNumber) {

		switch (effectNumber) {
		case 1:
			return effect1Name;
			break;
		case 2:
			return effect2Name;
			break;
		case 3:
			return effect3Name;
			break;
		case 4:
			return effect4Name;
			break;
		case 5:
			return effect5Name;
			break;
		default:
			return emptyString;
			break;
		}
	}

	inline float getEffectValue(int effectNumber) {
		switch (effectNumber) {
		case 1:
			return effect1Value;
			break;
		case 2:
			return effect2Value;
			break;
		case 3:
			return effect3Value;
			break;
		case 4:
			return effect4Value;
			break;
		case 5:
			return effect5Value;
			break;
		default:
			return 0;
			break;
		}
	}

	inline String& getCallback() {
		return callBack;
	}

	inline String& getParticleEffect() {
		return particleEffect;
	}

	inline bool isDebuff() {
		return debuff;
	}

	bool toBinaryStream(ObjectOutputStream* stream) {
		stream->writeInt(((int)((duration > 0) ? (getRemainingDuration()) : -1)));
		stream->writeInt(0);
		stream->writeInt((int) duration);
		stream->writeLong(buffer);

		return true;
	}

	bool parseFromBinaryStream(ObjectInputStream* stream) {
		// TODO?
		return true;
	}

	float getRemainingDuration() {
		Time curTime;

		uint64 theTime = curTime.getMiliTime();

		float timeleft = round(theTime - startTime);

		float durationReturned = duration - timeleft;

		return MAX(0.0f, durationReturned);
	}
};

#endif /* BUFFOBJECT_H_ */
