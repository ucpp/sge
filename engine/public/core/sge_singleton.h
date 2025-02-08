#ifndef _SGE_SINGLETON_H_
#define _SGE_SINGLETON_H_

namespace SGE
{
    template<typename T>
    class Singleton 
    {
    public:
        static T& Get() 
        {
            static T instance;
            return instance;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;

    protected:
        Singleton() = default;
        ~Singleton() = default;
    };
}

#endif // !_SGE_SINGLETON_H_