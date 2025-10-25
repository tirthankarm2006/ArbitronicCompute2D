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

