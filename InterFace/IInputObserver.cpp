//--------------------------------------------------------------------------------------
// File: IInputObserver.h
//
// 入力検知インターフェース
//	
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "Message/InputGroup.h"
#include "IInputObserver.h"

void IInputObserver::SetLayer(int layer)
{
	// レイヤーを設定
	m_layer = layer;
	// 親にソート命令
	if (m_parent) 
		m_parent->SortObserver();
}

IInputObserver::IInputObserver(InputGroup* group)
	: m_parent{group}
	, m_layer{0}
{
	// 親があったら追加させる
	if (group)
	{
		group->AddObserver(this);
	}
}

IInputObserver::~IInputObserver()
{
	if (m_parent)
		m_parent->DeleteObserver(this);
}
