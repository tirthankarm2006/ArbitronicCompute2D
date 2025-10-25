#pragma once
#include "ARBpch.h"
#include "EditorWindow.h"
#include "Logging/EditorLog.h"
#include "Logging/Terminal_Window_Sink.h"
#include "InspectorWindowUI.h"
#include "Shader.h"
#include "UIBackend.h"
#include "ComputeShader.h"

namespace ARB {
#define INSPECTOR_WIN_SIZE glm::vec2(600, 880)
#define INSPECTOR_WIN_POS glm::vec2(1020, -50)
#define TERMINAL_WIN_SIZE glm::vec2(1000, 200)
#define TERMINAL_WIN_POS glm::vec2(0, 700)

	class Engine
	{
	private:
		std::unique_ptr<Editor::Log> editorLogger;
		std::shared_ptr<Editor::EditorWindow> appWindow;
		std::shared_ptr<Editor::InspectorWindowUI> inspector1;
		std::shared_ptr<Editor::InspectorWindowUI> terminal;
		void OpenGLLimitations();
		bool recompileComputeShader, shaderCompiledOnce, useCustomShader, initValue_WorkGrp_Invoc_Size_Once;
		unsigned int quadVAO, quadVBO;
		void SetupDisplayQuad();
		std::shared_ptr<Shader> cubeShader;
		std::shared_ptr<ComputeShader> cShader1;
		unsigned int m_TextureID;
		glm::ivec3 workGroupSize, prev_workGroupSize;
		glm::ivec3 invocationSize;
		int fcounter;
		float lastDeltaTime,currentTime;
		void RenderQuad();
		void SetupTexture(unsigned int width, unsigned int height);
		void DispatchCompute();
		void DeleteTexture_Buffers();
		void ShowFrameData();
		void InitValue_WorkGrp_Invoc_Size();
		void EngineTick();
		void Render_Ternimal_Window();
		std::string cShaderPath, cShaderName;
		std::string userVShaderPath, userFShaderPath;
		std::vector<std::string> terminalMsgs;
	public:
		Engine(unsigned int width = 1280, unsigned int height = 720, char* name = "Editor Window", int xPos = 500, int yPos = 500);
		~Engine();
		void runEditor();
		float deltaTime;
		float lastTime;
	};
}