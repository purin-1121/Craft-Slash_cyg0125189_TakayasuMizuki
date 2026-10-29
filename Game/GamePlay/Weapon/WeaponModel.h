#pragma once
#include "Game/GamePlay/GameObject/GameObject.h"

class WeaponModel : public GameObject
{
public:
    WeaponModel(std::string modelName)
        : GameObject(SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero)
        , m_modelName{ modelName }
    {
    }

    void Initialize(const GameContext& gameContext) override {}
    void Update(float, const GameContext&, Camera&) override {}
    void Render(const GameContext& gameContext, SimpleMath::Matrix view) override
    {
        if (m_modelName == "none")
            return;
        auto world = SimpleMath::Matrix::CreateFromQuaternion(GetQuaternion())
            * SimpleMath::Matrix::CreateTranslation(GetPosition());
        Draw::GetInstance().DrawModel(gameContext, world, view, ObjectLib::m_models[m_modelName].get());
    }
    void RenderText() override {}
    void Finalize() override {}
    bool IsActive() override { return true; }

private:
    std::string m_modelName;
};
