#include "stdafx.h"
#include "EditorLeftPanel.h"
#include "EditorConstant.h"
#include "Engine.h"
#include "ShaderCode.h"
#include "VertexFormat.h"

struct testPoint
{
	glm::vec2 pos;
	glm::vec2 realPos;
};

std::vector<testPoint> testPoints;

testPoint Cursor;

bool EditorLeftPanel::Create()
{
	constexpr VertexPos3 vert[] =
	{
		{{-500.0f, -500.0f, 0.0f}},
		{{-500.0f,  500.0f, 0.0f}},
		{{ 500.0f,  500.0f, 0.0f}},
		{{ 500.0f, -500.0f, 0.0f}}
	};

	constexpr int indexs[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	m_vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(VertexPos3), vert);
	m_ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
	m_vao = render::CreateVertexArray(&m_vb, &m_ib, SimpleColorShader);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.IniFilename = nullptr; // не нужно хранить файл настроек

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	extern GLFWwindow* Window;

	ImGui_ImplGlfw_InitForOpenGL(Window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);


	if( !m_grid.Create() )
		return false;

	if( !m_cursor.Create() )
		return false;

	if ( !m_map.Create() )
		return false;

	return true;
}

void EditorLeftPanel::Destroy()
{
	m_map.Destroy();
	m_cursor.Destroy();
	m_grid.Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	render::DestroyResource(m_vb);
	render::DestroyResource(m_ib);
	render::DestroyResource(m_vao);
}

void EditorLeftPanel::Update(float deltaTime)
{
	if (!m_isActive) return;

	m_leftViewport.Update(deltaTime);
	m_cursor.Update(m_leftViewport, m_grid);
	m_commands.Update(m_cursor, m_map);


	glm::vec2 realMousePos = app::GetMousePosition();
	glm::ivec2 pos;
	glm::vec2 offset;
	glm::vec2 sizeCell;
	getMouseInfo(realMousePos, pos, offset, sizeCell);
	glm::ivec2 offsetTest = glm::ivec2(0);
	if (offset.x > 0.5f) offsetTest.x = 1;
	if (offset.y > 0.5f) offsetTest.y = 1;

	Cursor.pos = glm::vec2(pos.x + offsetTest.x, pos.y + offsetTest.y);
	Cursor.realPos = glm::vec2(Cursor.pos.x * gridStep, Cursor.pos.y * gridStep);

	/*if (app::IsMouseButtonPressed(0))
	{
		std::string text = "Mouse pos=" + std::to_string(pos.x) + ":" + std::to_string(pos.y);
		text += "- Offset=" + std::to_string(offset.x) + ":" + std::to_string(offset.y);
		text += "- Real=" + std::to_string(realMousePos.x) + ":" + std::to_string(realMousePos.y);
		LogPrint(text);
		testPoints.push_back(Cursor);
	}*/
}

void EditorLeftPanel::Draw(float deltaTime)
{
	(void)deltaTime;

	glDisable(GL_DEPTH_TEST);
	m_leftViewport.SetOpenGLViewport();
	m_grid.Draw(m_leftViewport);
	m_map.Draw(m_leftViewport);
	m_cursor.Draw(m_leftViewport);
	return;






	//if (m_isActive)
	{
		// масштаб экрана равен halfScreenWidth / viewSize, то есть 800 / 100, тогда
		// 10 в вьюве - это 80 реально
		// а 80 в реале (например мышь) - это 10 в вьюве


		glm::mat4 proj = glm::ortho(0.0f, (float)viewSize, (float)viewSize, 0.0f, -1.0f, 1.0f);

		glm::mat4 view = glm::lookAt(glm::vec3(m_leftViewport.GetCameraPosition().x, m_leftViewport.GetCameraPosition().y, -0.5f), glm::vec3(m_leftViewport.GetCameraPosition().x, m_leftViewport.GetCameraPosition().y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		


		render::Bind(SimpleColorShader);
		render::SetUniform(SimpleColorDrawProj, proj);
		render::SetUniform(SimpleColorDrawView, view);
		render::SetUniform(SimpleColorDrawColor, glm::vec3(0.1f, 0.2f, 0.3f));

		for (int x = 0; x <= gridSize; x += gridStep)
		{
			//if (x <= viewSize + m_2dCam.x) // TODO: проверить что невидимое выброшено (через рендердок)
			{
				glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f));
				world = glm::scale(world, glm::vec3(gridLineSize, 1.0f, 1.0f));
				render::SetUniform(SimpleColorDrawWorld, world);
				render::Draw(m_vao);
			}

			//if (x <= viewSize + m_2dCam.y) // TODO: проверить что невидимое выброшено (через рендердок)
			{
				glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, x, 0.0f));
				world = glm::scale(world, glm::vec3(1.0f, gridLineSize, 1.0f));
				render::SetUniform(SimpleColorDrawWorld, world);
				render::Draw(m_vao);
			}
		}

		render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 1.0f, 1.0f));
		for (size_t i = 0; i < testPoints.size(); i++)
		{
			glm::mat4 world = glm::translate(glm::mat4(1.0f),
				glm::vec3(
					testPoints[i].realPos.x,
					testPoints[i].realPos.y,
					0.0f));
			world = glm::scale(world, glm::vec3(gridLineSize));
			render::SetUniform(SimpleColorDrawWorld, world);
			render::Draw(m_vao);
		}

		// draw cursor
		render::SetUniform(SimpleColorDrawColor, glm::vec3(1.0f, 1.0f, 0.3f));
		glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(Cursor.realPos.x, Cursor.realPos.y, 0.0f));
		world = glm::scale(world, glm::vec3(gridLineSize*2));
		render::SetUniform(SimpleColorDrawWorld, world);
		render::Draw(m_vao);


		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if( show_demo_window )
		//	ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if( ImGui::Button("Button") )                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		//if( show_another_window )
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if( ImGui::Button("Close Me") )
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void EditorLeftPanel::SetActive(bool active)
{
	m_isActive = active;
}

void EditorLeftPanel::getMouseInfo(const glm::vec2& realPos, glm::ivec2& posInMap, glm::vec2& offset, glm::vec2& sizeCell)
{
	const float halfScreenWidth = (float)app::GetWindowWidth() / 2.0f;
	sizeCell.x = halfScreenWidth / viewSize * gridStep;
	sizeCell.y = (float)app::GetWindowHeight() / viewSize * gridStep;
	
	float posX = realPos.x / sizeCell.x + m_leftViewport.GetCameraPosition().x;
	float posY = realPos.y / sizeCell.y + m_leftViewport.GetCameraPosition().y;

	posInMap.x = static_cast<int>(posX);
	posInMap.y = static_cast<int>(posY);

	offset.x = posX - (float)posInMap.x;
	offset.y = posY - (float)posInMap.y;
}