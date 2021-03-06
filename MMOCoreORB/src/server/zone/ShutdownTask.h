/*
 * ShutdownTask.h
 *
 *  Created on: 20/05/2012
 *      Author: victor
 */

#ifndef SHUTDOWNTASK_H_
#define SHUTDOWNTASK_H_

#include "server/ServerCore.h"
#include "server/zone/ZoneServer.h"
#include "server/chat/ChatManager.h"

class ShutdownTask : public Task {
	int minutesRemaining;
	int shutdownFlags;;
	ManagedReference<ZoneServer*> zoneServer;
public:
	ShutdownTask(ZoneServer* srv, int minutes, int flags = 0) {
		zoneServer = srv;
		minutesRemaining = minutes;
		shutdownFlags = flags;
	}

	void run() {
		--minutesRemaining;

		String str = "Server will shutdown in " + String::valueOf(minutesRemaining) + " minutes";

		if (minutesRemaining <= 0)
			str = "SHUTTING DOWN NOW!";

		Logger::console.info(str, true);

		zoneServer->getChatManager()->broadcastGalaxy(nullptr, str);

		if (minutesRemaining <= 0) {
			ServerCore::getInstance()->signalShutdown(static_cast<ServerCore::ShutdownFlags>(shutdownFlags));
		} else {
			schedule(60 * 1000);
		}

	}
};


#endif /* SHUTDOWNTASK_H_ */
