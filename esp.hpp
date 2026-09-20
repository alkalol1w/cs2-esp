#pragma once
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"
#include <vector>
#include <string>
#include <cstdio>

struct PlayerInfo {
    int health=0, team=0; bool alive=false;
    Vector3 origin{}, head{}; Vector2 screenFoot{}, screenHead{};
    std::string name; bool onScreen=false; float distance=0.0f;
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

        static int frame = 0;
        bool log = (frame++ % 200 == 0);

        if (log) {
            printf("\n=========== ESP DEBUG ===========\n");
            printf("entityList=0x%llX localPawn=0x%llX\n",
                   (unsigned long long)entityList, (unsigned long long)localPawn);
        }

        for (int i = 1; i < 16; ++i) {  // İlk 15 entity'ye bakalım
            uintptr_t listEntry = mem.Read<uintptr_t>(
                entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!listEntry) continue;

            uintptr_t controller = mem.Read<uintptr_t>(
                listEntry + 120 * (i & 0x1FF));
            if (!controller) continue;

            if (!log) continue;

            printf("\n--- i=%d ctrl=0x%llX ---\n",
                   i, (unsigned long long)controller);

            // ══════ TEST 1: Direkt pointer @ 0x814 ══════
            uintptr_t pawn1 = mem.Read<uintptr_t>(controller + 0x814);
            if (pawn1 && pawn1 > 0x10000) {
                int hp1 = mem.Read<int>(pawn1 + Offsets::m_iHealth);
                int tm1 = mem.Read<uint8_t>(pawn1 + Offsets::m_iTeamNum);
                printf("  T1 ptr@0x814=0x%llX hp=%d team=%d\n",
                       (unsigned long long)pawn1, hp1, tm1);
            }

            // ══════ TEST 2: Direkt pointer @ 0x7FC ══════
            uintptr_t pawn2 = mem.Read<uintptr_t>(controller + 0x7FC);
            if (pawn2 && pawn2 > 0x10000) {
                int hp2 = mem.Read<int>(pawn2 + Offsets::m_iHealth);
                int tm2 = mem.Read<uint8_t>(pawn2 + Offsets::m_iTeamNum);
                printf("  T2 ptr@0x7FC=0x%llX hp=%d team=%d\n",
                       (unsigned long long)pawn2, hp2, tm2);
            }

            // ══════ TEST 3: Handle + chunk lookup stride=120 ══════
            uint32_t handle3 = mem.Read<uint32_t>(controller + 0x814);
            if (handle3 && handle3 < 0x10000) {
                uintptr_t le3 = mem.Read<uintptr_t>(
                    entityList + 8 * ((handle3 & 0x7FFF) >> 9) + 0x10);
                if (le3) {
                    uintptr_t pawn3 = mem.Read<uintptr_t>(
                        le3 + 120 * (handle3 & 0x1FF));
                    if (pawn3 && pawn3 > 0x10000) {
                        int hp3 = mem.Read<int>(pawn3 + Offsets::m_iHealth);
                        int tm3 = mem.Read<uint8_t>(pawn3 + Offsets::m_iTeamNum);
                        printf("  T3 hdl=0x%X stride120 pawn=0x%llX hp=%d team=%d\n",
                               handle3, (unsigned long long)pawn3, hp3, tm3);
                    }
                }
            }

            // ══════ TEST 4: Handle + chunk lookup stride=0x70 ══════
            uint32_t handle4 = mem.Read<uint32_t>(controller + 0x814);
            if (handle4 && handle4 < 0x10000) {
                uintptr_t le4 = mem.Read<uintptr_t>(
                    entityList + 8 * ((handle4 & 0x7FFF) >> 9) + 0x10);
                if (le4) {
                    uintptr_t pawn4 = mem.Read<uintptr_t>(
                        le4 + 0x70 * (handle4 & 0x1FF));
                    if (pawn4 && pawn4 > 0x10000) {
                        int hp4 = mem.Read<int>(pawn4 + Offsets::m_iHealth);
                        int tm4 = mem.Read<uint8_t>(pawn4 + Offsets::m_iTeamNum);
                        printf("  T4 hdl=0x%X stride70 pawn=0x%llX hp=%d team=%d\n",
                               handle4, (unsigned long long)pawn4, hp4, tm4);
                    }
                }
            }
        }
    }
};
