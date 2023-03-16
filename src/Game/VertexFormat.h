#pragma once

struct VertexPos3
{
	glm::vec3 pos;
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