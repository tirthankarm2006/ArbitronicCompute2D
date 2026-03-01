#include "ARBpch.h"
#include "Engine.h"

namespace ARB {
	void Engine::runEditor() {
		bool setShaderFirst = false;
		char temp_cShaderPath[INPUT_SHADER_LOC_MAX_SIZE] = "";
		char temp_cShaderName[INPUT_SHADER_NAME_MAX_SIZE] = "";
		char temp_userVShaderPath[INPUT_SHADER_LOC_MAX_SIZE] = "", temp_userFShaderPath[INPUT_SHADER_LOC_MAX_SIZE] = "";

		//Printing Maximum number of Work group size and maximum number of invocations that can be deployed
		OpenGLLimitations();

		//Creating an empty texture object for Compute Shader to fill it will values
		SetupTexture(workGroupSize.x, workGroupSize.y);

		//RENDERING THE CUBE
		SetupDisplayQuad();

		//Setting the cube shader texture sampler
		quadShader->useShader();
		quadShader->setIntUniform("material.diffuse", UNIFORM_TEXTURE_LOC_FSC);

		while (!appWindow->windowShouldClose()) {
			appWindow->processInput();
			appWindow->startUpdate();
			if (appWindow->isWindowMinized()) {
				appWindow->endUpdate();
				EngineTick();
				continue;
			}

			Editor::UIBackend::CreateNewImguiFrame();

			inspector1->createFrame();

			//Setting Shader for first time by asking it from the user
			if (!setShaderFirst) {
				if (!initValue_WorkGrp_Invoc_Size_Once) {
					InitValue_WorkGrp_Invoc_Size();
					initValue_WorkGrp_Invoc_Size_Once = true;
				}

				//Input of Compute Shader file path and name
				ImGui::Text("Compute Shader File Path:");
				ImGui::InputText("##shaderPath", &temp_cShaderPath[0], INPUT_SHADER_LOC_MAX_SIZE);
				ImGui::Text("Compute Shader Name:");
				ImGui::InputText("##ShaderName", &temp_cShaderName[0], INPUT_SHADER_NAME_MAX_SIZE);
				cShaderPath = std::string(temp_cShaderPath);
				cShaderName = std::string(temp_cShaderName);

			    if (cShaderPath == "") {
				    ImGui::TextColored(UI_YELLOW, "Empty Shader Path");
			    }
				if (cShaderName == "") {
					ImGui::TextColored(UI_YELLOW, "Empty Shader Name");
				}

				//Optional Use of custom shader for rendering
				ImGui::Checkbox("Use Custom Shader for rendering", &useCustomShader);
				if(useCustomShader){
					ImGui::TextColored(UI_YELLOW, "Custom Shader will require certain variables. See documentation to learn more");
					ImGui::Text("Vertex Shader File Path:");
					ImGui::InputText("##vshaderPath", &temp_userVShaderPath[0], INPUT_SHADER_LOC_MAX_SIZE);
					ImGui::Text("Fragment Shader File Path:");
					ImGui::InputText("##fshaderPath", &temp_userFShaderPath[0], INPUT_SHADER_LOC_MAX_SIZE);
					userVShaderPath = std::string(temp_userVShaderPath);
					userFShaderPath = std::string(temp_userFShaderPath);
					if (userVShaderPath == "") {
						ImGui::TextColored(UI_YELLOW, "Empty Vertex Shader Path");
					}
					if (userFShaderPath == "") {
						ImGui::TextColored(UI_YELLOW, "Empty Fragment Shader Path");
					}
				}
				else
					ImGui::TextColored(UI_YELLOW, "Not giving custom shader will make engine to use it's default shader(Recommended)");

				//Initialize Shader Button
				if (ImGui::Button("Initialize Shader", UI_BUTTON_SIZE)) {
					if (cShaderPath != "" && cShaderName != "") {
						if (!shaderCompiledOnce) {
							cShader1->InitShader(cShaderPath, cShaderName);
							shaderCompiledOnce = true;
						}
						else
						   cShader1->recompileShader(cShaderPath, cShaderName);
						workGroupSize.x = workGroupSize.x / cShader1->invocationSize.x;
						workGroupSize.y = workGroupSize.y / cShader1->invocationSize.y;
						workGroupSize.z = workGroupSize.z / cShader1->invocationSize.z;
						invocationSize = cShader1->invocationSize;
						prev_workGroupSize = workGroupSize;
						setShaderFirst = true;
					}
					if (useCustomShader) {
						quadShader->recompile(userVShaderPath.c_str(), userFShaderPath.c_str(), "cubeShader");
						//Setting the cube shader texture sampler
						quadShader->useShader();
						quadShader->setIntUniform("material.diffuse", UNIFORM_TEXTURE_LOC_FSC);
					}
				}

				ImGui::TextColored(UI_YELLOW, "Make sure you have put a correct shader path");
				inspector1->endFrame();
				Editor::UIBackend::RenderAllWindows();
				appWindow->endUpdate();
				EngineTick();
				continue;
			}
			else {
				if (ImGui::Button("Set New Shader", UI_BUTTON_SIZE)) {
					Editor::Terminal_Window_Sink::Get_Singleton()->Clear_All_Logs();
					setShaderFirst = false;
					initValue_WorkGrp_Invoc_Size_Once = false;
				}
			}

			//Setting Work group Size and Invocation size per work group
			ImGui::InputInt3(":Work Group Size", &workGroupSize[0]);

			ImGui::TextColored(UI_YELLOW, "Window width = Work Group Size X * Invocation Size X");
			ImGui::TextColored(UI_YELLOW, "Window height = Work Group Size Y * Invocation Size Y");
			ImGui::TextColored(UI_YELLOW, "To fully apply changes of Invocation Size onto the Window,\n"
				                                              "first recompile shader then reset Compute shader");

			//Checking whether New Work Group Size if applied or not
			if (prev_workGroupSize.x != workGroupSize.x || prev_workGroupSize.y != workGroupSize.y || prev_workGroupSize.z != workGroupSize.z) {
				ImGui::TextColored(UI_RED,"Reset Window to apply changes for Work Group Size");
			}

			//Checking whether New Invocation Size if applied or not
			if (invocationSize != cShader1->invocationSize) {
				ImGui::TextColored(UI_RED, "Reset Window to apply changes for Invocation Size");
			}

			//Reseting the Window Size/Total parallel threads and reseting the texture size
			if (ImGui::Button("RESET WINDOW", UI_BUTTON_SIZE)) {
				Editor::Terminal_Window_Sink::Get_Singleton()->Clear_All_Logs();
				prev_workGroupSize = workGroupSize;
				invocationSize = cShader1->invocationSize;
				SetupTexture(workGroupSize.x * invocationSize.x, workGroupSize.y * invocationSize.y);
				appWindow->ResetWindowSize(workGroupSize.x * invocationSize.x, workGroupSize.y * invocationSize.y);
				editorLogger->logger->info("Window has been reset with Work Group Size, {0}, {1}, {2}", prev_workGroupSize.x, prev_workGroupSize.y, prev_workGroupSize.z);
				editorLogger->logger->info("Window has been reset with Invocation Size, {0}, {1}, {2}", invocationSize.x, invocationSize.y, invocationSize.z);
			}

			//Getting Parameters through Inspector
			GetParamsOverInspector();

			//Setting all uniform variables value to OpenGL
			cShader1->useShader();
			cShader1->setFloatUniform(UNIFORM_VAR_TIME_LOC_CSC, currentTime);
			cShader1->setFloatUniform(UNIFORM_VAR_DELTA_TIME_LOC_CSC, deltaTime);
			cShader1->setValuesToAllParams();

			//Recompile Button
			if (ImGui::Button("RECOMPILE", UI_BUTTON_SIZE)) {
				Editor::Terminal_Window_Sink::Get_Singleton()->Clear_All_Logs();
				cShader1->recompileShader(cShaderPath, cShaderName);
			}

			//V-Sync button
			SetVSync();

			//FrameTime/FPS values
			ShowFrameData();

			inspector1->endFrame();

			//Rendering the terminal window with all messages
			Render_Ternimal_Window();

			//Dispatching the Compute Data to Computer Shader
			DispatchCompute();
			
			//Rendering the Texture Quad
			RenderQuad();

			Editor::UIBackend::RenderAllWindows();

			appWindow->endUpdate();
			EngineTick();
		}

		DeleteTexture_Buffers();
		cShader1->DeleteAllParameters();
		cShader1->DeleteShaderProgram();
		quadShader->deleteProgram();
		Editor::Terminal_Window_Sink::Get_Singleton()->Clear_All_Logs();

		Editor::UIBackend::ShutdownImguiBackend();
		appWindow->onWindowClosed();
	}

