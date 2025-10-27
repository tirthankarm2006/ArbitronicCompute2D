#pragma once
#include "Logging/EditorLog.h"

namespace ARB {
	namespace Editor {
//Colors for the letters
#define UI_RED ImVec4{1.0f, 0.0f, 0.0f, 1.0f}
#define UI_GREEN ImVec4{0.0f, 1.0f, 0.0f, 1.0f}
#define UI_WHITE ImVec4{1.0f, 1.0f, 1.0f, 1.0f}
#define UI_YELLOW ImVec4{1.0f, 1.0f, 0.0f, 1.0f}

#define UI_BUTTON_SIZE ImVec2(150, 20)

//Defining UI Themes for the UI windows
#define IM_WIN_BG_COLOR ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f }
#define IM_WIN_HEADER_COLOR ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f }
#define IM_WIN_HEADER_HOVERED_COLOR ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f }
#define IM_WIN_HEADER_ACTIVE_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_BUTTON_COLOR ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f }
#define IM_BUTTON_HOVERED_COLOR ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f }
#define IM_BUTTON_ACTIVE_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_FRAMEBG_COLOR ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f }
#define IM_FRAMEBG_HOVERED_COLOR ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f }
#define IM_FRAMEBG_ACTIVE_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_TAB_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_TAB_ACTIVE_COLOR ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f }
#define IM_TAB_HOVERED_COLOR ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f }
#define IM_TAB_UNFOCUSED_ACTIVE_COLOR ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f }
#define IM_TAB_UNFOCUSED_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_TITLEBG_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }
#define IM_TITLEBG_ACTIVE_COLOR ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f }
#define IM_TITLEBG_HOVERED_COLOR ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }

		class UIBackend
		{
		public:
			static bool InitiateImguiBackend(GLFWwindow* window);
			static void ShutdownImguiBackend();
			static void CreateNewImguiFrame();
			static void RenderAllWindows();
			static void setUITheme();
			static bool isInit;
		};
	}
}

