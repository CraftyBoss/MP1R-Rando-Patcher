#pragma once

#include "../types.h"

#include "CGame/CGameStateManager.h"
#include "CObject/CObjectId.h"
#include "CStateManagerGameLogicMP1.h"
#include <CGame/CGameAreaManager.h>
#include <CGame/CGameInstanceState.h>
#include <rstl/enum_bit_field.h>
#include <rstl/rc_ptr.hpp>

class NStateManager {
public:
    enum EStateManagerType {};
};

class NUIRelayGOC {
public:
    enum EUIProxyEvent {};
    enum SUIProxyEventData {};
};

class IStateManagerGameLogic;

class CDataEnumValue;
class CFinalInput;
class CGameObjectComponent;
class CSceneProxy;

class CTransform4f;
class CVector3f;

class CStateManager {
public:

    enum EUpdateFlag {};
    enum EResetElapsedTime {};

    CStateManager(NStateManager::EStateManagerType,CObjectId const&,rstl::ncrc_ptr<CGameStateManager> const&,rstl::ncrc_ptr<IStateManagerGameLogic> const&);
    CStateManager(NStateManager::EStateManagerType,CObjectId const&,rstl::ncrc_ptr<CGameStateManager> const&);
    ~CStateManager();

    void GetDisplayManager(void);
//    void GameLogicOnAreaLoaded(CValueVersionId<uint,ushort,ushort,16u,16u>);
    void EmitUIProxyEvent(NUIRelayGOC::EUIProxyEvent);
    void EmitUIProxyEvent(NUIRelayGOC::SUIProxyEventData const&);
    CGameInstanceState& GameInstanceState(void);
    void InitializeState(void);
    void SetInventoryCountCallback(CDataEnumValue const&,float,float);
    void SetInventoryCapacityCallback(CDataEnumValue const&,float,float);
    CGameState& GameState(void);
    void ShutdownState(void);
    void ProcessInput(CFinalInput const&);
    void ConsumePlayerInput(void);
//    void Update(float,rstl::enum_bit_field<CStateManager::EUpdateFlag,uchar,(CStateManager::EUpdateFlag)1>);
    void UpdateEverything(float);
    void BeginFrame(float);
    void UpdateAutoSave(float);
    void PumpScriptMessagesAndAllow(void);
    void BeginInputFrame(void);
    void UpdateRumbleManagers(float);
    void UpdateDisplayManager(float);
    void UpdateTerrainManager(float,bool);
    void PostCameraMove(float);
    void EndInputFrame(void);
    void LegacyEndInputFrame(void);
    void EndFrame(float);
    void DoPreThinkLogic(float);
    void GetCurrentTimeScale(void);
    void DoThinkLogic(float);
    void UpdateEffects(float);
    void UpdatePhysicsState(float);
    void DoTouchLogic(void);
    void RemoveFromSortedList(CGameObjectComponent &);
    void UpdateRenderState(void);
    void PushRenderState(rstl::ncrc_ptr<CSceneProxy> &,CStateManager::EResetElapsedTime);
    bool IsPaused(void) const;
    CGameInstanceState* GetGameInstanceState(void);
    void IsSkippingCinematic(void);
    void StartAutoSave(void);
    void UpdateListenerData(CTransform4f const&,CVector3f const&);
    void SetCameraPositionForAcousticBoundsFade(CVector3f const&);
    void StartTimeScale(float,float,float,float);
    void BlendOutTimeScale(uint);
    void StopTimeScale(uint);
    void GetTimeScaleState(uint);
//    void ConnectToOnRequestCompleteSignal(uint,TFunctorR<void,CStateManager&,CTimeScaleRequest::ERequestCompletionReason> const&);
    void CreatePhysicsWorld(void);
    CGameState* GetGameState(void) const;
    void GetUIDisplayManager(void);
    void UIDisplayManager(void);

    // world record for largest struct ever holy crap
    char gapA[5112128];
    CGameAreaManager * mGameAreaManager;
    void *unkPtr1;
    CStateManagerGameLogicMP1 *mStateManagerGameLogic;
    void *unkPtr3;
    char gap4E0148[1380];
    void* qword4E06C4;
    void* qword4E06CC;
};