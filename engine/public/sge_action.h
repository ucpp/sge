#ifndef _SGE_ACTION_H_
#define _SGE_ACTION_H_

#include <memory>
#include <vector>
#include <algorithm>
namespace SGE
{
    template <typename... Args>
    class IAction
    {
    public:
        virtual ~IAction() = default;

        virtual void Invoke(Args... args) = 0;
        virtual void* GetObjectPtr() const = 0;
    };

    template <typename T, typename... Args>
    class MemberFunctionAction : public IAction<Args...>
    {
    public:
        MemberFunctionAction(T* object, void (T::*method)(Args...))
            : m_object(object), m_method(method) {}

        void Invoke(Args... args) override
        {
            (m_object->*m_method)(args...);
        }

        void* GetObjectPtr() const override
        {
            return m_object;
        }

    private:
        T* m_object;
        void (T::*m_method)(Args...);
    };

    template <typename... Args>
    class Action
    {
    public:
        template <typename T>
        void Subscribe(T* object, void (T::*method)(Args...))
        {
            m_subscribers.push_back(std::make_unique<MemberFunctionAction<T, Args...>>(object, method));
        }

        void Unsubscribe(void* object)
        {
            m_subscribers.erase(
                std::remove_if(m_subscribers.begin(), m_subscribers.end(),
                            [object](const auto& action)
                            {
                                return action->GetObjectPtr() == object;
                            }),
                m_subscribers.end());
        }

        void Invoke(Args... args)
        {
            for (auto& subscriber : m_subscribers)
            {
                subscriber->Invoke(args...);
            }
        }

        void Clear()
        {
            m_subscribers.clear();
        }

    private:
        std::vector<std::unique_ptr<IAction<Args...>>> m_subscribers;
    };
}

#endif // !_SGE_ACTION_H_