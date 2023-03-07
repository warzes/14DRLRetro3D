#include "stdafx.h"
#include "WorldRender.h"

// TODO: возможно координаты мира не могут быть меньше нуля чтобы не создавать проблемы
// unit - единица из которй вычисляется все остальное - например текстурные координаты

// а как быть с секторами у которых нет прямой связи с камерой, но они попадают в фрустум? это для открытых пространств.
// может быть такое решение - после рендера тек сектора перебрать все заново, и отрисовать попавшие в фрустум, ведь я ставлю метку что сектор уже отрисован, значит повторно не отрисуется.
// но наверное надо упростить

constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec2 vertexTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 FragmentColor;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
	FragmentColor = vertexColor;
	Normal        = vertexNormal;
	TexCoord      = vertexTexCoord;
}
)";

constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 FragmentColor;
in vec3 Normal;
in vec2 TexCoord;

struct DirectionalLight
{
	float Ambient, Diffuse;
	vec3 Direction;
};
uniform DirectionalLight Light;

uniform sampler2D Texture;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(FragmentColor, 1.0);
	float NdotLD = max(dot(Light.Direction, normalize(Normal)), 0.0); // ламберт
	outColor.rgb *= Light.Ambient + Light.Diffuse * NdotLD;
	//float attenuation = saturate(1.0 - DistanceToLight / LightRadius);
	//frag_Color.rgb *= Light.Ambient + Light.Diffuse * NdotLD * attenuation;
}
)";
ShaderProgram SectorShader;
Uniform SectorUniformProj;
Uniform SectorUniformView;
Uniform SectorUniformLightAmbient;
Uniform SectorUniformLightDiffuse;
Uniform SectorUniformLightDirection;

std::vector<Sector> Sectors(3);

void CreateWorldRender()
{
	SectorShader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	SectorUniformProj = render::GetUniform(SectorShader, "projectionMatrix");
	SectorUniformView = render::GetUniform(SectorShader, "viewMatrix");
	SectorUniformLightAmbient = render::GetUniform(SectorShader, "Light.Ambient");
	SectorUniformLightDiffuse = render::GetUniform(SectorShader, "Light.Diffuse");
	SectorUniformLightDirection = render::GetUniform(SectorShader, "Light.Direction");


	// Sector 1
	{
		Sectors[0].FloorHeight = 0.0f;
		Sectors[0].CeilingHeight = 3.0f;

		Sectors[0].walls.push_back({ .p1 = {0.0f, 0.0f}, .p2 = {4.0f, 4.0f}, .portal = 0, .textureId = 1 });
		Sectors[0].walls.push_back({ .p1 = {4.0f, 4.0f}, .p2 = {8.0f, 4.0f}, .portal = 0, .textureId = 2 });
		Sectors[0].walls.push_back({ .p1 = {8.0f, 4.0f}, .p2 = {12.0f, 0.0f}, .portal = 0, .textureId = 3 });
		Sectors[0].walls.push_back({ .p1 = {12.0f, 0.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 3 });
		Sectors[0].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {4.0f, -4.0f}, .portal = 2, .textureId = 3 });
		Sectors[0].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {0.0f, 0.0f}, .portal = 0, .textureId = 3 });

		CompleteSector(Sectors[0]);
	}

	// Sector 2
	{
		Sectors[1].FloorHeight = 0.0f;
		Sectors[1].CeilingHeight = 3.0f;

		Sectors[1].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {8.0f, -10.0f}, .portal = 0, .textureId = 1 });
		Sectors[1].walls.push_back({ .p1 = {8.0f, -10.0f}, .p2 = {4.0f, -10.0f}, .portal = 0, .textureId = 1 });
		Sectors[1].walls.push_back({ .p1 = {4.0f, -10.0f}, .p2 = {4.0f, -4.0f}, .portal = 0, .textureId = 1 });
		Sectors[1].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {8.0f, -4.0f}, .portal = 1, .textureId = 1 });



		//Sectors[1].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {5.0f, -4.0f}, .portal = 0, .textureId = 1 });
		//Sectors[1].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {4.0f, -10.0f}, .portal = 0, .textureId = 1 });
		//Sectors[1].walls.push_back({ .p1 = {4.0f, -10.0f}, .p2 = {8.0f, -12.0f}, .portal = 0, .textureId = 1 });
		//Sectors[1].walls.push_back({ .p1 = {8.0f, -12.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 1 });

		CompleteSector(Sectors[1]);
	}
}

float t = 0.0f;

void DrawWorldRender()
{
	extern scene::Camera cam;

	Sectors[0].FloorHeight = t;

	if (app::IsKeyPressed(app::KEY_Q))
		t += 0.5f;
	if (app::IsKeyPressed(app::KEY_E))
		t -= 0.5f;

	if (IsInside(Sectors[0], cam.position))
	{
		LogPrint("Is inside");
	}
	else
		LogPrint("Is outside");

	const float aspectRatio = (float)app::GetWindowWidth() / (float)app::GetWindowHeight();
	glm::mat4 mat = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);
	glm::vec3 LightDirection = -glm::normalize(cam.viewPoint - cam.position);
		//glm::vec3(0.0f, 0.5f, -1.0f);

	render::Bind(SectorShader);
	render::SetUniform(SectorUniformProj, mat);
	render::SetUniform(SectorUniformView, scene::GetCameraViewMatrix(cam));

	render::SetUniform(SectorUniformLightAmbient, 0.333333f);
	render::SetUniform(SectorUniformLightDiffuse, 0.666666f);
	render::SetUniform(SectorUniformLightDirection, LightDirection);

	DrawSectors(0, Sectors);

	extern scene::Camera cam;

}