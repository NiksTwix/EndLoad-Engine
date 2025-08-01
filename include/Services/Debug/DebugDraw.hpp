#pragma once
#include <Core\IServices.hpp>
#include <Math\Vector3.hpp>
#include <Math\Vector4.hpp>
#include <vector>

class DebugDraw : public IHelperService
{
public:
    // ������� ����� � ������� � ������
    struct DebugLine {
        Math::Vector3 start;
        Math::Vector3 end;
        Math::Vector4 color; // RGBA
        float duration;      // ����� ����� � �������� (0 = ������ �� 1 ����)
    };

    // ��� ��������� (X,Y,Z)
    struct DebugAxis {
        Math::Vector3 origin;
        Math::Vector3 XDir;
        Math::Vector3 YDir;
        Math::Vector3 ZDir;
        float duration;
    };

    // �����
    struct DebugGrid {
        float cellSize; 
        int cellCount;// ���������� ����� � ���� �������
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