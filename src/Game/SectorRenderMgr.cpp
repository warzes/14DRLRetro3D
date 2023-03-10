#include "stdafx.h"
#include "SectorRenderMgr.h"

static int cursect;
static glm::vec3 ipos, irig, idow, ifor;

bool SectorRenderMgr::Create()
{
	Start();
	LoadMap("../test.map", &cursect, &ipos, &irig, &idow, &ifor);
	return true;
}

void SectorRenderMgr::Destroy()
{
}

void SectorRenderMgr::Draw()
{
}
