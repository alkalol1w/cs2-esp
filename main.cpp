#include <Windows.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include "memory.hpp"
#include "esp.hpp"
#include "overlay.hpp"

// Ctrl+Alt+Q ile çıkış
bool g_running = true;

DWORD WINAPI HotkeyThread(LPVOID) {
    while (g_running) {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000 &&
            GetAsyncKeyState(VK_MENU)    & 0x8000 &&
            GetAsyncKeyState('Q')         & 0x8000) {
            printf("\n[CIKIS] Ctrl+Alt+Q -> kapatiliyor\n");
            g_running = false;
            ExitProcess(0);
        }
        Sleep(50);
    }
    return 0;
}

int main() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    printf("=== CS2 ESP DEBUG (Guvenli Mod) ===\n");
    printf("Ctrl+Alt+Q = cikis\n\n");

    CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);

    Memory mem;
    while (!mem.Attach(L"cs2.exe")) {
        printf("CS2 bekleniyor...\n");
        Sleep(2000);
    }
    printf("[OK] Baglandi. clientBase=0x%llX\n", (unsigned long long)mem.clientBase);

    Overlay overlay;
    if (!overlay.Init()) {
        printf("[HATA] Overlay.Init() basarisiz!\n");
        Sleep(10000);
        return 1;
    }
    printf("[OK] Overlay hazir. hwnd=0x%llX\n", (unsigned long long)overlay.hwnd);

    ESP esp;
    uintptr_t entityList = mem.Read<uintptr_t>(mem.clientBase + Offsets::dwEntityList);
    printf("[INFO] dwEntityList offset=0x%llX -> deger=0x%llX\n",
           (unsigned long long)Offsets::dwEntityList,
           (unsigned long long)entityList);

    int counter = 0;
    while (g_running) {
        overlay.PollEvents();

        uintptr_t localCtrl = mem.Read<uintptr_t>(
            mem.clientBase + Offsets::dwLocalPlayerController);
        int localTeam = localCtrl
            ? mem.Read<uint8_t>(localCtrl + Offsets::m_iTeamNum) : 0;

        esp.Update(mem, localTeam, overlay.width, overlay.height);

        if (++counter % 150 == 0) {
            printf("\n=== FRAME %d ===\n", counter);
            printf("localCtrl=0x%llX localTeam=%d oyuncu_sayisi=%zu\n",
                   (unsigned long long)localCtrl, localTeam, esp.players.size());

            for (auto& p : esp.players) {
                printf("  '%s' team=%d hp=%d onScreen=%d dist=%.1f "
                       "head=(%.0f,%.0f) foot=(%.0f,%.0f)\n",
                       p.name.c_str(), p.team, p.health,
                       p.onScreen, p.distance,
                       p.screenHead.x, p.screenHead.y,
                       p.screenFoot.x, p.screenFoot.y);
            }
        }

        overlay.Render(esp.players, localTeam);
        Sleep(5);
    }
    return 0;
}
