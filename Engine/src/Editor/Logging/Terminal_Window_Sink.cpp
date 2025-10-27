#include "ARBpch.h"
#include "Terminal_Window_Sink.h"

namespace ARB {
	namespace Editor {
		std::shared_ptr<Editor::Terminal_Window_Sink>& Terminal_Window_Sink::Get_Singleton() {
			static std::shared_ptr<Editor::Terminal_Window_Sink> sink_terminal = nullptr;
			if (sink_terminal == nullptr)
				sink_terminal = std::make_shared<Editor::Terminal_Window_Sink>();
			return sink_terminal;
		}

		void Terminal_Window_Sink::Clear_All_Logs() {
			terminalLogs.clear();
		} 

		void Terminal_Window_Sink::sink_it_(const spdlog::details::log_msg& msg) {
			spdlog::memory_buf_t buffer;
			//Using formatter_ of the base class to format the msg
			this->formatter_->format(msg, buffer);

			terminalLogs.push_back({ fmt::to_string(buffer), msg.level });
		}
	}
}