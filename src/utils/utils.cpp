/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2026 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"
#include "../cs2fixes.h"
#include "../gameconfig.h"
#include "../addresses.h"
#include "engine2/isource2engine.h"
#include "engine2/inetworkserverservice.h"
#include "engine2/ivengineserver2.h"

void Message(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	ConColorMsg(Color(255, 0, 255, 255), "[CS2Fixes] %s", buf);

	va_end(args);
}

void Panic(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	Warning("[CS2Fixes] %s", buf);

	va_end(args);
}

CUtlVector<CServerSideClient*>* GetClientList()
{
	auto* server = GetNetworkGameServer();
	const int offset = g_GameConfig ? g_GameConfig->GetClientListOffset() : -1;
	if (!server || offset <= 0)
		return nullptr;

	return reinterpret_cast<CUtlVector<CServerSideClient*>*>(reinterpret_cast<char*>(server) + offset);
}

// The manager interface is the first base of CSpawnGroupMgrGameSystem.
// Use its full vector-out accessor, including already loaded spawn groups.
bool GetActualSpawnGroups(CUtlVector<SpawnGroupHandle_t>& groups)
{
	auto* manager = g_pNetworkServerService ? g_pNetworkServerService->GetGameSpawnGroupMgr() : nullptr;
	if (!manager || !addresses::GetSpawnGroups)
		return false;
	addresses::GetSpawnGroups(manager, &groups);
	return true;
}

CServerSideClient* GetClientBySlot(CPlayerSlot slot)
{
	CUtlVector<CServerSideClient*>* pClients = GetClientList();

	if (!pClients || slot.Get() < 0 || slot.Get() >= pClients->Count())
		return nullptr;

	return pClients->Element(slot.Get());
}

uint32 GetSoundEventHash(const char* pszSoundEventName)
{
	return MurmurHash2LowerCase(pszSoundEventName, 0x53524332);
}

std::string StringToLower(std::string strValue)
{
	for (int i = 0; strValue[i]; i++)
		strValue[i] = tolower(strValue[i]);

	return strValue;
}

ISteamUGC* GetSteamUGC()
{
	if (g_pEngineServer2->IsDedicatedServer())
		return SteamGameServerUGC();
	else
		return SteamUGC();
}

ISteamHTTP* GetSteamHTTP()
{
	if (g_pEngineServer2->IsDedicatedServer())
		return SteamGameServerHTTP();
	else
		return SteamHTTP();
}