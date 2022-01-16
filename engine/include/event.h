#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <functional>

namespace sge
{
    template <class... Args>
    class Event
    {
    public:
        std::shared_ptr<void> addListener(std::function<void(Args...)> func)
        {
            std::shared_ptr<std::function<void(Args...)>> callback(new std::function<void(Args...)>(std::move(func)),
                                                                   [&](std::function<void(Args...)> *obj)
                                                                   { delete obj; clear(); });
            callbacks.push_back(callback);

            return callback;
        }

        static void removeListener(std::shared_ptr<void> &func)
        {
            func.reset();
        }

        void clear()
        {
            callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
                                            [](std::weak_ptr<std::function<void(Args...)>> callback)
                                            { return callback.expired(); }),
                             callbacks.end());
        }

        void invoke(Args... args)
        {
            for (auto weak : callbacks)
            {
                if (auto shared = weak.lock())
                {
                    (*shared)(args...);
                }
            }
        }

    private:
        std::vector<std::weak_ptr<std::function<void(Args...)>>> callbacks;
    };
}