#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <Core/IService.hpp>
#include <typeindex>
#include <Systems\Modules\IModule.hpp>

enum class SystemPriority {
    WINDOW_MANAGER = 0,  // ELWindowManager (������ ����������� ������)
    PHYSICS = 1,  // PhysicsSystem
    RENDER = 2,  // RenderSystem
    INPUT = 3,  // InputSystem
    USER_SYSTEMS = 100 // ���������������� ������� (�� ���������)
};

class ISystem : public IService         //������� ������
{
    // ����� ��� �������
protected:
    std::vector<IModule*> _modules;  // ������ ������ � �������

public:
    virtual void Update() {}

    virtual SystemPriority GetPriority() { return SystemPriority::USER_SYSTEMS; } //��� ������ ������ 0-20, ��������� - ����������������

    void AddModule(IModule* module) {
        _modules.push_back(module);
        std::sort(_modules.begin(), _modules.end(),
            [](const IModule* a, const IModule* b) {
                return a->GetPriority() < b->GetPriority();
            });
    }

    // �������� ������
    void RemoveModule(IModule* module) {
        auto it = std::remove(_modules.begin(), _modules.end(), module);
        _modules.erase(it, _modules.end());
        delete module;
    }
};