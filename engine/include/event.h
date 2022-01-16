#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <functional>

namespace Engine
{
    template <class... Args>
    class Event
    {
    public:
        std::shared_ptr<void> AddListener(std::function<void(Args...)> func)
        {
            std::shared_ptr<std::function<void(Args...)>> callback(new std::function<void(Args...)>(std::move(func)),
                                                                   [&](std::function<void(Args...)> *obj)
                                                                   { delete obj; Clear(); });
            callbacks_.push_back(callback);

            return callback;
        }

        static void RemoveListener(std::shared_ptr<void> &func)
        {
            func.reset();
        }

        void Clear()
        {
            callbacks_.erase(std::remove_if(callbacks_.begin(), callbacks_.end(),
                                            [](std::weak_ptr<std::function<void(Args...)>> callback)
                                            { return callback.expired(); }),
                             callbacks_.end());
        }

        void Invoke(Args... args)
        {
            for (auto weak : callbacks_)
            {
                if (auto shared = weak.lock())
                {
                    (*shared)(args...);
                }
            }
        }

    private:
        std::vector<std::weak_ptr<std::function<void(Args...)>>> callbacks_;
    };
}