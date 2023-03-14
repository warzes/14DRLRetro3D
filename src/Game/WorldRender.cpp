#include "stdafx.h"
#include "WorldRender.h"
#include "ShaderCode.h"

// TODO: возможно координаты мира не могут быть меньше нуля чтобы не создавать проблемы
// unit - единица из которй вычисляется все остальное - например текстурные координаты

// а как быть с секторами у которых нет прямой связи с камерой, но они попадают в фрустум? это для открытых пространств.
// может быть такое решение - после рендера тек сектора перебрать все заново, и отрисовать попавшие в фрустум, ведь я ставлю метку что сектор уже отрисован, значит повторно не отрисуется.
// но наверное надо упростить

std::vector<Sector> Sectors(3);

std::vector<Texture2D> textures(6);

void CreateWorldRender()
{
	textures[0] = render::CreateTexture2D("../data/textures/t00.png");
	textures[1] = render::CreateTexture2D("../data/textures/t01.png");
	textures[2] = render::CreateTexture2D("../data/textures/t02.png");
	textures[3] = render::CreateTexture2D("../data/textures/t03.png");
	textures[4] = render::CreateTexture2D("../data/textures/t04.png");
	textures[5] = render::CreateTexture2D("../data/textures/t05.png");

	Sectors = LoadSectorFromFile("../map.map", 0.5f);
	//// Sector 1
	//{
	//	Sectors[0].FloorHeight = 0.0f;
	//	Sectors[0].CeilingHeight = 3.0f;

	//	Sectors[0].walls.push_back({ .p1 = {0.0f, 0.0f}, .p2 = {4.0f, 4.0f}, .portal = 0, .textureId = 1 });
	//	Sectors[0].walls.push_back({ .p1 = {4.0f, 4.0f}, .p2 = {8.0f, 4.0f}, .portal = 0, .textureId = 2 });
	//	Sectors[0].walls.push_back({ .p1 = {8.0f, 4.0f}, .p2 = {12.0f, 0.0f}, .portal = 0, .textureId = 3 });
	//	Sectors[0].walls.push_back({ .p1 = {12.0f, 0.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 3 });
	//	Sectors[0].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {4.0f, -4.0f}, .portal = 2, .textureId = 3 });
	//	Sectors[0].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {0.0f, 0.0f}, .portal = 0, .textureId = 3 });

	//	CompleteSector(Sectors[0]);
	//}

	//// Sector 2
	//{
	//	Sectors[1].FloorHeight = 0.0f;
	//	Sectors[1].CeilingHeight = 3.0f;

	//	Sectors[1].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {8.0f, -10.0f}, .portal = 0, .textureId = 1 });
	//	Sectors[1].walls.push_back({ .p1 = {8.0f, -10.0f}, .p2 = {4.0f, -10.0f}, .portal = 0, .textureId = 1 });
	//	Sectors[1].walls.push_back({ .p1 = {4.0f, -10.0f}, .p2 = {4.0f, -4.0f}, .portal = 0, .textureId = 1 });
	//	Sectors[1].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {8.0f, -4.0f}, .portal = 1, .textureId = 1 });



	//	//Sectors[1].walls.push_back({ .p1 = {8.0f, -4.0f}, .p2 = {5.0f, -4.0f}, .portal = 0, .textureId = 1 });
	//	//Sectors[1].walls.push_back({ .p1 = {4.0f, -4.0f}, .p2 = {4.0f, -10.0f}, .portal = 0, .textureId = 1 });
	//	//Sectors[1].walls.push_back({ .p1 = {4.0f, -10.0f}, .p2 = {8.0f, -12.0f}, .portal = 0, .textureId = 1 });
	//	//Sectors[1].walls.push_back({ .p1 = {8.0f, -12.0f}, .p2 = {8.0f, -4.0f}, .portal = 0, .textureId = 1 });

	//	CompleteSector(Sectors[1]);
	//}
}

float t = 0.0f;

void DrawWorldRender(const scene::Camera& cam)
{
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

	render::Bind(SectorRenderShader);
	render::SetUniform(SectorRenderUniformProj, mat);
	render::SetUniform(SectorRenderUniformView, scene::GetCameraViewMatrix(cam));

	render::SetUniform(SectorRenderUniformLightAmbient, 0.333333f);
	render::SetUniform(SectorRenderUniformLightDiffuse, 0.666666f);
	render::SetUniform(SectorRenderUniformLightDirection, LightDirection);

	DrawSectors(0, Sectors, textures);
	DrawSectors(1, Sectors, textures);
}