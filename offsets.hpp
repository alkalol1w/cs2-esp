#pragma once
#include <cstdint>

namespace Offsets {
    // client.dll (offsets CS2 build ~2024 sonu; güncelleme gelirse yenile)
    constexpr uintptr_t dwEntityList              = 0x2577BE0;
    constexpr uintptr_t dwLocalPlayerPawn         = 0x2578FC8;
    constexpr uintptr_t dwLocalPlayerController   = 0x23FECD0;
    constexpr uintptr_t dwViewMatrix              = 0x256CFF0;
    constexpr uintptr_t dwGlowManager             = 0x254E8B8;

    // Entity offsets
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