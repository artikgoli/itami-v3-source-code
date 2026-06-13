#pragma once
// global lib includes
#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <wtypes.h>
#include <cstdint>
#include "process.h"
#include <thread>
#include <TlHelp32.h>
#include <string>
#include <unordered_map>
#include <map>
#include <chrono>
#include <vector>
#include <limits>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <regex>
#include <shlobj.h>  
#include <iosfwd>
#include <filesystem>
#include <array>
#include <memory>
#include <xmemory>
#include <fstream>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <wininet.h>
#include <functional> 
#include <set>
#include <urlmon.h>
#include <mmsystem.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "winmm.lib")
#include <bitset>

// global file includes
#include "../utilities/cmd/cmd.h"
#include "../security.h"
#include "color.h"
#include "../core/entry/entry.h"
#include "../utilities/memory/memory.h"
#include "../../valve/math/utl_containers.h" // <-- msvc..
#include "../../valve/sdk/sdk.h"
#include "../../valve/interfaces/ISchemaSystem.h"
#include "../../valve/schema/schema.h"
#include "../../valve/convar/convar.h"
#include "../../valve/offsets.h"
#include "../../app/core/config/config.h"

// sdk
#include "../../valve/math/vector2d.h"
#include "../../valve/math/vector3d.h"
#include "../../valve/math/vector4d.h"
#include "../../valve/math/matrix4x4.h"
#include "../../valve/math/matrix3x4.h"
#include "../../valve/classes/players/c_base_weapon.h"
#include "../../valve/classes/players/c_base_entity.h"
#include "../../valve/classes/players/players.h"
#include "../../valve/classes/globals/global_vars.h"
#include "../../valve/classes/hitboxes/hitbox_studio.h"
#include "../../valve/math/math.h"

#include "../../valve/parser/parser.h"

