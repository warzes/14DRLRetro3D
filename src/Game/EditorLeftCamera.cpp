#include "stdafx.h"
#include "EditorLeftCamera.h"
#include "Engine.h"
#include "EditorConstant.h"

void EditorLeftCamera::Update(float deltaTime)
{
	if( app::IsKeyDown(app::KEY_A) ) cam.x -= CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_D) ) cam.x += CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_W) ) cam.y -= CameraSpeedInEditorLeftPanel * deltaTime;
	if( app::IsKeyDown(app::KEY_S) ) cam.y += CameraSpeedInEditorLeftPanel * deltaTime;
	if( cam.x < 0.0f ) cam.x = 0.0f;
	if( cam.y < 0.0f ) cam.y = 0.0f;
	if( cam.x > gridSize-viewSize ) cam.x = gridSize-viewSize;
	if( cam.y > gridSize-viewSize ) cam.y = gridSize-viewSize;

	//std::string text = "cam=" + std::to_string(cam.x) + "-" + std::to_string(cam.y);
	//LogPrint(text);
}