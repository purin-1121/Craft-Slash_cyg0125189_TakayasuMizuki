#include "pch.h"
#include "FadeFactory.h"
#include "Lib/Easing.h"
#include <Croutine/Croutine.h>

FadeFactory* FadeFactory::m_fadeFactory = nullptr;

void FadeFactory::FadeUpdate(float elapsedTime)
{
	float alpha = Easing::Linear(m_currnet, (m_isIn ? 0.0f : 1.0f), (m_isIn ? 1.0f : -1.0f), m_timer);

	m_fade.SetColor({ 0.0f, 0.0f, 0.0f, alpha });
}

std::function<void()> FadeFactory::SetFade(float timer, bool isIn, float elapsedTime)
{
	m_currnet = 0;
	m_timer = timer;
	m_isIn = isIn;

	return[&, elapsedTime]() {FadeUpdate(elapsedTime); return IEmurator::Null; };
}
