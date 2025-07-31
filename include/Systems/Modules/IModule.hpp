#pragma once
#include <cstdint>
//Модули - это помощники систем, которые разделяют их логику

class SceneContext;

class IModule 
{
public:
    virtual ~IModule() = default;
    virtual uint8_t GetPriority() const = 0;
    virtual void Update(SceneContext* context, float deltaTime) = 0;
};