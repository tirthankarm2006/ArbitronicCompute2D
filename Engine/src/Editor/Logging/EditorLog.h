#pragma once
#include "Terminal_Window_Sink.h"

namespace ARB {
	namespace Editor {
		class Log
		{
		public:
			Log(std::string name);
			void push_terminal_sink(std::shared_ptr<Terminal_Window_Sink> sink_terminal);
			std::shared_ptr<spdlog::logger> logger;
		private:
			std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> sink_stdout;
		};
	}
}

