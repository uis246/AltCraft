#pragma once

#include <string>

#include "Vector.hpp"
#include "Platform.hpp"

struct BlockInfo;
struct Plugin;

namespace PluginSystem {
	AC_API void RegisterPlugin(Plugin &plugin);

	AC_INTERNAL void Init() noexcept;
	AC_INTERNAL void Deinit() noexcept;

	void Execute(const std::string &luaCode, bool except = false);

	void CallOnChangeState(std::string newState);

	void CallOnTick(double deltaTime);
}
