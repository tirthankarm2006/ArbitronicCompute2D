#pragma once
#include "Logging/EditorLog.h"

namespace ARB{
	namespace Editor {
		struct WindowProps {
			GLFWwindow* window;
			unsigned int width, height;
			char* windowName;
		};
		class EditorWindow {
		private:
			std::shared_ptr<Editor::Log> windowLogger;
			std::shared_ptr<WindowProps> mainWindow;
		public:
			void processInput();
			int windowShouldClose();
			void onWindowClosed();
			void startUpdate();
			void endUpdate();
			void ResetWindowSize(unsigned int width, unsigned int height);
			int GetWindowHeight() {
				return mainWindow->height;
			}
			int GetWindowWidth() {
				return mainWindow->width;
			}
			GLFWwindow* GetWindowPtr() {
				return mainWindow->window;
			}
			void closeWindow();
			EditorWindow(unsigned int width, unsigned int height, char* name, int xpos, int ypos);
			EditorWindow(){}
			~EditorWindow();
		};
	}
}
