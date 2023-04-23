#pragma once
#include <math/vec2.hpp>

// external
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <fmod/fmod_studio.hpp>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui.h>

// windows
#include <SDKDDKVer.h>
#include <Windows.h>
#include <wrl/client.h>
#include <dxgi.h>
#include <d3d11.h>

// std
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
namespace rn = std::ranges;
namespace vw = std::views;