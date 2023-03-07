#include "stdafx.h"
#include "Engine.h"
#include "GameApp.h"
#include "WorldRender.h"

//https://www.youtube.com/watch?v=fSjc8vLMg8c
//
//сектор
//<portal ? > -где переход в другой сектор
//
//https ://github.com/victorfisac/Physac
//
//https://shrines.rpgclassics.com/fds/deepdungeon/info.shtml
//
//https://www.youtube.com/@LeoOno/videos
//
//https://www.youtube.com/channel/UCjdHbo8_vh3rxQ-875XGkvw/community?lb=UgkxAhhnkGgELDWMs41ygSHgOm2tWV5nTrty

#if defined(_WIN32)
constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
	fragmentColor = vertexColor;
	TexCoord      = vertexTexCoord;
}
)";
#elif defined(__EMSCRIPTEN__)
constexpr const char* vertexShaderText = R"(#version 300 es

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
	fragmentColor = vertexColor;
	TexCoord      = vertexTexCoord;
}
)";
#endif

#if defined(_WIN32)
constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 fragmentColor;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(fragmentColor, 1.0);
}
)";
#elif defined(__EMSCRIPTEN__)
constexpr const char* fragmentShaderText = R"(#version 300 es
precision mediump float;

in vec3 fragmentColor;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(fragmentColor, 1.0);
}
)";
#endif

struct testVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};

testVertex vert[] =
{
	{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
	{{-1.0f,  1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{ 1.0f, -1.0f, 4.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

int indexs[] =
{
	0, 1, 2,
	2, 3, 0
};


ShaderProgram shader;
Uniform uniformProj;
Uniform uniformView;
VertexBuffer vb;
IndexBuffer ib;
VertexArray vao;
Texture2D texture;
scene::Camera cam;


void mainLoop() 
{ 
	float mouseSensitivity = 10.0f * app::GetDeltaTime();
	float moveSpeed = 10.0f * app::GetDeltaTime();

	if( app::IsKeyDown(app::KEY_ESCAPE) ) app::Exit();

	if( app::IsKeyDown(app::KEY_W) ) scene::CameraMoveBy(cam, moveSpeed);
	if( app::IsKeyDown(app::KEY_S) ) scene::CameraMoveBy(cam, -moveSpeed);
	if( app::IsKeyDown(app::KEY_A) ) scene::CameraStrafeBy(cam, moveSpeed);
	if( app::IsKeyDown(app::KEY_D) ) scene::CameraStrafeBy(cam, -moveSpeed);

	glm::vec2 delta = app::GetMouseDelta();
	if (delta.x != 0.0f ) scene::CameraRotateLeftRight(cam, delta.x * mouseSensitivity);
	if( delta.y != 0.0f ) scene::CameraRotateUpDown(cam, -delta.y * mouseSensitivity);

	const float aspectRatio = (float)app::GetWindowWidth() / (float)app::GetWindowHeight();
	glm::mat4 mat = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);

	app::BeginFrame();

	render::Bind(shader);
	render::SetUniform(uniformProj, mat);
	render::SetUniform(uniformView, scene::GetCameraViewMatrix(cam));

	render::Bind(texture);
	render::Draw(vao);

	DrawWorldRender();

	app::EndFrame();
}

int main()
{
	if( app::Create({}) )
	{
		app::SetMouseLock(true);

		shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
		uniformProj = render::GetUniform(shader, "projectionMatrix");
		uniformView = render::GetUniform(shader, "viewMatrix");
		vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert);
		ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
		vao = render::CreateVertexArray(&vb, &ib, shader);


		render::Texture2DInfo texInfo;
		texInfo.mipmap = false;
		texture = render::CreateTexture2D("../data/textures/tile.png", texInfo);

		cam.position = { 0.0f, 0.0f, 0.0f };
		cam.viewPoint = { 0.0f, 0.0f, 1.0f };
		cam.upVector = { 0.0f, 1.0f, 0.0f };

		CreateWorldRender();

		glEnable(GL_DEPTH_TEST);

#if defined(__EMSCRIPTEN__)
		emscripten_set_main_loop(mainLoop, 0, true);
#else
		while( !app::IsClose() )
		{
			mainLoop();
		}
#endif

		render::DestroyResource(shader);
		render::DestroyResource(vb);
		render::DestroyResource(ib);
		render::DestroyResource(vao);
		render::DestroyResource(texture);

	}
	app::Destroy();
}