#pragma once

#include "Engine.h"

//=============================================================================
// Model shader
//=============================================================================

constexpr const char* vertexShaderModelText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec2 vertexTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = uProjection * uView * uWorld * vec4(vertexPosition, 1.0);
	fragmentColor = vertexColor;
	TexCoord      = vertexTexCoord;
}
)";
constexpr const char* fragmentShaderModelText = R"(
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

extern ShaderProgram shaderModel;
extern Uniform uniformModelWorldMatrix;
extern Uniform uniformModelViewMatrix;
extern Uniform uniformModelProjectionMatrix;

//=============================================================================
// Example shader
//=============================================================================

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

extern ShaderProgram shader;
extern Uniform uniformProj;
extern Uniform uniformView;

//=============================================================================
// Sector Render Shader
//=============================================================================

constexpr const char* vertexShaderSectorRenderText = R"(
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

constexpr const char* fragmentShaderSectorRenderText = R"(
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
	float NdotLD2 = max(dot(Light.Direction, normalize(-Normal)), 0.0); // ламберт

	
	outColor.rgb *= Light.Ambient + Light.Diffuse * max(NdotLD, NdotLD2);
	//float attenuation = saturate(1.0 - DistanceToLight / LightRadius);
	//frag_Color.rgb *= Light.Ambient + Light.Diffuse * NdotLD * attenuation;
}
)";
extern ShaderProgram SectorRenderShader;
extern Uniform SectorRenderUniformProj;
extern Uniform SectorRenderUniformView;
extern Uniform SectorRenderUniformLightAmbient;
extern Uniform SectorRenderUniformLightDiffuse;
extern Uniform SectorRenderUniformLightDirection;

//=============================================================================
// Simple Color shader
//=============================================================================
constexpr const char* vertexShaderSimpleColor = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(vertexPosition, 1.0);
}
)";
constexpr const char* fragmentShaderSimpleColor = R"(
#version 330 core

uniform vec3 uColor;

out vec4 outColor;

void main()
{
	outColor = vec4(uColor, 1.0);
}
)";

extern ShaderProgram SimpleColorShader;
extern Uniform SimpleColorDrawProj;
extern Uniform SimpleColorDrawView;
extern Uniform SimpleColorDrawWorld;
extern Uniform SimpleColorDrawColor;

//=============================================================================
// Simple 2d Line Draw shader
//=============================================================================
constexpr const char* vertexShaderSimple2DLineDraw = R"(
#version 330 core

layout(location = 0) in float vertexId;

uniform mat4 uVP;
uniform vec4 uPos;

void main()
{
	gl_Position = uVP * vec4(((vertexId < 1.0) ? uPos.xy : uPos.zw), 0.0, 1.0);
}
)";
constexpr const char* fragmentShaderSimple2DLineDraw = R"(
#version 330 core

uniform vec3 uColor;

out vec4 outColor;

void main()
{
	outColor = vec4(uColor, 1.0);
}
)";

extern ShaderProgram Simple2DLineDrawShader;
extern Uniform Simple2DLineDrawViewProj;
extern Uniform Simple2DLineDrawColor;
extern Uniform Simple2DLineDrawPos;

//=============================================================================
// Grid Cell shader
//=============================================================================
constexpr const char* vertexShaderGridCellText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 TexCoord;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(vertexPosition, 1.0);
	TexCoord = vertexTexCoord;
}
)";
constexpr const char* fragmentShaderGridCellText = R"(
#version 330 core

in vec2 TexCoord;

uniform vec3 uColor;

out vec4 outColor;

uniform sampler2D Texture;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(uColor, 1.0);
}
)";

extern ShaderProgram GridCellShader;
extern Uniform UniformGridCellProj;
extern Uniform UniformGridCellView;
extern Uniform UniformGridCellWorld;
extern Uniform UniformGridCellColor;

//=============================================================================
// Main func
//=============================================================================

bool CreateDefaultShader();
void DestroyDefaultShader();