	Engine::Engine(unsigned int width, unsigned int height, char* name, int xPos, int yPos)
		:editorLogger(std::make_unique<Editor::Log>("Engine::Core")), appWindow(std::make_shared<Editor::EditorWindow>(width, height, name, xPos, yPos)),
		inspector1(std::make_shared<Editor::InspectorWindowUI>("Inspector", INSPECTOR_WIN_POS, INSPECTOR_WIN_SIZE)),
		terminal(std::make_shared<Editor::InspectorWindowUI>("Terminal", TERMINAL_WIN_POS, TERMINAL_WIN_SIZE)),
		cShader1(std::make_shared<ComputeShader>()), quadShader(std::make_shared<Shader>("quadShader")),
		recompileComputeShader(false), shaderCompiledOnce(false), initValue_WorkGrp_Invoc_Size_Once(false), fcounter(0),
		lastDeltaTime(0), useCustomShader(false), cShaderPath("_CSPATH_"), cShaderName("_CSNAME_"), vsync(true){

		InitValue_WorkGrp_Invoc_Size();

		if (Editor::UIBackend::InitiateImguiBackend(appWindow->GetWindowPtr()))
			editorLogger->logger->info("{0} successfully linked to ImGui", glfwGetWindowTitle(appWindow->GetWindowPtr()));
		else {
			editorLogger->logger->critical("{0} could not link to ImGui, engaged to some other window", glfwGetWindowTitle(appWindow->GetWindowPtr()));
			return;
		}		
		
		Editor::UIBackend::setUITheme();

		editorLogger->logger->info("Editor Initialized");
	}

