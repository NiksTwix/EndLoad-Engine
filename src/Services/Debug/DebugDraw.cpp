#include "Services\Debug\DebugDraw.hpp"

void DebugDraw::AddLine(Math::Vector3 start, Math::Vector3 end, Math::Vector4 color, float duration)
{
	this->lines.push_back(DebugLine(start, end, color, duration));
}

void DebugDraw::AddAxis(Math::Vector3 origin, Math::Vector3 XDir, Math::Vector3 YDir, Math::Vector3 ZDir, float length, float duration)
{
	this->axes.push_back(DebugAxis(origin, XDir*length, YDir * length, ZDir * length, duration));
}

void DebugDraw::AddGrid(float cellSize, int cellCount, const Math::Vector4& color)
{
	this->grids.push_back(DebugGrid(cellSize, cellCount, color));
}

void DebugDraw::Clear()
{
	this->axes.clear();
	this->grids.clear();
	this->lines.clear();
}
