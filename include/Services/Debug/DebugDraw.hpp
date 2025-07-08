#pragma once
#include <Core\IServices.hpp>
#include <Math\Vector3.hpp>
#include <Math\Vector4.hpp>
#include <vector>

class DebugDraw : public IHelperService
{
public:
    // Цветная линия с началом и концом
    struct DebugLine {
        Math::Vector3 start;
        Math::Vector3 end;
        Math::Vector4 color; // RGBA
        float duration;      // Время жизни в секундах (0 = только на 1 кадр)
    };

    // Ось координат (X,Y,Z)
    struct DebugAxis {
        Math::Vector3 origin;
        Math::Vector3 XDir;
        Math::Vector3 YDir;
        Math::Vector3 ZDir;
        float duration;
    };

    // Сетка
    struct DebugGrid {
        float cellSize; 
        int cellCount;// Количество линий в одну сторону
        Math::Vector4 color;
    };

    void AddLine(Math::Vector3 start,Math::Vector3 end,Math::Vector4 color,float duration);
    void AddAxis(Math::Vector3 origin,Math::Vector3 XDir,Math::Vector3 YDir,Math::Vector3 ZDir,float length,float duration);
    void AddGrid(float cellSize, int cellCount, const Math::Vector4& color);

    virtual void Render() {};
    virtual void Clear();
protected:
    std::vector<DebugLine> lines;
    std::vector<DebugAxis> axes;
    std::vector<DebugGrid> grids;
};