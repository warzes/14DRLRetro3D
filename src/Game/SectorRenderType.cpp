#include "stdafx.h"
#include "SectorRenderType.h"
#include "crc32.h"

Build2 build2;

Tile *gtile;
long gnumtiles = 0;
long gmaltiles = 256;
long gtilehashead[1024];

static PlayerData *gps;
PlayerData *gdps; //global temp variable

GameState *gst;
GameState sst, pst;

static int moveindex = 0;
static int viewindex = 0;


static unsigned char gammlut[256], gotpal = 0;
void setgammlut(double gammval)
{
	long i;

	gammval = 1.0 / gammval;
	for( i = 0; i < 256; i++ ) gammlut[i] = pow(((double)i) * (1.0 / 256.0), gammval) * 256.0;

	//Remove all loaded tiles from memory
	if( gtile )
		for( i = gnumtiles - 1; i >= 0; i-- )
			if( gtile[i].tt.f ) { free((void *)gtile[i].tt.f); gtile[i].tt.f = 0; }

	gotpal = 0; //Force palette to reload
}

#ifndef STANDALONE
void Start()
{
	LogPrint("SectorApp Start");

	initcrc32();
	
	gnumtiles = 0; memset(gtilehashead, -1, sizeof(gtilehashead));
	gmaltiles = 256;

	gtile = (Tile*)malloc(gmaltiles * sizeof(Tile));

	gst = &sst;
	gst->numsects = 0;
	gst->malsects = 256;
	gst->sect = (Sector*)malloc(gst->malsects * sizeof(Sector));
	memset(gst->sect, 0, gst->malsects * sizeof(Sector));
	
	gst->numspris = 0;
	gst->malspris = 256;
	gst->spri = (Sprite *)malloc(gst->malspris * sizeof(Sprite));
	memset(gst->spri, 0, gst->malspris * sizeof(Sprite));

	gst->blankheadspri = -1;
	for( int i = 0; i < gst->malspris; i++ )
	{
		gst->spri[i].sectn = gst->blankheadspri;
		gst->spri[i].sectp = -1;
		gst->spri[i].sect = -1;
		if( gst->blankheadspri >= 0 ) gst->spri[gst->blankheadspri].sectp = i;
		gst->blankheadspri = i;
	}

	build2.gammaval = 1.0; //1.0=no change, useful range={0.0..~4.0)
	setgammlut(build2.gammaval); //Safe here because gst exists and gnumtiles = 0

	gettileind("..\\tex\\cloud.png"); //force it to be index 0

	gst->light_sprinum = 0;

	gst->startpos.x = 0.0; gst->startpos.y = 0.0; gst->startpos.z = 0.0;
	gst->startrig.x = 1.0; gst->startrig.y = 0.0; gst->startrig.z = 0.0;
	gst->startdow.x = 0.0; gst->startdow.y = 0.0; gst->startdow.z = 1.0;
	gst->startfor.x = 0.0; gst->startfor.y = -1.0; gst->startfor.z = 0.0;

	//for( i = 0; i < numplayers; i++ )
	{
		gps = &gst->p;
		memset(gps, 0, sizeof(PlayerData));
#ifdef STANDALONE
		gps->fcmousx = xres / 2;
		gps->fcmousy = yres / 2;
		gps->grabsect = -1; gps->grabwall = -1; gps->grabmode = -1; gps->grabcf = 0;
		gps->playerindex = i;
		gps->circnum = 7;
		gps->docollide = 1;
		gps->gridlock = 1;
		gps->boolfunc = POLYBOOL_END;
		gps->showdebug = 0;
		gps->showedges3d = 1;
#endif
		gps->dgridlock = 1.0 / 8.0;
		gps->compact2d = 1.0;
		gps->goalheight2d = 24.0;
		gps->editmode = 3;
		gps->cursect = -1;
		gps->rendheight = 0;
		gps->rendinterp = 0;

		gps->ipos.x = 0; gps->ipos.y = 0; gps->ipos.z = -gps->goalheight2d;
		gps->irig.x = 1; gps->irig.y = 0; gps->irig.z = 0;
		gps->idow.x = 0; gps->idow.y = 1; gps->idow.z = 0;
		gps->ifor.x = 0; gps->ifor.y = 0; gps->ifor.z = 1;

		gps->grdc.x = 0; gps->grdc.y = 0; gps->grdc.z = 0; //center
		gps->grdu.x = 1; gps->grdu.y = 0; gps->grdu.z = 0;
		gps->grdv.x = 0; gps->grdv.y = 1; gps->grdv.z = 0;
		gps->grdn.x = 0; gps->grdn.y = 0; gps->grdn.z = 1; //normal

		gps->ghx = 640/*xres*/ / 2;
		gps->ghy = 480/*yres*/ / 2;
		gps->ghz = gps->ghx;
		gps->zoom = gps->ozoom = 1.f;
	}
	
	gps->compact2d = 0.0;

	build2.fattestsprite = 2.0;

	memcpy(&pst, &sst, sizeof(GameState));
	pst.sect = (Sector *)malloc(pst.malsects * sizeof(Sector));
	pst.spri = (Sprite *)malloc(pst.malspris * sizeof(Sprite));
	memset(pst.sect, 0, pst.malsects * sizeof(Sector));
	memset(pst.spri, 0, pst.malspris * sizeof(Sprite));
}
#endif