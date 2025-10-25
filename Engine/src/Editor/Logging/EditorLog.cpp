#include "ARBpch.h"
#include "EditorLog.h"

namespace ARB{
	namespace Editor {
		Log::Log(std::string name) {
			sink_stdout = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
		    logger = std::make_shared<spdlog::logger>(name);
			logger->sinks().push_back(sink_stdout);
			logger->set_level(spdlog::level::trace);
			logger->set_pattern("[%n]%^[%l]:  %v%$ [%r]");
		}

		void Log::push_terminal_sink(std::shared_ptr<Terminal_Window_Sink> sink_terminal) {
			logger->sinks().push_back(sink_terminal);
		}
	}
}