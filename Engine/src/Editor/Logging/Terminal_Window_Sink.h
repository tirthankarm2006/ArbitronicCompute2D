#pragma once
#include "ARBpch.h"

namespace ARB {
	namespace Editor {
		class Terminal_Window_Sink : public spdlog::sinks::base_sink<std::mutex> {
		public:
			typedef struct {
				std::string log_msg;
				spdlog::level::level_enum log_level;
			}LogDetail;

			std::vector<LogDetail> terminalLogs;

			static std::shared_ptr<Editor::Terminal_Window_Sink> Get_Singleton() {
				static std::shared_ptr<Editor::Terminal_Window_Sink> sink_terminal = nullptr;
				if (sink_terminal == nullptr)
					sink_terminal = std::make_shared<Editor::Terminal_Window_Sink>();
				return sink_terminal; 
			}
		protected:
			void sink_it_(const spdlog::details::log_msg& msg) override {
				spdlog::memory_buf_t buffer;
				//Using formatter_ of the base class to format the msg
				this->formatter_->format(msg, buffer);

				terminalLogs.push_back({fmt::to_string(buffer), msg.level});
			}
			void flush_() override { }
		};
	}
}