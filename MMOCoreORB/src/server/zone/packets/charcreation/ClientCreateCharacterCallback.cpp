/*
 * ClientCreateCharacterCallback.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: theanswer
 */

#include "ClientCreateCharacterCallback.h"

#include "ClientCreateCharacter.h"

#include "ClientCreateCharacterSuccess.h"
#include "ClientCreateCharacterFailed.h"

#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/objects/player/Races.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"


void ClientCreateCharacterCallback::parse(Message* message) {

	message->parseAscii(customization);
	message->parseUnicode(characterName);

	message->parseAscii(racefile);
	message->parseAscii(location);

	message->parseAscii(hairobj);
	message->parseAscii(haircust); //grab the hair cust data

	message->parseAscii(profession);

	isJedi = message->parseBoolean();

	height = message->parseFloat();

	message->parseUnicode(bio); //get the biography.

	tutflag = message->parseByte(); //tutorial bool.

	int raceID = Races::getRaceID(racefile);
	species = Races::getSpeciesID(raceID);
}

void ClientCreateCharacterCallback::run() {
	client->info("ClientCreateCharacterCallback::run()");

	PlayerManager* playerManager = server->getPlayerManager();
	bool success = playerManager->createPlayer(this);

	if (success) {
		client->info("success creating char");
	} else {
		client->info("failed to create char", true);
	}
}
