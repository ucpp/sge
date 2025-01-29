#ifndef _SGE_PCH_H_
#define _SGE_PCH_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <dxgi.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>

#include <wrl.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <exception>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <array>

using namespace DirectX;
using namespace SimpleMath;
using namespace Microsoft::WRL;

#include "sge_types.h"
#include "sge_constants.h"

#include "json.hpp"

#define ORDERED_JSON 1
#if ORDERED_JSON
using njson = nlohmann::ordered_json;
#else
using njson = nlohmann::json;
#endif

#include "sge_math.h"

#endif //!_SGE_PCH_H