//--------------------------------------------------------------------------------------
// File: IInputObserver.h
//
// 入力検知インターフェース
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Message/Message.h"
#include <unordered_map>
#include "InterFace/IInputObserver.h"

class InputGroup : public IInputObserver
{

public:

	// メッセージ受け取り処理
	virtual bool OnInputAccept(Message::InputEvent input) override;
	// 観察者追加
	void AddObserver(IInputObserver* observer);
	// 観察者削除
	void DeleteObserver(IInputObserver* obs) { 
		if(!m_observers.empty())
			std::erase(m_observers, obs); 
	};
	// 観察者全削除
	void DeleteObservers() { m_observers.clear(); }
	// 観察者をレイヤー順に並び替え
	void SortObserver();

	InputGroup(InputGroup* parent);

protected:

	// 観察者たち
	std::vector<IInputObserver*> m_observers;
	// 観察者番号
	static int m_currentObserverNum;

};