	void Engine::GetParamsOverInspector() {		
		if (ImGui::TreeNode("SHADER PARAMETERS")) {
			if((cShader1->m_params->floatParams.size()-2) <= 0)
			   ImGui::TextColored(UI_YELLOW, "FLOAT PARAMETERS");
			for (int i = 0; i < cShader1->m_params->floatParams.size(); i++) {
				ImGui::InputFloat(cShader1->m_params->floatParams[i].name.c_str(), &cShader1->m_params->floatParams[i].value, -10000.0f, 10000.0f, "%.3f");
			}

			if(cShader1->m_params->intParams.size())
			   ImGui::TextColored(UI_YELLOW, "INTEGER PARAMETERS");
			for (int i = 0; i < cShader1->m_params->intParams.size(); i++) {
				ImGui::InputInt(cShader1->m_params->intParams[i].name.c_str(), &cShader1->m_params->intParams[i].value, -10000, 10000);
			}

			if(cShader1->m_params->boolParams.size())
			   ImGui::TextColored(UI_YELLOW, "BOOLEAN PARAMETERS");
			for (int i = 0; i < cShader1->m_params->boolParams.size(); i++) {
				ImGui::Checkbox(cShader1->m_params->boolParams[i].name.c_str(), &cShader1->m_params->boolParams[i].value);
			}

			if(cShader1->m_params->uintParams.size())
			   ImGui::TextColored(UI_YELLOW, "UNSIGNED INTEGER PARAMETERS");
			for (int i = 0; i < cShader1->m_params->uintParams.size(); i++) {
				ImGui::InputScalar(cShader1->m_params->uintParams[i].name.c_str(), ImGuiDataType_U32, &cShader1->m_params->uintParams[i].value);
			}

			if(cShader1->m_params->vec3Params.size())
			    ImGui::TextColored(UI_YELLOW, "FLOAT VECTOR-3 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->vec3Params.size(); i++) {
				ImGui::InputFloat3(cShader1->m_params->vec3Params[i].name.c_str(), &cShader1->m_params->vec3Params[i].value[0], "%.3f");
			}

			if(cShader1->m_params->vec2Params.size())
			    ImGui::TextColored(UI_YELLOW, "FLOAT VECTOR-2 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->vec2Params.size(); i++) {
				ImGui::InputFloat2(cShader1->m_params->vec2Params[i].name.c_str(), &cShader1->m_params->vec2Params[i].value[0], "%.3f");
			}
			
			if(cShader1->m_params->vec4Params.size())
			    ImGui::TextColored(UI_YELLOW, "FLOAT VECTOR-4 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->vec4Params.size(); i++) {
				ImGui::InputFloat4(cShader1->m_params->vec4Params[i].name.c_str(), &cShader1->m_params->vec4Params[i].value[0], "%.3f");
			}

			if(cShader1->m_params->ivec3Params.size())
			    ImGui::TextColored(UI_YELLOW, "INTEGER VECTOR-3 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->ivec3Params.size(); i++) {
				ImGui::InputInt3(cShader1->m_params->ivec3Params[i].name.c_str(), &cShader1->m_params->ivec3Params[i].value[0]);
			}

			if(cShader1->m_params->ivec2Params.size())
			    ImGui::TextColored(UI_YELLOW, "INTEGER VECTOR-2 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->ivec2Params.size(); i++) {
				ImGui::InputInt2(cShader1->m_params->ivec2Params[i].name.c_str(), &cShader1->m_params->ivec2Params[i].value[0]);
			}

			if(cShader1->m_params->ivec4Params.size())
			    ImGui::TextColored(UI_YELLOW, "INTEGER VECTOR-4 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->ivec4Params.size(); i++) {
				ImGui::InputInt4(cShader1->m_params->ivec4Params[i].name.c_str(), &cShader1->m_params->ivec4Params[i].value[0]);
			}

			if(cShader1->m_params->uvec3Params.size())
			    ImGui::TextColored(UI_YELLOW, "UNSIGNED INTEGER VECTOR-3 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->uvec3Params.size(); i++) {
				ImGui::InputScalarN(cShader1->m_params->uvec3Params[i].name.c_str(), ImGuiDataType_U32, &cShader1->m_params->uvec3Params[i].value[0], 3);
			}

			if(cShader1->m_params->uvec2Params.size())
			    ImGui::TextColored(UI_YELLOW, "UNSIGNED INTEGER VECTOR-2 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->uvec2Params.size(); i++) {
				ImGui::InputScalarN(cShader1->m_params->uvec2Params[i].name.c_str(), ImGuiDataType_U32, &cShader1->m_params->uvec2Params[i].value[0], 2);
			}

			if(cShader1->m_params->uvec4Params.size())
			    ImGui::TextColored(UI_YELLOW, "UNSIGNED INTEGER VECTOR-4 PARAMETERS");
			for (int i = 0; i < cShader1->m_params->uvec4Params.size(); i++) {
				ImGui::InputScalarN(cShader1->m_params->uvec4Params[i].name.c_str(), ImGuiDataType_U32, &cShader1->m_params->uvec4Params[i].value[0], 4);
			}

			ImGui::TreePop();
		}
	}

