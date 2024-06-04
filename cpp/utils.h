#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>
#include <unknwn.h>

#define RPC_PAUSE_MS 10

#define THROW_HR_EX(hr, info)\
{\
    std::stringstream ss;\
    ss << "bad HRESULT 0x" << std::hex << hr << std::dec << ", file " << __FILE__ << ", line " << __LINE__;\
    if (info)\
    {\
        ss << ", " << utf8(info);\
    }\
    throw std::runtime_error(ss.str());\
}

#define THROW_IF_FAILED_EX(x, info)\
while (true)\
{\
    HRESULT hr = x;\
    if (SUCCEEDED(hr)) break;\
    if\
    (\
        hr == RPC_E_CALL_REJECTED ||\
        hr == RPC_E_RETRY ||\
        hr == RPC_E_SERVERCALL_RETRYLATER ||\
        hr == RPC_S_SERVER_TOO_BUSY\
    )\
    {\
        Sleep(RPC_PAUSE_MS);\
        continue;\
    }\
    THROW_HR_EX(hr, info);\
}

#define THROW_HR(hr) THROW_HR_EX(hr, 0)
#define THROW_IF_FAILED(x) THROW_IF_FAILED_EX(x, 0)

std::string utf8(const std::wstring& s);

#endif // UTILS_H
