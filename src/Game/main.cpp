#include "stdafx.h"
#include "Engine.h"
#include "GameApp.h"

https://www.youtube.com/watch?v=fSjc8vLMg8c

сектор
<portal ? > -где переход в другой сектор

https ://github.com/victorfisac/Physac

https://shrines.rpgclassics.com/fds/deepdungeon/info.shtml

https://www.youtube.com/@LeoOno/videos

https://www.youtube.com/channel/UCjdHbo8_vh3rxQ-875XGkvw/community?lb=UgkxAhhnkGgELDWMs41ygSHgOm2tWV5nTrty


void errorCallback(int error, const char* description) noexcept
{
	Fatal("Error (" + std::to_string(error) + "): " + std::string(description));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

#if defined(_WIN32)
constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 projectionMatrix;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * vec4(vertexPosition, 1.0);
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

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * vec4(vertexPosition, 1.0);
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

GLFWwindow* window;
ShaderProgram shader;
Uniform uniform;
VertexBuffer vb;
IndexBuffer ib;
VertexArray vao;
Texture2D texture;

void mainLoop() 
{ 
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	const float aspectRatio = (float)width / (float)height;

	glm::mat4 mat = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);

	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	render::Bind(shader);
	render::SetUniform(uniform, mat);
	render::Bind(texture);
	render::Draw(vao);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main()
{
	LogCreate("../log.txt");

	LogPrint("Hello");

	glfwSetErrorCallback(errorCallback);

	if( !glfwInit() )
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if( !window )
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

#if defined(_WIN32)
	gladLoadGL(glfwGetProcAddress);
#endif

	shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	uniform = render::GetUniform(shader,"projectionMatrix");
	vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert);
	ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(indexs), sizeof(int), indexs);
	vao = render::CreateVertexArray(&vb, &ib, shader);
	texture = render::CreateTexture2D("../data/textures/1mx1m.png");


#if defined(_WIN32)
	while( !glfwWindowShouldClose(window) )
	{
		mainLoop();
	}
#elif defined(__EMSCRIPTEN__)
	emscripten_set_main_loop(mainLoop, 0, true);
#endif

	render::DestroyResource(shader);
	render::DestroyResource(vb);
	render::DestroyResource(ib);
	render::DestroyResource(vao);
	render::DestroyResource(texture);

	glfwDestroyWindow(window);
	glfwTerminate();

	LogDestroy();
}