#pragma once
#include <typeindex>

class Event 
{
public:
	virtual ~Event() = default;
	virtual std::type_index GetType() const = 0;

    //Запрещаем копирование
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    // Разрешаем перемещение
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;
};


class PhysicsProccessEvent : public Event {};
class ProcessEvent : public Event {};