#include "stdafx.h"
#include "Engine.h"
#include "GameApp.h"

void errorCallback(int error, const char* description) noexcept
{
	Fatal("Error (" + std::to_string(error) + "): " + std::string(description));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

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

int index[] =
{
	0, 1, 2,
	2, 3, 0
};

int main()
{
	LogCreate("../log.txt");

	LogPrint("Hello");

	glfwSetErrorCallback(errorCallback);

	if( !glfwInit() )
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if( !window )
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGL(glfwGetProcAddress);

	ShaderProgram shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	Uniform uniform = render::GetUniform(shader,"projectionMatrix");
	VertexBuffer vb = render::CreateVertexBuffer(render::ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert);
	IndexBuffer ib = render::CreateIndexBuffer(render::ResourceUsage::Static, Countof(index), sizeof(int), index);
	VertexArray vao = render::CreateVertexArray(&vb, &ib, shader);
	Texture2D texture = render::CreateTexture2D("../data/textures/1mx1m.png");

	while( !glfwWindowShouldClose(window) )
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

	render::DestroyResource(shader);
	render::DestroyResource(vb);
	render::DestroyResource(ib);
	render::DestroyResource(vao);
	render::DestroyResource(texture);

	glfwDestroyWindow(window);
	glfwTerminate();

	LogDestroy();
}