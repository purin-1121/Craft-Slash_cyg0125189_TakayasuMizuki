#include "pch.h"
#include "Canvas.h"
#include "Common/UI/Button/Button.h"
#include "Image.h"
#include "Button/FunctionButton.h"
#include "Button/InputField.h"
#include "Bar/Slider.cpp"

Canvas::Canvas(bool active, InputGroup* group)
	: InputGroup(group)
	, m_isActive{ active }
	, m_UI{}
	, m_threadFunc{}
	, m_func{}
{
}

void Canvas::Initialize(const GameContext& gameContext)
{
	for (auto& UI : m_UI)
	{
		UI.object->Initialize(gameContext);
	}
}

bool Canvas::Update(const GameContext& gameContext,const float& elapsedTime)
{
	// 非アクティブなら実行しない
	if (!m_isActive)
		return false;
	for (auto& UI : m_UI)
	{
		// 非アクティブだったら次へ
		if (!UI.object->IsActive())
			continue;
		UI.object->Update(gameContext, elapsedTime);
		UI.object->SetOffset(m_position);
	}
	// 内蔵された関数の実行
	for (auto& f : m_func)
	{
		f.second();
	}

	return false;
}

void Canvas::Render(const GameContext& gameContext, const Camera& camera)
{
	// 非アクティブなら実行しない
	if (!m_isActive)
		return;
	for (auto& UI : m_UI)
	{
		// 非アクティブだったら次へ
		if (!UI.object->IsActive())
			continue;
		UI.object->Render(gameContext, camera);
	}
}

void Canvas::RenderText()
{
	// 非アクティブなら実行しない
	if (!m_isActive)
		return;
	for (auto& UI : m_UI)
	{
		// 非アクティブだったら次へ
		if (!UI.object->IsActive())
			continue;
		UI.object->RenderText();
	}
}

void Canvas::Finalize()
{
	for (auto& UI : m_UI)
	{
		UI.object->Finalize();
		delete UI.object;
	}

	m_UI.clear();
}

bool Canvas::IsOnUIs()
{
	// 全UIを検知
	for (auto& ui : m_UI)
	{
		// 非アクティブだったら次へ
		if (!ui.object->IsActive())
			continue;
		// 何か一つでも反応したらtrue
		if (ui.object->MouseOn())
			return true;
	}
	return false;
}

UIObject* Canvas::SammonUI(const std::string& name)
{
	auto it = std::find_if(m_UI.begin(), m_UI.end(), [&](UI a) { return a.name == name; });
	if (it != m_UI.end())
	{
		return it->object;
	}
	return nullptr;
}

template<typename T>
T* Canvas::SammonUI(const std::string& name)
{
	auto it = std::find_if(m_UI.begin(), m_UI.end(), [&](UI a) { return a.name == name; });
	if (it != m_UI.end())
	{
		return dynamic_cast<T*>(it->object);
	}
	return nullptr;
}


template<typename T>
void Canvas::SortLayer(std::function<bool(T, T)> f)
{
	//std::sort(m_UI.begin(), m_UI.end(), f);
}

template void Canvas::SortLayer<int>(std::function<bool(int, int)>);
template void Canvas::SortLayer<double>(std::function<bool(double, double)>);
template void Canvas::SortLayer<float>(std::function<bool(float, float)>);


template Image* Canvas::SammonUI<Image>(const std::string& name);
template Text* Canvas::SammonUI<Text>(const std::string& name);
template Button* Canvas::SammonUI<Button>(const std::string& name);
template FunctionButton* Canvas::SammonUI<FunctionButton>(const std::string& name);
template InputField* Canvas::SammonUI<InputField>(const std::string& name);
template Bar<int>* Canvas::SammonUI<Bar<int>>(const std::string& name);
template Bar<float>* Canvas::SammonUI<Bar<float>>(const std::string& name);
template Bar<double>* Canvas::SammonUI<Bar<double>>(const std::string& name);
template Slider<int>* Canvas::SammonUI<Slider<int>>(const std::string& name);
template Slider<float>* Canvas::SammonUI<Slider<float>>(const std::string& name);
template Slider<double>* Canvas::SammonUI<Slider<double>>(const std::string& name);