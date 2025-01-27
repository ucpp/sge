#ifndef _SGE_ACTION_H_
#define _SGE_ACTION_H_

#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

namespace SGE
{
    template <typename... Args>
    class Action
    {
    public:
        template <typename T>
        void Subscribe(T* object, void (T::*method)(Args...))
        {
            if (!object || !method)
            {
                return;
            }

            m_subscribers.emplace_back(object, [object, method](Args... args) { (object->*method)(args...); });
        }

        void Unsubscribe(void* object)
        {
            if (!object)
            {
                return;
            }

            m_subscribers.erase(std::remove_if(m_subscribers.begin(), m_subscribers.end(),
            [object](const auto& subscriber) { return subscriber.first == object; }), m_subscribers.end());
        }

        void Invoke(Args... args)
        {
            for (const auto& subscriber : m_subscribers)
            {
                if (subscriber.second) subscriber.second(args...);
            }
        }

        void Clear()
        {
            m_subscribers.clear();
        }

    private:
        std::vector<std::pair<void*, std::function<void(Args...)>>> m_subscribers;
    };
}

#endif // !_SGE_ACTION_H_