	void Engine::Render_Ternimal_Window() {
		//Terminal Window Beginning frame
		terminal->createFrame();
		//Pooling all the Terminal Logs onto the terminal
		for (int i = 0; i < Editor::Terminal_Window_Sink::Get_Singleton()->Get_Num_Terminal_Log_Msgs(); i++) {
			if (Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Level(i) == LOG_LEVEL_INFO)
				ImGui::TextColored(UI_GREEN, Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Msg(i).c_str());
			else if (Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Level(i) == LOG_LEVEL_ERROR)
				ImGui::TextColored(UI_RED, Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Msg(i).c_str());
			else if (Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Level(i) == LOG_LEVEL_WARN)
				ImGui::TextColored(UI_YELLOW, Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Msg(i).c_str());
			else if (Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Level(i) == LOG_LEVEL_TRACE)
				ImGui::TextColored(UI_WHITE, Editor::Terminal_Window_Sink::Get_Singleton()->Get_Terminal_Log_Msg(i).c_str());
		}
		terminal->endFrame();
	}

	void Engine::EngineTick() {
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
	}

	void Engine::SetVSync() {
		if (ImGui::Button("V-Sync", UI_BUTTON_SIZE)) {
			vsync = (vsync) ? false : true;
			appWindow->setVsync(vsync);
		}
		ImGui::SameLine();
		ImGui::TextColored(UI_WHITE, ((vsync) ? "ON" : "OFF"));
	}

