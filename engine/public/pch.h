#ifndef _SGE_PCH_H_
#define _SGE_PCH_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "core/d3dx12.h"
#include <d3dcommon.h>
#include <d3dcompiler.h>

#include <wrl.h>
#include <wrl/client.h>

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

using namespace Microsoft::WRL;

#include "core/sge_types.h"
#include "core/sge_constants.h"

#include "json.hpp"

#define ORDERED_JSON
#ifdef ORDERED_JSON
using njson = nlohmann::ordered_json;
#else
using njson = nlohmann::json;
#endif

#include "core/sge_math.h"

#endif //!_SGE_PCH_H