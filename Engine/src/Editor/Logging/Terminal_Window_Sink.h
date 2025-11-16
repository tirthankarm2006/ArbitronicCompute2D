#pragma once

namespace ARB {
	namespace Editor {
#define LOG_LEVEL spdlog::level::level_enum
#define LOG_LEVEL_INFO spdlog::level::info
#define LOG_LEVEL_WARN spdlog::level::warn
#define LOG_LEVEL_ERROR spdlog::level::err
#define LOG_LEVEL_TRACE spdlog::level::trace

		class Terminal_Window_Sink : public spdlog::sinks::base_sink<std::mutex> {	
		private:
			typedef struct {
				std::string log_msg;
				LOG_LEVEL log_level;
			}LogDetail;
			std::vector<LogDetail> terminalLogs;
		public:
			static std::shared_ptr<Editor::Terminal_Window_Sink>& Get_Singleton();
			void Clear_All_Logs();
			inline std::string& Get_Terminal_Log_Msg(int i) { return terminalLogs[i].log_msg; }
			inline LOG_LEVEL& Get_Terminal_Log_Level(int i) { return terminalLogs[i].log_level; }
			inline int Get_Num_Terminal_Log_Msgs() { return terminalLogs.size(); }
		protected:
			void sink_it_(const spdlog::details::log_msg& msg) override;
			void flush_() override { }
		};
	}
}