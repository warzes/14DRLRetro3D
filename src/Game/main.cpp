#include "stdafx.h"
#include "Engine.h"
#include "GameApp.h"
#include "GameStateManager.h"
#include "GameStateTest.h"
#include "ShaderCode.h"

#include "trace.h"

//https://www.youtube.com/watch?v=MDusDn8oTSE
//https://www.youtube.com/watch?v=v1riwEd0BDA
//https://github.com/SirJonthe/retro3d
//либу коллизий (на основе моего матана) в отдельном репо

//идеи
//графику для шутера взять отсюда - https://tommulgrew.itch.io/trial-of-the-sorcerer
//https ://apapappa.itch.io/yaw
//https://itey.itch.io/royal-goblin-hunt


//https://www.youtube.com/watch?v=fSjc8vLMg8c
//
//сектор
//<portal ? > -где переход в другой сектор
//
//https ://github.com/victorfisac/Physac
//
//https://shrines.rpgclassics.com/fds/deepdungeon/info.shtml
//
//https://www.youtube.com/@LeoOno/videos
//
//https://www.youtube.com/channel/UCjdHbo8_vh3rxQ-875XGkvw/community?lb=UgkxAhhnkGgELDWMs41ygSHgOm2tWV5nTrty

GameStateManager gGameStateManager;

void mainLoop() 
{ 
	gGameStateManager.Update(app::GetDeltaTime());
	gGameStateManager.Frame(app::GetDeltaTime());
}

int main()
{
//	coll_geom_t geom;
//	// load geometry from OBJ file
//	coll_geom_load(&geom, "../rock.obj", 0);
//
//
//	bpcd_grid_t grid;
//	// load BPCD (Broad Phase Collision Detection) grid
//	coll_geom_load_bpcd_grid(&geom, &grid, 0.1f/*GRID_CELL_SIZE*/);
//
//	coll_geom_trace_test_t test;
//	// initialize trace test - specify NULL for grid to perform full culling (if polygon count is low)
//	coll_geom_trace_test_init(&test, &geom, &grid, COLL_GEOM_TRACE_SPHERE, 0);
//
//	//given two points p1 and p2, and a vector v going from p1 to p2 
//	float3 p1, p2, v;
//	f3make(v, p1, p2);
//
//	sphere_t sphere;
//	f3copy(sphere.c, p1);
//	sphere.radius = 0.1f/*RADIUS*/;
//
//	//break trace down into multiple steps to avoid clipping for larger movements
//	int32 steps = 8;
//	f3muls(v, 1.0f / (float)steps);
//
//	for (int32 i = 0; i < steps; i++) {
//		coll_geom_trace_test_sphere(&test, sphere);
//		coll_geom_trace(&test, v);
//		if (test.stuck)
//			break;
//		f3add(sphere.c, v);
//	}
//
//	//sphere.c now contains the resolved position

	if( app::Create({ .window = {.width = 1600, .height = 900} }) )
	{
		CreateDefaultShader();

		gGameStateManager.AddState(&gGameStateTest);
		gGameStateManager.SetActiveState(&gGameStateTest);
		
#if defined(__EMSCRIPTEN__)
		emscripten_set_main_loop(mainLoop, 0, true);
#else
		while( !app::IsClose() )
		{
			mainLoop();
		}
#endif

		DestroyDefaultShader();

		gGameStateManager.Destroy();
	}
	app::Destroy();
}