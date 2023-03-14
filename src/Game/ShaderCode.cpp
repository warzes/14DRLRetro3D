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

ShaderProgram LineDrawShader;
Uniform UniformLineDrawProj;
Uniform UniformLineDrawView;
Uniform UniformLineDrawWorld;
Uniform UniformLineDrawColor;

ShaderProgram GridCellShader;
Uniform UniformGridCellProj;
Uniform UniformGridCellView;
Uniform UniformGridCellWorld;
Uniform UniformGridCellColor;

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

	LineDrawShader = render::CreateShaderProgram(vertexShaderLineDrawText, fragmentShaderLineDrawText);
	UniformLineDrawProj = render::GetUniform(LineDrawShader, "uProjection");
	UniformLineDrawView = render::GetUniform(LineDrawShader, "uView");
	UniformLineDrawWorld = render::GetUniform(LineDrawShader, "uWorld");
	UniformLineDrawColor = render::GetUniform(LineDrawShader, "uColor");

	GridCellShader = render::CreateShaderProgram(vertexShaderGridCellText, fragmentShaderGridCellText);
	UniformGridCellProj = render::GetUniform(GridCellShader, "uProjection");
	UniformGridCellView = render::GetUniform(GridCellShader, "uView");
	UniformGridCellWorld = render::GetUniform(GridCellShader, "uWorld");
	UniformGridCellColor = render::GetUniform(GridCellShader, "uColor");

	return true;
}

void DestroyDefaultShader()
{
	render::DestroyResource(shader);
	render::DestroyResource(shaderModel);
	render::DestroyResource(SectorRenderShader);
	render::DestroyResource(LineDrawShader);
}