#include <Windows.h>
#include <thread>
#include <chrono>
#include "memory.hpp"
#include "esp.hpp"
#include "overlay.hpp"

int main() {
    Memory mem;
    while (!mem.Attach(L"cs2.exe")) {
        Sleep(2000);
    }

    Overlay overlay;
    if (!overlay.Init()) return 1;

    ESP esp;

    auto getLocalTeam = [&]() -> int {
        uintptr_t localCtrl = mem.Read<uintptr_t>(
            mem.clientBase + Offsets::dwLocalPlayerController);
        if (!localCtrl) return 0;
        return mem.Read<uint8_t>(localCtrl + Offsets::m_iTeamNum);
    };

    while (true) {
        overlay.PollEvents();

        // Pencereyi en üstte tut
        SetWindowPos(overlay.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        int localTeam = getLocalTeam();
        esp.Update(mem, localTeam, overlay.width, overlay.height);
        overlay.Render(esp.players, localTeam);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}
