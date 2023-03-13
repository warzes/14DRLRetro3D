#include "stdafx.h"
#include "ShaderCode.h"

ShaderProgram shaderModel;
Uniform uniformModelWorldMatrix;
Uniform uniformModelViewMatrix;
Uniform uniformModelProjectionMatrix;

ShaderProgram shader;
Uniform uniformProj;
Uniform uniformView;

bool CreateDefaultShader()
{
	shader = render::CreateShaderProgram(vertexShaderText, fragmentShaderText);
	uniformProj = render::GetUniform(shader, "projectionMatrix");
	uniformView = render::GetUniform(shader, "viewMatrix");

	shaderModel = render::CreateShaderProgram(vertexShaderModelText, fragmentShaderModelText);
	uniformModelWorldMatrix = render::GetUniform(shaderModel, "uWorld");
	uniformModelViewMatrix = render::GetUniform(shaderModel, "uView");
	uniformModelProjectionMatrix = render::GetUniform(shaderModel, "uProjection");

	return true;
}

void DestroyDefaultShader()
{
	render::DestroyResource(shader);
	render::DestroyResource(shaderModel);
}