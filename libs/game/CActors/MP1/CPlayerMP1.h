#pragma once

#include "Math.hpp"
#include "CActors/CActor.h"

#include <cstddef>
#include <cstdint>

template<typename Res>
inline Res *GetField(const void *thiz, size_t offset) {
    size_t ptr = ((size_t)thiz + offset);
    return (Res*)ptr;
};

inline size_t GetVtable(const void *thiz) {
    return *GetField<size_t>(thiz, 0);
}

template<typename T>
inline T VtableLookup(const void *thiz, size_t functionOffset) {
    size_t *vtable = *GetField<size_t*>(thiz, 0);
    size_t ptr = vtable[functionOffset + 2];
    //  OSReport("VT %x offset %x ptr %x\n", (int)vtable, functionOffset + 2, ptr);
    return (T)ptr;
}

class CStateManager;
class CPhysicsActorMP1 : public CActorMP1 {
public:
    CVector3f *GetVelocityWR(const CStateManager &mgr) const;
    //_ZN16CPhysicsActorMP113SetVelocityWRER13CStateManagerRK9CVector3f
    //_ZN16CPhysicsActorMP113SetVelocityWRERK13CStateManagerRK9CVector3f
    void SetVelocityWR(CStateManager &mgr, const CVector3f &vel);

    CAxisAngle *GetAngularVelocityWR(const CStateManager &mgr) const;
    void SetAngularVelocityWR(CStateManager &mgr, const CAxisAngle &vel);
};

enum class EPlayerMovementState: uint32_t { OnGround, Jump, ApplyJump, Falling, FallingMorphed };
inline const char* stateToName(EPlayerMovementState state) {
    switch (state) {
    case EPlayerMovementState::OnGround: return "OnGround";
    case EPlayerMovementState::Jump: return "Jump";
    case EPlayerMovementState::ApplyJump: return "ApplyJump";
    case EPlayerMovementState::Falling: return "Falling";
    case EPlayerMovementState::FallingMorphed: return "FallingMorphed";
    default: return "Unknown";
    }
}

class CMorphBallMP1 {
public:
    uint32_t GetBombJumpState() const;
    bool GetIsInHalfPipeMode() const;
    bool GetIsInHalfPipeModeInAir() const;
    bool GetTouchedHalfPipeRecently() const;
};

class CTweakPlayerMP1 {
public:
    inline float GetMaxFreeLookPitchAngle() { return *GetField<float>(this, 0x138); }
};

extern CTweakPlayerMP1* gpTweakPlayer;

class CFinalInput;
class CPlayerMP1 : public CPhysicsActorMP1 {
public:
    enum class EOrbitBrokenType : uint32_t;
    void BreakOrbit(EOrbitBrokenType, CStateManager&);
    inline float* GetFreeLookPitchAngle() { return GetField<float>(this, 0x5cc); };
    inline CMorphBallMP1 *GetMorphBall() { return *GetField<CMorphBallMP1*>(this, 0x9d0); }
    inline EPlayerMovementState GetMoveState() { return *GetField<EPlayerMovementState>(this, 0x3f0); }
    CHealthInfoMP1 &HealthInfo(CStateManager &mgr);
    void ProcessInput(const CFinalInput&, CStateManager&);
    void RenderGun(const CStateManager& mgr, const CVector3f& pos) const;
};