#ifndef _SGE_NON_COPYABLE_H_
#define _SGE_NON_COPYABLE_H_

namespace SGE
{
    class NonCopyable
    {
    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;

    private:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        NonCopyable(NonCopyable&&) = delete;
        NonCopyable& operator=(NonCopyable&&) = delete;
    };
}

#endif // !_SGE_NON_COPYABLE_H_