#pragma once

#include "Engine.h"

struct AtlasTexture
{
	Texture2D texture;
	glm::vec2 posInAtlas;
	glm::vec2 sizeInAtlas;
};

//=============================================================================
// Main func
//=============================================================================

bool CreateDefaultTextures();
void DestroyDefaultTextures();