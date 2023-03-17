#include "stdafx.h"
#include "GameStateEditor.h"
//-----------------------------------------------------------------------------
GameStateEditor gGameStateEditor;
int EditorMapGridSize = 300;
int EditorGridStep = 3;
int EditorViewSize = 90;
//-----------------------------------------------------------------------------
bool GameStateEditor::OnCreate()
{
	if( !createImgui() )
	{
		Fatal("Editor Imgui Init failed!");
		return false;
	}

	if (!m_leftPanel.Create())
	{
		Fatal("Editor Left Panel failed!");
		return false;
	}

	if (!m_rightPanel.Create())
	{
		Fatal("Editor Right Panel failed!");
		return false;
	}

	activeLeftPanel();

	return true;
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnDestroy()
{
	m_leftPanel.Destroy();
	m_rightPanel.Destroy();

	destroyImgui();
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnActive()
{
	activeLeftPanel();
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnUpdate(float deltaTime)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// переключение между левой и правой панелями редактора
	const ImGuiIO& io = ImGui::GetIO();
	if( !io.WantCaptureMouse && !io.WantCaptureKeyboard )
	{
		if( !m_rightPanel.IsActive() )
		{
			// правая панель перехватывает указатель мыши, поэтому мы не можем переключиться на левую мышкой из нее
			if( app::IsMouseButtonPressed(0))
			{
				if( m_leftPanel.IsMouseIn() )
					activeLeftPanel();
				else
					activeRightPanel();
			}
		}
		else
		{
			// если активна правая панель, то выход по esc
			if( app::IsKeyPressed(app::KEY_ESCAPE) )
				activeLeftPanel();
		}

		// обновляем события если не было событий в имгуи
		// TODO: а возможно нужно передавать событие имгуи и апдейт вызывать всегда
		m_leftPanel.Update(deltaTime);
		m_rightPanel.Update(deltaTime);
	}
}
//-----------------------------------------------------------------------------
void GameStateEditor::OnFrame(float deltaTime)
{
	app::BeginFrame();
	
	m_leftPanel.Draw();
	m_rightPanel.Draw(deltaTime);

	ImGui::Begin("Window"); 
	ImGui::Text("Hello window!");
	ImGui::SameLine();
	ImGui::Button("Close");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	app::EndFrame();
}
//-----------------------------------------------------------------------------
bool GameStateEditor::createImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.IniFilename = nullptr; // не нужно хранить файл настроек

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	extern GLFWwindow* Window;
	if( !ImGui_ImplGlfw_InitForOpenGL(Window, true) )
		return false;
	const char* glsl_version = "#version 330";
	if( !ImGui_ImplOpenGL3_Init(glsl_version) )
		return false;

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	// IM_ASSERT(font != NULL);

	return true;
}
//-----------------------------------------------------------------------------
void GameStateEditor::destroyImgui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
//-----------------------------------------------------------------------------
void GameStateEditor::activeLeftPanel()
{
	m_leftPanel.SetActive(true);
	m_rightPanel.SetActive(false);
	app::SetMouseLock(false);
}
//-----------------------------------------------------------------------------
void GameStateEditor::activeRightPanel()
{
	m_leftPanel.SetActive(false);
	m_rightPanel.SetActive(true);
	app::SetMouseLock(true);
}
//-----------------------------------------------------------------------------