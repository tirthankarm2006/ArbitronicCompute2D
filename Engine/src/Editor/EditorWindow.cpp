#include"ARBpch.h"
#include "EditorWindow.h"
#include "UIBackend.h"

namespace ARB {
	namespace Editor {
		EditorWindow::EditorWindow(unsigned int width, unsigned int height, char* name, int xpos, int ypos) {
			mainWindow = std::make_shared<WindowProps>();
			windowLogger = std::make_shared<Editor::Log>("Engine::EditorWindow");
			windowLogger->push_terminal_sink();

			GLFWimage image;

			if (glfwInit()) {
				windowLogger->logger->info("GLFW Initiated");
			}
			else
				windowLogger->logger->error("GLFW could not be initiated");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			mainWindow->height = height;
			mainWindow->width = width;
			mainWindow->windowName = name;
			mainWindow->window = glfwCreateWindow(width, height, name, NULL, NULL);
			glfwSetWindowPos(mainWindow->window, xpos, ypos);
			image.pixels = stbi_load("ARBC2D_Icon.png", &image.width, &image.height, nullptr, 4);
			glfwSetWindowIcon(mainWindow->window, 1, &image);

			if (mainWindow->window) {
				windowLogger->logger->info("{} window has been created", mainWindow->windowName);
			}
			else
				windowLogger->logger->error("{} window could not be created", mainWindow->windowName);

			glfwMakeContextCurrent(mainWindow->window);
			glfwSetWindowUserPointer(mainWindow->window, &mainWindow);

			glfwSetFramebufferSizeCallback(mainWindow->window, [](GLFWwindow* window, int width, int height)
			{
				glViewport(0, 0, width, height);
				std::shared_ptr<WindowProps> win = *(std::shared_ptr<WindowProps>*)glfwGetWindowUserPointer(window);
				win->height = height;
				win->width = width;	
			});

			glfwSetWindowIconifyCallback(mainWindow->window, [](GLFWwindow* window, int iconified) {
				//If the glfw window is iconified, we shutdown the imgui backend and if it is restored, we restore the imgui backend
				if (iconified == GLFW_TRUE) {
					UIBackend::ShutdownImguiBackend();
				}
				else {
					UIBackend::InitiateImguiBackend(window);
					Editor::UIBackend::setUITheme();
				}
			});

			glfwSetInputMode(mainWindow->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSwapInterval(0);

			if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				windowLogger->logger->info("GLAD Initiated");
			}
			else
				windowLogger->logger->error("GLAD could not be initiated");

		}

		void EditorWindow::ResetWindowSize(unsigned int width, unsigned int height) {
			mainWindow->width = width;
			mainWindow->height = height;
			glfwSetWindowSize(mainWindow->window, width, height);
			windowLogger->logger->trace("Window Sized to {0} X {1}", width, height);
		}

		EditorWindow::~EditorWindow() {
		}

		void EditorWindow::processInput() {
			if (glfwGetKey(mainWindow->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				closeWindow();
		}

		int EditorWindow::windowShouldClose() {
			return glfwWindowShouldClose(mainWindow->window);
		}

		void EditorWindow::onWindowClosed() {
			glfwDestroyWindow(mainWindow->window);
			windowLogger->logger->info("{} window has been destroyed", mainWindow->windowName);
			glfwTerminate();
			windowLogger->logger->info("GLFW terminated");
		}

		void EditorWindow::closeWindow() {
			glfwSetWindowShouldClose(mainWindow->window, true);
		}

		bool EditorWindow::isWindowMinized() {
			return glfwGetWindowAttrib(mainWindow->window, GLFW_ICONIFIED);
		}

		void EditorWindow::startUpdate() {
			glClearColor(0.12f, 0.12f, 0.12f, 0.12f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		void EditorWindow::endUpdate() {
			glfwSwapBuffers(mainWindow->window);
			glfwPollEvents();
		}
	}
}