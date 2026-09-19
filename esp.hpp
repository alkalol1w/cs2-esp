#pragma once
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"
#include <vector>
#include <string>

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
        if (localPawn) {
            localPos = mem.Read<Vector3>(localPawn + Offsets::m_vOldOrigin);
        }

        for (int i = 1; i < 64; ++i) {
            uintptr_t listEntry = mem.Read<uintptr_t>(
                entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!listEntry) continue;

            uintptr_t controller = mem.Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
            if (!controller) continue;

            uint32_t pawnHandle = mem.Read<uint32_t>(controller + Offsets::m_hPlayerPawn);
            if (pawnHandle == 0) continue;

            uintptr_t listEntry2 = mem.Read<uintptr_t>(
                entityList + (8 * ((pawnHandle & 0x7FFF) >> 9)) + 16);
            if (!listEntry2) continue;

            uintptr_t pawn = mem.Read<uintptr_t>(listEntry2 + 120 * (pawnHandle & 0x1FF));
            if (!pawn || pawn == localPawn) continue;

            PlayerInfo info{};

            info.health = mem.Read<int>(pawn + Offsets::m_iHealth);
            info.team   = mem.Read<uint8_t>(pawn + Offsets::m_iTeamNum);

            // CS2: LIFE_ALIVE = 0
            uint8_t lifeState = mem.Read<uint8_t>(pawn + Offsets::m_lifeState);
            info.alive = (lifeState == 0) && (info.health > 0) && (info.health <= 100);
            if (!info.alive) continue;

            info.name   = mem.ReadString(controller + Offsets::m_iszPlayerName);
            if (info.name.empty()) info.name = "player";

            info.origin = mem.Read<Vector3>(pawn + Offsets::m_vOldOrigin);
            // Basit head tahmini (bone okuma istersen genişletilebilir)
            info.head = { info.origin.x, info.origin.y, info.origin.z + 72.0f };

            info.onScreen =
                WorldToScreen(info.origin, info.screenFoot, vm, screenW, screenH) &&
                WorldToScreen(info.head,   info.screenHead, vm, screenW, screenH);

            Vector3 delta = info.origin - localPos;
            info.distance = delta.Length() * 0.01905f; // units -> metre

            players.push_back(info);
        }
    }
};