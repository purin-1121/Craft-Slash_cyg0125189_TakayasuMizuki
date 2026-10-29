#include "pch.h"
#include "InputGroup.h"
#include "InterFace/IInputObserver.h"

int InputGroup::m_currentObserverNum = 0;

void InputGroup::AddObserver(IInputObserver* obs)
{
	// IDの設定
	obs->m_observerID = m_currentObserverNum++;
	// 登録
	m_observers.push_back(obs);
}

bool InputGroup::OnInputAccept(Message::InputEvent input)
{
    bool is = false;
    for (int i = static_cast<int>(m_observers.size()) - 1; i >= 0; --i) {
        if (i >= static_cast<int>(m_observers.size())) continue;

        if (m_observers[i] && m_observers[i]->OnInputAccept(input)) {
            is = true;
            break; // 一番手前のUIが反応したら打ち切り
        }
    }
	return is;
}

void InputGroup::SortObserver()
{
    std::sort(m_observers.begin(), m_observers.end(),
        [](IInputObserver* a, IInputObserver* b) {
            // nullptrだったら次へ
            if (!a) return false;
            if (!b) return true;

            // a のレイヤーが b のレイヤーより小さければ前に来る
            return a->GetLayer() < b->GetLayer();
        }
    );
}

InputGroup::InputGroup(InputGroup* parent)
	: IInputObserver(parent)
{
}

