#pragma once
#include "InterFace/IWeapon.h"
#include "Game/GamePlay/Data/WeaponData.h"
#include "Bullet/BulletManager.h"
#include "WeaponModel.h"
// 発射武器の管理クラス
class RangedWeapon : public IWeapon
{
public:
    RangedWeapon(WeaponData* data);

    void Initialize(const GameContext& gameContext) override { m_bulletManager.Initialize(gameContext); }
    void Update(float elapsedTime, const GameContext&, Camera&) override;
    
    void Render(const GameContext&, SimpleMath::Matrix) override;
    void Finalize() override {};

    void AttackOn(ColliderTag tag) override;
    void AttackOff() override {};
    bool IsAttackOn() const override { return false; } // 弾自体が判定を持つので武器側は常にfalseでOK
    std::string_view GetName() const override { return m_data->name; }
    int GetDamage() const override { return static_cast<int>(m_data->power); }
    const WeaponData* GetData() const override { return m_data; }

    // 見た目用のGameObjectを外に渡せるようにする
    GameObject* GetModelObject() { return &m_model; }
    void SetOwner(GameObject* owner) override { m_owner = owner; };

    // 装着位置(手のボーン行列など)を武器に伝える共通の窓口
    void SetAttachTransform(const SimpleMath::Matrix& m) override
    {
        m_model.SetWorldMatrix(m);
    }

    float GetCoolTime() { return m_coolTimer; };
    SimpleMath::Matrix GetHoldMatrix() override {
        return
            SimpleMath::Matrix::CreateTranslation(m_data->offsets.at(m_stateName).pos) *
            SimpleMath::Matrix::CreateFromQuaternion(m_data->offsets.at(m_stateName).rot);
    };

    // オフセットの種類変更
    void SetOffsetOfState(std::string state) {
        // 見つかったら適応
        if (m_data->offsets.find(state) != m_data->offsets.end())
            m_stateName = state;
    }

private:

    const WeaponData* m_data;
    GameObject* m_owner;
    SimpleMath::Vector3 m_muzzleOffset;
    WeaponModel m_model; // 見た目専用
    BulletManager m_bulletManager;
    BulletData* m_currentData;
    float m_coolTimer = 0.0f;
    std::string m_stateName;
};

