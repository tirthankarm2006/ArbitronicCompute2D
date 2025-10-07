#pragma once
#include "EditorWindow.h"
#include "Logging/EditorLog.h"

namespace ARB {
	namespace Editor {
		class InspectorWindowUI
		{
		public:
			InspectorWindowUI(const char* name, glm::vec2 pos, glm::vec2 size);
			void createFrame();
			void ui();
			void endFrame();
		private:
			std::shared_ptr<Editor::Log> inspectorLogger;
			std::string name;
			bool setWindowParams = false;
			glm::vec2 position;
			glm::vec2 size;
		};
	}
}

