#pragma once
#include <cstdint>

namespace Offsets {
    // client.dll — dumper 2026-09-10
    constexpr uintptr_t dwEntityList              = 0x2577BE0;
    constexpr uintptr_t dwLocalPlayerPawn         = 0x23CCC08;
    constexpr uintptr_t dwLocalPlayerController   = 0x23A78D0;
    constexpr uintptr_t dwViewMatrix              = 0x23D21F0;
    constexpr uintptr_t dwGlowManager             = 0x23C93F8;

    // Entity offsets
    constexpr uintptr_t m_iHealth                 = 0x34C;   // C_BaseEntity
    constexpr uintptr_t m_iTeamNum                = 0x3E7;   // C_BaseEntity
    constexpr uintptr_t m_lifeState               = 0x354;   // C_BaseEntity
    constexpr uintptr_t m_vOldOrigin              = 0x13B8;  // C_BasePlayerPawn
    constexpr uintptr_t m_pGameSceneNode          = 0x330;   // C_BaseEntity
    constexpr uintptr_t m_modelState              = 0x140;   // CGameSceneNode
    constexpr uintptr_t m_boneArray               = 0x80;    // bone matrix offset
    constexpr uintptr_t m_iIDEntIndex             = 0x342C;  // C_CSPlayerPawn
    constexpr uintptr_t m_iszPlayerName           = 0x6F4;   // controller
    constexpr uintptr_t m_hPlayerPawn             = 0x914;   // controller -> pawn handle
}
