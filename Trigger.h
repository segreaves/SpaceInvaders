#pragma once
#include <unordered_map>
#include <functional>
#include <string>

template<typename T = void>
class Trigger
{
public:
    void addCallback(std::string id, const std::function<void(T)>& callback)
    {
        m_callbacks.emplace(id, callback);
    }

    void removeCallback(std::string id)
	{
        auto it = m_callbacks.find(id);
        if (it == m_callbacks.end()) return;
        m_callbacks.erase(it);
	}

    void dispatch(const T& eventData) const
    {
        std::unordered_map<std::string, std::function<void(T)>> callbacks = m_callbacks;
        for (const auto& callback : callbacks) {
            callback.second(eventData);
        }
    }

private:
    std::unordered_map<std::string, std::function<void(T)>> m_callbacks;
};

template<>
class Trigger<void>
{
public:
    void addCallback(std::string id, const std::function<void()>& callback)
	{
		m_callbacks.emplace(id, callback);
	}

    void removeCallback(std::string id)
    {
        auto it = m_callbacks.find(id);
		if (it == m_callbacks.end()) return;
		m_callbacks.erase(it);
    }

    void dispatch() const
    {
        std::unordered_map<std::string, std::function<void()>> callbacks = m_callbacks;
        for (const auto& callback : callbacks) {
            callback.second();
        }
    }

private:
    std::unordered_map<std::string, std::function<void()>> m_callbacks;
};
