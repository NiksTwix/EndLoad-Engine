#pragma once
#include <Core/IServices.hpp>
#include <Services/Events/Event.hpp>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <atomic>

class EventManager : public IHelperService {
//public:
//    using HandlerID = size_t;
//    using EventHandler = std::function<void(const Event&)>;
//
//    static constexpr HandlerID InvalidHandlerID = std::numeric_limits<HandlerID>::max();
//
//    EventManager() = default;
//    ~EventManager() = default;
//
//    // ��������� �����������
//    EventManager(const EventManager&) = delete;
//    EventManager& operator=(const EventManager&) = delete;
//
//    // ��������� �����������
//    EventManager(EventManager&&) = default;
//    EventManager& operator=(EventManager&&) = default;
//
//    // �������� �� ������� � ��������� ����������� ID
//    template<typename T>
//    HandlerID Subscribe(EventHandler handler) {
//        static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");
//
//        std::lock_guard<std::mutex> lock(m_mutex);
//        HandlerID id = m_nextHandlerID++;
//        m_handlers[typeid(T)].emplace_back(id, std::move(handler));
//        return id;
//    }
//
//    // �������� �������
//    template<typename T>
//    void Emit(const T& event) {
//        static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");
//
//        std::vector<std::pair<HandlerID, EventHandler>> handlersCopy;
//        {
//            std::lock_guard<std::mutex> lock(m_mutex);
//            auto it = m_handlers.find(typeid(T));
//            if (it != m_handlers.end()) {
//                handlersCopy = it->second;
//            }
//        }
//
//        for (const auto& [id, handler] : handlersCopy) {
//            handler(event);
//        }
//    }
//
//    template<typename T>
//    bool EmitToHandler(HandlerID id, const T& event) {
//        static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");
//
//        std::lock_guard<std::mutex> lock(m_mutex);
//        auto it = m_handlers.find(typeid(T));
//        if (it != m_handlers.end()) {
//            for (const auto& [handlerID, handler] : it->second) {
//                if (handlerID == id) {
//                    handler(event); // ����� ������ ������� �����������
//                    return true;
//                }
//            }
//        }
//        
//        return false; // ���������� �� ������
//    }
//
//    // ������� �� ID
//    template<typename T>
//    bool Unsubscribe(HandlerID id) {
//        static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");
//
//        std::lock_guard<std::mutex> lock(m_mutex);
//        auto it = m_handlers.find(typeid(T));
//        if (it != m_handlers.end()) {
//            auto& handlers = it->second;
//            handlers.erase(
//                std::remove_if(handlers.begin(), handlers.end(),
//                    [id](const auto& pair) { return pair.first == id; }),
//                handlers.end()
//            );
//            return true;
//        }
//        return false;
//    }
//
//private:
//    std::mutex m_mutex;
//    std::atomic<HandlerID> m_nextHandlerID{ 0 };
//    std::unordered_map<std::type_index, std::vector<std::pair<HandlerID, EventHandler>>> m_handlers;
};