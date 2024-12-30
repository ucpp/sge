#ifndef _SGE_DEBUG_H_
#define _SGE_DEBUG_H_

#include "pch.h"

#ifdef _DEBUG
    #define DEBUG_ONLY(x) x
#else
    #define DEBUG_ONLY(x)
#endif

namespace SGE
{
    template<typename T>
    inline void Verify(const T& value, const std::string& message = "")
    {
        DEBUG_ONLY(
            if (!value)
            {
                std::ostringstream errorMessage;
                errorMessage << "Value is invalid";
                
                if (!message.empty())
                {
                    errorMessage << ": " << message;
                }

                throw std::runtime_error(errorMessage.str());
            }
        );
    }

    inline void Verify(const HRESULT& hr, const std::string& message = "")
    {
        DEBUG_ONLY(
            if (FAILED(hr))
            {
                std::ostringstream errorMessage;
                errorMessage << "HRESULT validation failed: " << std::to_string(hr);
                
                if (!message.empty())
                {
                    errorMessage << ": " << message;
                }

                throw std::runtime_error(errorMessage.str());
            }
        );
    }
}

#endif // !_SGE_DEBUG_H_