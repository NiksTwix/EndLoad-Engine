#include <Systems\Physics\Physics.hpp>
#include <Core\ServiceLocator.hpp>
#include <Core\Logger.hpp>

Physics::Physics()
{
	Init();
}

Physics::~Physics()
{
	Shutdown();
}

void Physics::Init()
{
	if (m_isValid) return;
	m_isValid = true;
}

void Physics::Shutdown()
{
	if (!m_isValid) return;
	ServiceLocator::Get<Logger>()->Log("PhysicsSystem shutdown completed.");
	m_isValid = false;
}

void Physics::Update()
{
	if (!m_isValid) return;
}

void Physics::BasicUpdate()
{
	/*
		Обновление базовых данный
		Пересчёт вращений и тд
		Обновление позиции и видимости
		Расчёт матриц моделей и нормалей
	*/
}
