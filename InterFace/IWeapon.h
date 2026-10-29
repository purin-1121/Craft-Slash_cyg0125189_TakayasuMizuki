#pragma once

#include "Tools/GameContext.h"
#include "Lib/Camera.h"
#include "InterFace/IDamageDealer.h"
#include "Game/GamePlay/Data/WeaponData.h"

class IWeapon : public IDamageDealer
{
public:
    virtual ~IWeapon() = default;

    virtual void Initialize(const GameContext& gameContext) = 0;
    virtual void Update(float elapsedTime, const GameContext& gameContext, Camera& camera) = 0;
    virtual void Render(const GameContext& gameContext, SimpleMath::Matrix view) = 0;
    virtual void Finalize() = 0;

    virtual void AttackOn(ColliderTag tag) = 0;          // 攻撃開始命令
    virtual void AttackOff() = 0;
    virtual bool IsAttackOn() const = 0;  // 攻撃中か

    virtual std::string_view GetName() const = 0;
    virtual int GetDamage() const override = 0;
	virtual const WeaponData* GetData() const = 0;

    // 装着位置(手のボーン行列など)を武器に伝える共通の窓口
    virtual void SetAttachTransform(const SimpleMath::Matrix& m) = 0;
    virtual void SetOwner(GameObject* owner) = 0;
    virtual float GetCoolTime() = 0;
    virtual SimpleMath::Matrix GetHoldMatrix() = 0;
    virtual void SetOffsetOfState(std::string state) = 0;
};