/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.
 */

#include "server/zone/managers/gcw/observers/SquadObserver.h"

#include "server/zone/managers/combat/CombatManager.h"
#include "server/zone/objects/creature/ai/AiAgent.h"
#include "server/zone/objects/tangible/TangibleObject.h"

void SquadObserverImplementation::addMember(AiAgent* member) {
	Locker locker(&containmentTeamLock);
	teamMembers.add(member);
}

int SquadObserverImplementation::size() {
	Locker locker(&containmentTeamLock);
	return teamMembers.size();
}

AiAgent* SquadObserverImplementation::getMember(unsigned int teamMemberIndex) {
	Locker locker(&containmentTeamLock);
	if (teamMemberIndex < teamMembers.size()) {
		return teamMembers.get(teamMemberIndex);
	} else {
		return nullptr;
	}
}

void SquadObserverImplementation::removeMember(unsigned int teamMemberIndex) {
	Locker locker(&containmentTeamLock);
	if (teamMemberIndex < teamMembers.size()) {
		teamMembers.remove(teamMemberIndex);
	}
}

bool SquadObserverImplementation::despawnMembersCloseToLambdaShuttle(const Vector3& landingPosition, bool forcedCleanup) {
	// Do not lock containmentTeamLock in this method to avoid deadlocks. Use the minimal locking methods above.
	for (int i = size() - 1; i >= 0; i--) {
		auto npc = getMember(i);
		if (npc != nullptr) {
			Locker npcLock(npc);
			auto distance = npc->getWorldPosition().squaredDistanceTo(landingPosition);
			if (npc->isDead()) {
				removeMember(i);
				continue;
			} else if (!npc->isInCombat()) {
				npc->getCooldownTimerMap()->updateToCurrentAndAddMili("reaction_chat", 60000);

				npc->eraseBlackboard("formationOffset");
				npc->setFollowObject(nullptr);

				npc->clearPatrolPoints();
				npc->setNextPosition(landingPosition.getX(), landingPosition.getZ(), landingPosition.getY());

				if (forcedCleanup)
					npc->leash();

				if (distance < 8 * 8) {
					npc->destroyObjectFromWorld(true);
					removeMember(i);
				}
			}
		} else {
			removeMember(i);
		}
	}
	return teamMembers.size() == 0;
}
