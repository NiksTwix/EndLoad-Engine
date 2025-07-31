#pragma once
#include <typeindex>

class Event 
{
public:
	virtual ~Event() = default;
    //Запрещаем копирование
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    // Разрешаем перемещение
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;
};
 