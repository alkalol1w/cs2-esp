#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "esp.hpp"

constexpr COLORREF COL_ENEMY  = RGB(255, 60,  60);
constexpr COLORREF COL_TEAM   = RGB(60,  200, 255);
constexpr COLORREF COL_NAME   = RGB(255, 255, 255);

inline LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

class Overlay {
public:
    HWND hwnd   = nullptr;
    int  width  = 0;
    int  height = 0;

    bool Init() {
        width  = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);

        WNDCLASSEXW wc{};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = WndProc;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"CS2_ESP_OVR";
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        RegisterClassExW(&wc);

        hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            L"CS2_ESP_OVR", L"",
            WS_POPUP,
            0, 0, width, height,
            nullptr, nullptr, wc.hInstance, nullptr
        );
        if (!hwnd) return false;

        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        return true;
    }

    void Render(const std::vector<PlayerInfo>& players, int localTeam) {
        if (!hwnd) return;

        HDC hdc = GetDC(hwnd);

        // Siyah arka plan -> colorkey ile şeffaf
        HBRUSH bg = CreateSolidBrush(RGB(50, 50, 50));  // TEST için gri
        RECT rc{ 0, 0, width, height };
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        for (const auto& p : players) {
            if (!p.onScreen) continue;

            COLORREF col = (p.team == localTeam) ? COL_TEAM : COL_ENEMY;

            float boxH = p.screenFoot.y - p.screenHead.y;
            if (boxH <= 1.0f) continue;

            float boxW = boxH * 0.45f;
            float bx   = p.screenHead.x - boxW * 0.5f;
            float by   = p.screenHead.y;

            // Bounding box
            HPEN pen = CreatePen(PS_SOLID, 1, col);
            HGDIOBJ oldPen   = SelectObject(hdc, pen);
            HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc,
                      (int)bx,        (int)by,
                      (int)(bx+boxW), (int)(by+boxH));
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(pen);

            // Health bar
            DrawHealthBar(hdc, (int)bx - 6, (int)by, (int)boxH, p.health);

            // İsim + mesafe
            SetTextColor(hdc, COL_NAME);
            SetBkMode(hdc, TRANSPARENT);
            std::string label = p.name + "  " +
                                std::to_string((int)p.distance) + "m";
            int textX = (int)(p.screenHead.x - (int)label.size() * 3);
            TextOutA(hdc, textX, (int)by - 16, label.c_str(), (int)label.size());
        }

        ReleaseDC(hwnd, hdc);
    }

    void PollEvents() {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    ~Overlay() {
        if (hwnd) DestroyWindow(hwnd);
    }

private:
    void DrawHealthBar(HDC hdc, int x, int y, int height, int hp) {
        if (height <= 0) return;
        if (hp > 100) hp = 100;
        if (hp < 0)   hp = 0;

        // Arka plan (gri çerçeve)
        HPEN grayPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
        HGDIOBJ oldPen   = SelectObject(hdc, grayPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, x, y, x + 4, y + height);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(grayPen);

        // Dolgu
        float ratio = hp / 100.0f;
        int   fillH = (int)(height * ratio);

        COLORREF hpCol;
        if (hp > 60)      hpCol = RGB(60, 255, 60);
        else if (hp > 30) hpCol = RGB(255, 200, 0);
        else              hpCol = RGB(255, 60, 60);

        HBRUSH hpBrush = CreateSolidBrush(hpCol);
        RECT   hpRect  = { x + 1, y + (height - fillH), x + 3, y + height };
        FillRect(hdc, &hpRect, hpBrush);
        DeleteObject(hpBrush);
    }
};
