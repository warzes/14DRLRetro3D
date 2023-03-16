#include "stdafx.h"
#include "EditorLeftPanel.h"
#include "Engine.h"
#include <numbers>

убрать
template <class>
inline constexpr bool AlwaysFalse = false;

template <class T>
struct Invalid {
	static_assert(AlwaysFalse<T>, "A program that instantiates a primary template of a mathematical constant "
		"variable template is ill-formed. (N4835 [math.constants]/3)");
};

template <class T>
inline constexpr T pi_v = Invalid<T>{};
template <>
inline constexpr double pi_v<double> = 3.141592653589793;
template <>
inline constexpr float pi_v<float> = 3.14f;


//-----------------------------------------------------------------------------
bool EditorLeftPanel::Create()
{
	if( !m_drawHelper.Create() )
	{
		Fatal("Editor Draw Helper Init failed!");
		return false;
	}		
	if( !m_map.Create() )
	{
		Fatal("Editor Map Init failed!");
		return false;
	}


	auto t = pi_v<float>;

	if( !m_cursor.Create() )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Destroy()
{
	m_map.Destroy();
	m_cursor.Destroy();
	m_drawHelper.Destroy();
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Update(float deltaTime)
{
	if (!m_isActive) return;

	m_leftViewport.Update(deltaTime);
	m_cursor.Update(m_leftViewport, m_map);
	m_commands.Update(m_cursor, m_map);
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::Draw(float deltaTime)
{
	(void)deltaTime;

	m_leftViewport.SetOpenGLViewport();
	m_map.Draw(m_leftViewport);
	m_cursor.Draw(m_leftViewport);
}
//-----------------------------------------------------------------------------
void EditorLeftPanel::SetActive(bool active)
{
	m_isActive = active;
}
//-----------------------------------------------------------------------------
bool EditorLeftPanel::IsMouseIn() const
{
	const glm::vec2 realMousePos = app::GetMousePosition();
	return realMousePos.x <= m_leftViewport.GetWidth();
}
//-----------------------------------------------------------------------------