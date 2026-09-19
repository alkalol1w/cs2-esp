#pragma once
#include <cstdint>

namespace Offsets {
    // client.dll — 2026-09-10 dumper çıktısı
    constexpr uintptr_t dwEntityList              = 0x2577BE0;
    constexpr uintptr_t dwLocalPlayerPawn         = 0x23CCC08;
    constexpr uintptr_t dwLocalPlayerController   = 0x23A78D0;
    constexpr uintptr_t dwViewMatrix              = 0x23D21F0;
    constexpr uintptr_t dwGlowManager             = 0x23C93F8;

    // Entity offsets (C_BaseEntity / C_CSPlayerPawn)
    constexpr uintptr_t m_iHealth                 = 0x344;
    constexpr uintptr_t m_iTeamNum                = 0x3E3;
    constexpr uintptr_t m_lifeState               = 0x338;
    constexpr uintptr_t m_vOldOrigin              = 0x127C;
    constexpr uintptr_t m_pGameSceneNode          = 0x320;
    constexpr uintptr_t m_modelState              = 0x160;
    constexpr uintptr_t m_boneArray               = 0x80;
    constexpr uintptr_t m_iIDEntIndex             = 0x7E4;
    constexpr uintptr_t m_iszPlayerName           = 0x660;
    constexpr uintptr_t m_hPlayerPawn             = 0x7E4;
}
