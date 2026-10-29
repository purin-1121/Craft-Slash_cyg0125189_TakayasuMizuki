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

class InputGroup;

class IInputObserver
{

public:

	// メッセージ受け取り処理
	virtual bool OnInputAccept(Message::InputEvent input) = 0;
	// オブザーバーID
	int m_observerID = 0;
	// 親
	InputGroup* m_parent;
	// 親設定
	void SetInputParent(InputGroup* p) { m_parent = p; };
	void SetLayer(int layer);
	int GetLayer() { return m_layer; }

protected:

	IInputObserver(InputGroup* group);
	virtual ~IInputObserver();	

	// 自身の優先度
	int m_layer;
};