	void Engine::SetupTexture(unsigned int width, unsigned int height) {
		glDeleteTextures(1, &m_TextureID);//deleting texture once before

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

		glActiveTexture(GL_TEXTURE0);
		glBindImageTexture(0, m_TextureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	void Engine::RenderQuad() {
		quadShader->useShader();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void Engine::SetupDisplayQuad() {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	void Engine::DispatchCompute() {
		glDispatchCompute((unsigned int)(prev_workGroupSize.x), (unsigned int)(prev_workGroupSize.y), (unsigned int)(prev_workGroupSize.z));
		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void Engine::DeleteTexture_Buffers() {
		glDeleteTextures(1, &m_TextureID);
		glDeleteBuffers(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}

	void Engine::OpenGLLimitations() {
		int max_compute_work_group_count[3];
		int max_compute_work_group_size[3];
		int max_compute_work_group_invocations;

		for (int i = 0; i < 3; i++) {
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &max_compute_work_group_count[i]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, &max_compute_work_group_size[i]);
		}
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

		editorLogger->logger->trace("Maximum dimension of tensor of workgroups can be deployed: {0}, {1}, {2}", max_compute_work_group_count[0], max_compute_work_group_count[1], max_compute_work_group_count[2]);
		editorLogger->logger->trace("Maximum dimension of a workgroup tensor: {0}, {1}, {2}", max_compute_work_group_size[0], max_compute_work_group_size[1], max_compute_work_group_size[2]);
		editorLogger->logger->trace("Maximum number invocations inside a workgroup: {}", max_compute_work_group_invocations);
	}

	void Engine::ShowFrameData() {
		if (fcounter > 100) {
			ImGui::Text("Frame Time: %f", deltaTime);
			ImGui::Text("FPS: %f", (float)(1 / deltaTime));
			lastDeltaTime = deltaTime;
			fcounter = 0;
		}
		else {
			ImGui::Text("Frame Time: %f", lastDeltaTime);
			ImGui::Text("FPS: %f", (float)(1 / lastDeltaTime));
			fcounter++;
		}
	}

	void Engine::InitValue_WorkGrp_Invoc_Size() {
		workGroupSize = glm::ivec3(appWindow->GetWindowWidth(), appWindow->GetWindowHeight(), 1);
		prev_workGroupSize = workGroupSize;
		invocationSize = glm::ivec3(1, 1, 1);
	}

	Engine::~Engine() {
	}
}
