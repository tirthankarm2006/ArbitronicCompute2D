#include "ARBpch.h"
#include "UIBackend.h"

namespace ARB {
	namespace Editor {
		bool UIBackend::isInit;

		bool UIBackend::InitiateImguiBackend(GLFWwindow* window) {
			if (!isInit) {
				//Setup Dear ImGui context
				IMGUI_CHECKVERSION();
				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;// Enable Gamepad Controls
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;// IF using Docking Branch
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;// IF using Docking Branch
				ImGui_ImplOpenGL3_Init();
				isInit = true;

				return ImGui_ImplGlfw_InitForOpenGL(window, true);// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
			}
			else
				return false;
		}

		void UIBackend::ShutdownImguiBackend() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			isInit = false;
		}

		void UIBackend::CreateNewImguiFrame() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
		void UIBackend::RenderAllWindows() {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
			ImGui::EndFrame();
		}

		void UIBackend::setUITheme() {
			auto& colors = ImGui::GetStyle().Colors;

			colors[ImGuiCol_WindowBg] = IM_WIN_BG_COLOR;

			// Headers
			colors[ImGuiCol_Header] = IM_WIN_HEADER_COLOR;
			colors[ImGuiCol_HeaderHovered] = IM_WIN_HEADER_HOVERED_COLOR;
			colors[ImGuiCol_HeaderActive] = IM_WIN_HEADER_ACTIVE_COLOR;

			// Buttons
			colors[ImGuiCol_Button] = IM_BUTTON_COLOR;
			colors[ImGuiCol_ButtonHovered] = IM_BUTTON_HOVERED_COLOR;
			colors[ImGuiCol_ButtonActive] = IM_BUTTON_ACTIVE_COLOR;

			// Frame BG
			colors[ImGuiCol_FrameBg] = IM_FRAMEBG_COLOR;
			colors[ImGuiCol_FrameBgHovered] = IM_FRAMEBG_HOVERED_COLOR;
			colors[ImGuiCol_FrameBgActive] = IM_FRAMEBG_ACTIVE_COLOR;

			// Tabs
			colors[ImGuiCol_Tab] = IM_TAB_COLOR;
			colors[ImGuiCol_TabHovered] = IM_TAB_HOVERED_COLOR;
			colors[ImGuiCol_TabActive] = IM_TAB_ACTIVE_COLOR;
			colors[ImGuiCol_TabUnfocused] = IM_TAB_UNFOCUSED_COLOR;
			colors[ImGuiCol_TabUnfocusedActive] = IM_TAB_UNFOCUSED_ACTIVE_COLOR;

			// Title
			colors[ImGuiCol_TitleBg] = IM_TITLEBG_COLOR;
			colors[ImGuiCol_TitleBgActive] = IM_TITLEBG_ACTIVE_COLOR;
			colors[ImGuiCol_TitleBgCollapsed] = IM_TITLEBG_HOVERED_COLOR;
		}
	}
}