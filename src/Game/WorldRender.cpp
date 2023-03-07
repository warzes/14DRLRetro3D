#include "stdafx.h"
#include "WorldRender.h"
#include "Engine.h"

struct WorldVertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};
VertexBuffer worldVB;
VertexArray worldVao;

Sector tempSector;

void CreateWorldRender()
{
	tempSector.FloorHeight = 0.0f;
	tempSector.CeilingHeight = 5.0f;

	tempSector.numWalls.push_back({
		.p1 = {0.0f, 0.0f},
		.p2 = {2.0f, 2.0f},
		.portal = 0,
		.textureId = 1
		});

	tempSector.numWalls.push_back({
		.p1 = {2.0f, 2.0f},
		.p2 = {0.0f, 2.0f},
		.portal = 0,
		.textureId = 2
		});


	tempSector.numWalls.push_back({
	.p1 = {0.0f, 2.0f},
	.p2 = {2.0f, 0.0f},
	.portal = 0,
	.textureId = 3
		});

	extern ShaderProgram shader;

	worldVB = render::CreateVertexBuffer(render::ResourceUsage::Dynamic, 1, sizeof(WorldVertex), nullptr);
	worldVao = render::CreateVertexArray(&worldVB, nullptr, shader);
}

void DrawWorldRender()
{
	std::vector<WorldVertex> data;

	for( int i = 0; i < tempSector.numWalls.size(); i++ )
	{
		WorldVertex vertex = {

		}
	}
}