//--------------------------------------------------------------------------------------
// File: State .h
//
// ÉvÉåÉCÉÑÅ[ÇÃçUåÇèÛë‘
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "InterFace/IGameObjectState.h"

template<typename TOwner>
class State : public IGameObjectState
{

public:
    State(TOwner* owner, const std::string& stateName) : m_owner(owner), m_stateName(stateName) {}
    virtual ~State() override = default;

	virtual void Enter() override = 0;
	virtual void Update(const GameContext& gameContext, float elapsedTime) override = 0;
	virtual void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override = 0;
	virtual void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) = 0;
	virtual void Exit() override = 0;
	const std::string& GetStateName() const override { return m_stateName; };

protected:

	std::string m_stateName;
    TOwner* m_owner; 
};

