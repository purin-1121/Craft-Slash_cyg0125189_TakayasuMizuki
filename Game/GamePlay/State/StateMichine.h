#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include "InterFace/IGameObjectState.h"
#include "Message/InputGroup.h"
#include "Game/GamePlay/Actor/ActorData.h"
#include "Game/GamePlay/State/Actor/StunState.h"

class StateMachine : public InputGroup
{

public:
    StateMachine() 
        : InputGroup(nullptr)
        , m_stateTimer{} 
        , m_id{StateID::Null}
    {};

    // ステートの登録
    template <typename TState, typename... Args>
    void RegisterState(Args&&... args) {
        m_states[typeid(TState)] = std::make_unique<TState>(std::forward<Args>(args)...);
    }
    template<typename pTState>
    void RegisterState(std::unique_ptr<pTState> pState, StateID id)
    {
        m_stateIDMap[id] = pState.get();
        m_states[typeid(*pState)] = std::move(pState);
    }

    // 遷移リクエスト（即座に変えず、Updateの安全なタイミングで変える）
    template <typename TState>
    void RequestChangeState() {
        auto it = m_states.find(typeid(TState));
        if (it != m_states.end()) {
            m_nextState = it->second.get();
        }
    }

    // 遷移リクエスト（ステート指定版）
    void RequestChangeState(IGameObjectState* state)
    {
        if (state)
            m_nextState = state;
    }

    // 遷移リクエスト（列挙型指定版）
    void RequestChangeState(StateID id)
    {
        if (id == StateID::Stun)
        {
            int a = 0;
            a++;
        }
        m_nextState = m_stateIDMap[id];
     }

    // ステートの取得
    const std::unordered_map<std::type_index, std::unique_ptr<IGameObjectState>>& GetStates() { return m_states; }
    const std::unordered_map<StateID, IGameObjectState*>& GetStatesIDMaps() { return m_stateIDMap; }

    // 更新処理
    void Update(const GameContext& gameContext, float elapsedTime) {
        // 遷移リクエストがあればステートを切り替える
        if (m_nextState != nullptr) {
            // 今のステートが切り替わり条件を満たしてない場合はなし
            if (m_currentState && !m_currentState->CanChanged())
            {
                m_nextState = nullptr;
            }
            // 同じステートだった場合は変更なし
            else if (m_currentState == m_nextState)
            {
                m_nextState = nullptr;
            }
            else
            {
                
                if (m_currentState) {
                    m_currentState->Exit();
                }
                m_oldState = m_currentState;
                m_currentState = m_nextState;
                m_nextState = nullptr;
                m_currentState->Enter();
            }
        }

        // 現在のステートを更新
        if (m_currentState) {
            m_currentState->Update(gameContext, elapsedTime);
        }
    }

    // 描画処理
    void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) {
        if (m_currentState) {
            m_currentState->Render(gameContext, view);
        }
    }

    // 現在のステート判定
    template <typename TState>
    bool IsState() const {
        if (!m_currentState) return false;
        return typeid(*m_currentState) == typeid(TState);
    }
    bool IsState(StateID id) {
        if (!m_currentState) return false;
        return typeid(*m_currentState) == typeid(m_stateIDMap[id]);
    }

    // ステートの取得
    template <typename TState>
    IGameObjectState* GetState()
    {
        return m_states[typeid(TState)].get();
    }

    // ステートIDの取得
    template<typename TState>
    const StateID& GetID(TState* state)
    {
        StateID id = StateID::Idle;
        for (auto& data : m_stateIDMap)
        {
            if (data.second == state)
            {
                return data.first;
            }
        }

        return StateID::Idle;
    }

    IGameObjectState* GetCurrentState() { return m_currentState; }

    // ---- ステート状態管理用関数 ---- //
    void ResetTimer() { m_stateTimer = 0; }
    void SetTimer(float time) { m_stateTimer = time; }
    void AddTimer(float add) { m_stateTimer += add; }
    float GetTimer() const { return m_stateTimer; }


    // 入力処理
    bool OnInputAccept(Message::InputEvent input) override
    {
        auto observer = dynamic_cast<IInputObserver*>(m_currentState);
        if(observer)
            return observer->OnInputAccept(input);

        return false;
    }

private:
    // 今のステート
    IGameObjectState* m_currentState = nullptr;
    // 次のステート
    IGameObjectState* m_nextState = nullptr;
    // ひとつ前のステート
    IGameObjectState* m_oldState = nullptr;
    // 型（Type）をキーにしてステートを保持するマップ
    std::unordered_map<std::type_index, std::unique_ptr<IGameObjectState>> m_states;
    // 列挙型から参照できるようにしたマップ
    std::unordered_map<StateID, IGameObjectState*> m_stateIDMap;

    // 今のステート内に入る秒数
    float m_stateTimer;
    // 今のステートID
    StateID m_id;

};