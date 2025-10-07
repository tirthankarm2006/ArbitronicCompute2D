#pragma once
#include "My_Struct_Sink.h"

namespace ARB {
#define BIT(x) (1 << x)
	namespace Editor {

		enum SINK_TYPE {
			STD_OUT = BIT(0),
			CUSTOM_TERMINAL = BIT(1)
		};

		class Log
		{
		public:
			Log(std::string name);
			void push_terminal_sink(std::shared_ptr<My_Struct_Sink> sink_terminal);
			std::shared_ptr<spdlog::logger> logger;
		private:
			std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> sink_stdout;
		};
	}
}

