#include "stdafx.h"
#include "ShaderCode.h"

ShaderProgram shaderModel;
Uniform uniformModelWorldMatrix;
Uniform uniformModelViewMatrix;
Uniform uniformModelProjectionMatrix;

ShaderProgram shader;
Uniform uniformProj;
Uniform uniformView;

ShaderProgram SectorRenderShader;
Uniform SectorRenderUniformProj;
Uniform SectorRenderUniformView;
Uniform SectorRenderUniformLightAmbient;
Uniform SectorRenderUniformLightDiffuse;
Uniform SectorRenderUniformLightDirection;

ShaderProgram SimpleColorShader;
Uniform SimpleColorDrawProj;
Uniform SimpleColorDrawView;
Uniform SimpleColorDrawWorld;
Uniform SimpleColorDrawColor;

ShaderProgram GridCellShader;
Uniform UniformGridCellProj;
Uniform UniformGridCellView;
Uniform UniformGridCellWorld;
Uniform UniformGridCellColor;

ShaderProgram Simple2DLineDrawShader;
Uniform Simple2DLineDrawViewProj;
Uniform Simple2DLineDrawColor;
Uniform Simple2DLineDrawPos;

bool CreateDefaultShader()
{
	shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	uniformProj = render::GetUniform(shader, "projectionMatrix");
	uniformView = render::GetUniform(shader, "viewMatrix");

	shaderModel = render::CreateShaderProgram(vertexShaderModelText, fragmentShaderModelText);
	uniformModelWorldMatrix = render::GetUniform(shaderModel, "uWorld");
	uniformModelViewMatrix = render::GetUniform(shaderModel, "uView");
	uniformModelProjectionMatrix = render::GetUniform(shaderModel, "uProjection");

	SectorRenderShader = render::CreateShaderProgram(vertexShaderSectorRenderText, fragmentShaderSectorRenderText);
	SectorRenderUniformProj = render::GetUniform(SectorRenderShader, "projectionMatrix");
	SectorRenderUniformView = render::GetUniform(SectorRenderShader, "viewMatrix");
	SectorRenderUniformLightAmbient = render::GetUniform(SectorRenderShader, "Light.Ambient");
	SectorRenderUniformLightDiffuse = render::GetUniform(SectorRenderShader, "Light.Diffuse");
	SectorRenderUniformLightDirection = render::GetUniform(SectorRenderShader, "Light.Direction");

	SimpleColorShader = render::CreateShaderProgram(vertexShaderSimpleColor, fragmentShaderSimpleColor);
	SimpleColorDrawProj = render::GetUniform(SimpleColorShader, "uProjection");
	SimpleColorDrawView = render::GetUniform(SimpleColorShader, "uView");
	SimpleColorDrawWorld = render::GetUniform(SimpleColorShader, "uWorld");
	SimpleColorDrawColor = render::GetUniform(SimpleColorShader, "uColor");

	GridCellShader = render::CreateShaderProgram(vertexShaderGridCellText, fragmentShaderGridCellText);
	UniformGridCellProj = render::GetUniform(GridCellShader, "uProjection");
	UniformGridCellView = render::GetUniform(GridCellShader, "uView");
	UniformGridCellWorld = render::GetUniform(GridCellShader, "uWorld");
	UniformGridCellColor = render::GetUniform(GridCellShader, "uColor");

	Simple2DLineDrawShader = render::CreateShaderProgram(vertexShaderSimple2DLineDraw, fragmentShaderSimple2DLineDraw);
	Simple2DLineDrawViewProj = render::GetUniform(Simple2DLineDrawShader, "uVP");
	Simple2DLineDrawColor = render::GetUniform(Simple2DLineDrawShader, "uColor");
	Simple2DLineDrawPos = render::GetUniform(Simple2DLineDrawShader, "uPos");

	return true;
}

void DestroyDefaultShader()
{
	render::DestroyResource(Simple2DLineDrawShader);
	render::DestroyResource(shader);
	render::DestroyResource(shaderModel);
	render::DestroyResource(SectorRenderShader);
	render::DestroyResource(SimpleColorShader);
}