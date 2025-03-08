#pragma once
#include "Pointers.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Protections.hpp"
#include "Natives.hpp"
#include "D3DRenderer.hpp"

namespace base {
	class guiScript {
	public:
		void onPresentTick();
		static void tick();
	};
	inline guiScript g_guiScript;
}