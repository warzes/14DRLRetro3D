#pragma once

struct VertexPos2
{
	glm::vec2 pos;
};

struct VertexPos3
{
	glm::vec3 pos;
};

struct VertexPos2Tex
{
	glm::vec2 pos;
	glm::vec2 texCoord;
};

struct VertexPos3Tex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
};

struct WorldVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};