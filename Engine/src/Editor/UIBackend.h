#pragma once
#include "Logging/EditorLog.h"

namespace ARB {
	namespace Editor {
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

