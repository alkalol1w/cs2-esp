#pragma once
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"
#include <vector>
#include <string>
#include <cstdio>

struct PlayerInfo {
    int         health   = 0;
    int         team     = 0;
    bool        alive    = false;
    Vector3     origin{};
    Vector3     head{};
    Vector2     screenFoot{};
    Vector2     screenHead{};
    std::string name;
    bool        onScreen = false;
    float       distance = 0.0f;
};

class ESP {
public:
    std::vector<PlayerInfo> players;

    void Update(const Memory& mem, int localTeam, int screenW, int screenH) {
        players.clear();
        if (!mem.IsAttached() || !mem.clientBase) return;

        ViewMatrix vm = mem.Read<ViewMatrix>(mem.clientBase + Offsets::dwViewMatrix);
        uintptr_t localPawn  = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwLocalPlayerPawn);
        uintptr_t entityList = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwEntityList);
        if (!entityList) return;

        Vector3 localPos{};
        if (localPawn) localPos = mem.Read<Vector3>(localPawn + Offsets::m_vOldOrigin);

        for (int i = 1; i < 64; ++i) {
            // ADIM 1: Chunk base (düzeltildi: 0x10 -> 0x8)
            uintptr_t listEntry = mem.Read<uintptr_t>(
                entityList + 8 * (i >> 9) + 0x8);
            if (!listEntry) continue;

            // ADIM 2: Controller (düzeltildi: 120 -> 0x70)
            uintptr_t controller = mem.Read<uintptr_t>(
                listEntry + 0x70 * (i & 0x1FF));
            if (!controller) continue;

            // ADIM 3: Pawn handle
            uint32_t pawnHandle = mem.Read<uint32_t>(
                controller + Offsets::m_hPlayerPawn);
            if (pawnHandle == 0) continue;

            // ADIM 4: İkinci chunk (düzeltildi: 0x10 -> 0x8)
            uintptr_t listEntry2 = mem.Read<uintptr_t>(
                entityList + 8 * ((pawnHandle & 0x7FFF) >> 9) + 0x8);
            if (!listEntry2) continue;

            // ADIM 5: Pawn (düzeltildi: 120 -> 0x70)
            uintptr_t pawn = mem.Read<uintptr_t>(
                listEntry2 + 0x70 * (pawnHandle & 0x1FF));
            if (!pawn || pawn == localPawn) continue;

            // ADIM 6: Veriler
            int      hp        = mem.Read<int>(pawn + Offsets::m_iHealth);
            int      team      = mem.Read<uint8_t>(pawn + Offsets::m_iTeamNum);
            uint8_t  lifeState = mem.Read<uint8_t>(pawn + Offsets::m_lifeState);

            // Filtre: Canlı ve geçerli takımda mı?
            if (lifeState != 0 || hp <= 0 || hp > 100) continue;
            if (team != 2 && team != 3) continue;

            PlayerInfo info{};
            info.health = hp;
            info.team   = team;
            info.alive  = true;

            info.name = mem.ReadString(controller + Offsets::m_iszPlayerName);
            if (info.name.empty()) info.name = "?";

            info.origin = mem.Read<Vector3>(pawn + Offsets::m_vOldOrigin);
            info.head   = { info.origin.x, info.origin.y, info.origin.z + 72.0f };

            info.onScreen =
                WorldToScreen(info.origin, info.screenFoot, vm, screenW, screenH) &&
                WorldToScreen(info.head,   info.screenHead, vm, screenW, screenH);

            Vector3 delta = info.origin - localPos;
            info.distance = delta.Length() * 0.01905f;

            players.push_back(info);
        }
    }
};
