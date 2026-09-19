#include <Windows.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"

int main() {
    // Konsol aç
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);

    printf("=== CS2 ESP DEBUG ===\n\n");

    Memory mem;
    while (!mem.Attach(L"cs2.exe")) {
        printf("CS2 bekleniyor...\n");
        Sleep(2000);
    }

    printf("[OK] CS2'ye baglandi\n");
    printf("clientBase = 0x%llX\n", (unsigned long long)mem.clientBase);
    printf("engineBase = 0x%llX\n", (unsigned long long)mem.engineBase);
    printf("\n");

    int counter = 0;
    while (true) {
        counter++;
        printf("--- Test #%d ---\n", counter);

        // Local player pawn
        uintptr_t localPawn = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwLocalPlayerPawn);
        printf("dwLocalPlayerPawn offset = 0x%llX\n",
               (unsigned long long)Offsets::dwLocalPlayerPawn);
        printf("localPawn degeri       = 0x%llX\n",
               (unsigned long long)localPawn);

        // Local player controller
        uintptr_t localCtrl = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwLocalPlayerController);
        printf("localCtrl degeri       = 0x%llX\n",
               (unsigned long long)localCtrl);

        // Entity list
        uintptr_t entityList = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwEntityList);
        printf("entityList degeri      = 0x%llX\n",
               (unsigned long long)entityList);

        // View matrix ilk deger
        ViewMatrix vm = mem.Read<ViewMatrix>(mem.clientBase + Offsets::dwViewMatrix);
        printf("vm.m[0][0]             = %f\n", vm.m[0][0]);
        printf("vm.m[3][3]             = %f\n", vm.m[3][3]);

        // Local oyuncu bilgileri
        if (localPawn) {
            int hp = mem.Read<int>(localPawn + Offsets::m_iHealth);
            int team = mem.Read<uint8_t>(localPawn + Offsets::m_iTeamNum);
            Vector3 pos = mem.Read<Vector3>(localPawn + Offsets::m_vOldOrigin);
            printf(">> HP=%d TEAM=%d POS=(%.1f, %.1f, %.1f)\n",
                   hp, team, pos.x, pos.y, pos.z);
        } else {
            printf(">> !!! localPawn NULL - OFFSET YANLIS OLABILIR !!!\n");
        }

        // Entity list ilk birkac giris
        if (entityList) {
            printf("Entity list ornek girisler:\n");
            for (int i = 1; i < 5; ++i) {
                uintptr_t listEntry = mem.Read<uintptr_t>(
                    entityList + (8 * (i & 0x7FFF) >> 9) + 16);
                if (listEntry) {
                    uintptr_t controller = mem.Read<uintptr_t>(
                        listEntry + 120 * (i & 0x1FF));
                    printf("  [%d] listEntry=0x%llX controller=0x%llX\n",
                           i, (unsigned long long)listEntry,
                           (unsigned long long)controller);
                }
            }
        }

        printf("\n");
        Sleep(3000);
    }

    return 0;
}
