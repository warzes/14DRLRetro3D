#include "stdafx.h"
#include "SectorRenderType.h"
#include "kplib.h"
#include "crc32.h"

typedef struct { long tilnum, flags, tag; glm::vec2 uv[3]; int dummy[6]; short asc, rsc, gsc, bsc; } osurf1_t;
typedef struct { float x, y; long n, ns, nw; Surface surf; } owall1_t;
typedef struct { float z[2]; glm::vec2 grad[2]; Surface surf[2]; long foglev; Wall *wall; int n, nmax; } osect1_t;

void compacttilelist_tilenums(void) //uses gtile[?].namcrc32 as the lut - a complete hack to avoid extra allocs :P
{
	Sector *sec;
	long s, w;

	sec = gst->sect;
	for( s = gst->numsects - 1; s >= 0; s-- )
	{
		for( w = 2 - 1; w >= 0; w-- ) { sec[s].surf[w].tilnum = gtile[sec[s].surf[w].tilnum].namcrc32; }
		for( w = sec[s].n - 1; w >= 0; w-- ) { sec[s].wall[w].surf.tilnum = gtile[sec[s].wall[w].surf.tilnum].namcrc32; }
#ifndef STANDALONE
		for( w = sec[s].headspri; w >= 0; w = gst->spri[w].sectn )
			if( gst->spri[w].tilnum >= 0 ) gst->spri[w].tilnum = gtile[gst->spri[w].tilnum].namcrc32;
	}
#else
}
	for( w = gst->numspris - 1; w >= 0; w-- )
		if( (unsigned)gst->spri[w].tilnum < (unsigned)gnumtiles ) gst->spri[w].tilnum = gtile[gst->spri[w].tilnum].namcrc32;
	for( w = 0; w < numplayers; w++ )
	{
		if( (unsigned)gst->p[w].copysurf[0].tilnum < (unsigned)gnumtiles ) gst->p[w].copysurf[0].tilnum = gtile[gst->p[w].copysurf[0].tilnum].namcrc32;
		if( (unsigned)gst->p[w].copyspri[0].tilnum < (unsigned)gnumtiles ) gst->p[w].copyspri[0].tilnum = gtile[gst->p[w].copyspri[0].tilnum].namcrc32;
	}
#endif
}


void compacttilelist(long flags)
{
	Sector *sec;
	long i, j, s, w, nnumtiles;

	sec = gst->sect;

	//gtile[?].namcrc32 used as temp in this function (must be reconstructed before returning)

	if( flags & 1 ) //Remove duplicate filenames (call right after load with alt+sectors copied)
	{
		for( i = gnumtiles - 1; i >= 0; i-- ) gtile[i].namcrc32 = i;
		for( s = 0; s < sizeof(gtilehashead) / sizeof(gtilehashead[0]); s++ )
			for( i = gtilehashead[s]; i >= 0; i = gtile[i].hashnext ) //n^2 compare on linked list
			{
				if( !gtile[i].filnam[0] ) continue;
				for( j = gtile[i].hashnext; j >= 0; j = gtile[j].hashnext )
					if( !_stricmp(gtile[i].filnam, gtile[j].filnam) )
					{
						if( gtile[j].tt.f ) { free((void *)gtile[j].tt.f); gtile[j].tt.f = 0; }
						gtile[j].filnam[0] = 0;
						gtile[j].namcrc32 = i;
					}
			}
		compacttilelist_tilenums();

		nnumtiles = 0;
		for( i = 0; i < gnumtiles; i++ )
		{
			if( !gtile[i].filnam[0] ) continue;
			j = gtile[nnumtiles].namcrc32; gtile[nnumtiles] = gtile[i]; gtile[nnumtiles].namcrc32 = j; //copy all except namcrc32
			gtile[i].namcrc32 = nnumtiles; nnumtiles++;
		}
		if( nnumtiles != gnumtiles ) { compacttilelist_tilenums(); gnumtiles = nnumtiles; }
	}

	if( flags & 2 ) //Remove unused tiles (call just before save)
	{
		for( i = 0; i < gnumtiles; i++ ) gtile[i].namcrc32 = 0;
		gtile[0].namcrc32 = 1; //Keep default tile (cloud.png)
		for( s = gst->numsects - 1; s >= 0; s-- )
		{
			for( w = 2 - 1; w >= 0; w-- ) gtile[sec[s].surf[w].tilnum].namcrc32 = 1;
			for( w = sec[s].n - 1; w >= 0; w-- ) gtile[sec[s].wall[w].surf.tilnum].namcrc32 = 1;
#ifndef STANDALONE
			for( w = sec[s].headspri; w >= 0; w = gst->spri[w].sectn )
				if( gst->spri[w].tilnum >= 0 ) gtile[gst->spri[w].tilnum].namcrc32 = 1;
		}
#else
	}
		for( w = gst->numspris - 1; w >= 0; w-- )
			if( (unsigned)gst->spri[w].tilnum < (unsigned)gnumtiles )
				gtile[gst->spri[w].tilnum].namcrc32 = 1;
#endif
		nnumtiles = 0;
		for( i = 0; i < gnumtiles; i++ )
		{
			if( !gtile[i].namcrc32 ) continue;
			j = gtile[nnumtiles].namcrc32; gtile[nnumtiles] = gtile[i]; gtile[nnumtiles].namcrc32 = j; //copy all except namcrc32
			gtile[i].namcrc32 = nnumtiles; nnumtiles++;
		}
		if( nnumtiles != gnumtiles ) { compacttilelist_tilenums(); gnumtiles = nnumtiles; }
}

	if( flags & 3 ) //Reconstruct namcrc32's and hash table from scratch
	{
		memset(gtilehashead, -1, sizeof(gtilehashead));
		for( i = 0; i < gnumtiles; i++ )
		{
			gtile[i].namcrc32 = getcrc32z(0, (unsigned char *)gtile[i].filnam);
			j = (gtile[i].namcrc32 & (sizeof(gtilehashead) / sizeof(gtilehashead[0]) - 1));
			gtile[i].hashnext = gtilehashead[j]; gtilehashead[j] = i;
		}
	}
}


static int loadmap(char *filnam)
{
	// TODO: не забыть что в структурах указатели, у них размер 8, тогда как оригинал ожидает 4. В будущем когда перетащу редактор карт - убрать эти промежуточные преобразования

	Surface *sur;
	Sector *sec;
	Wall *wal;
	Sprite *spr;
	float f, fx, fy;
	int i, j, k, l;
	long x, y, z, fileid, hitile, warned = 0, altsects, nnumtiles, nnumspris;
	short s, cursect;
	char och, tbuf[256];

	if( !kzOpen(filnam) )
	{   //Try without full pathname - see if it's in ZIP/GRP/Mounted_Dir
		for( i = j = 0; filnam[i]; i++ ) if( (filnam[i] == '/') || (filnam[i] == '\\') ) j = i + 1;
		if( !j ) return(0);
		filnam = &filnam[j];
		if( !kzOpen(filnam) ) return(0);
	}
	kzRead(&fileid, 4);
	if( (fileid == 0x04034b50) || (fileid == 0x536e654b) ) //'PK\3\4' is ZIP file id, 'KenS' is GRP file id
	{
		kzclose(); 
		kzaddstack(filnam); 
		return(1);
	}
	sec = gst->sect; gst->light_sprinum = 0;
	if( fileid == 0x3142534b ) //KSB1
	{
		Fatal("не перенес");
		return 0;
	}
	else if( fileid == 0x3242534b ) //KSB2 (current BUILD2 map format)
	{
		kzRead(&gst->startpos, sizeof(gst->startpos));
		kzRead(&gst->startrig, sizeof(gst->startrig));
		kzRead(&gst->startdow, sizeof(gst->startdow));
		kzRead(&gst->startfor, sizeof(gst->startfor));
		for( i = 0/*numplayers - 1*/; i >= 0; i-- )
		{
			gst->p.ipos = gst->startpos;
			gst->p.ifor = gst->startfor;
			gst->p.irig = gst->startrig;
			gst->p.idow = gst->startdow;
			gst->p.cursect = -1;
		}

		//Load sectors
		altsects = 0;
		for( i = 0; i < gst->numsects; i++ )
		{
			if( sec[i].owner < 0 )
			{
				while( sec[i].headspri >= 0 ) delspri(sec[i].headspri);
				if( gst->sect[i].wall ) { free(gst->sect[i].wall); gst->sect[i].wall = 0; }
				continue;
			}
			for( j = sec[i].headspri; j >= 0; j = gst->spri[j].sectn ) gst->spri[j].sect = altsects;
			memcpy(&sec[altsects], &sec[i], sizeof(Sector)); altsects++;
		}
		kzRead(&i, 4); gst->numsects = i + altsects;
		if( gst->numsects > gst->malsects )
		{
			i = gst->malsects; gst->malsects = std::max(gst->numsects + 1, gst->malsects << 1);
			sec = gst->sect = (Sector*)realloc(sec, gst->malsects * sizeof(Sector));
			memset(&sec[i], 0, (gst->malsects - i) * sizeof(Sector));
		}
#if !defined(_WIN64) // error in x64
		kzRead(&sec[altsects], (gst->numsects - altsects) * sizeof(Sector));
#else
		{
			prisect ps;
			kzRead(&ps, (gst->numsects - altsects) * sizeof(prisect));
			CopyToSector(ps, sec[altsects]);
		}
#endif

		//Load walls
		for( i = altsects; i < gst->numsects; i++ )
		{
			sec[i].wall = (Wall*)malloc(sec[i].nmax * sizeof(Wall));
			sec[i].owner = -1;
			for( j = 0; j < sec[i].n; j++ )
			{
#if !defined(_WIN64) // error in x64
				kzRead(&sec[i].wall[j], sizeof(Wall));
#else
				{
					priwall pw;
					kzRead(&pw, sizeof(priwall));
					CopyToWall(pw, sec[i].wall[j]);
				}
#endif
				if( !sec[i].wall[j].n )
				{
					if( !warned )
					{
						warned = 1;
						Fatal("Your map appears to be corrupt");
						for( ; i >= 0; i-- ) free(sec[i].wall);
						gst->numsects = 0;
						return 0;
					}
				}

				sec[i].wall[j].owner = -1;
				if( sec[i].wall[j].surfn > 1 )
				{
					sec[i].wall[j].xsurf = (Surface *)malloc((sec[i].wall[j].surfn - 1) * sizeof(Surface));
					kzRead(sec[i].wall[j].xsurf, (sec[i].wall[j].surfn - 1) * sizeof(Surface));
				}
			}
		}

		//Load tiles
		kzRead(&nnumtiles, sizeof(nnumtiles)); gnumtiles += nnumtiles;
		if( gnumtiles > gmaltiles )
		{
			gmaltiles = std::max(gnumtiles + 1, gmaltiles << 1);
			gtile = (Tile *)realloc(gtile, gmaltiles * sizeof(Tile));
		}
		for( i = gnumtiles - nnumtiles; i < gnumtiles; i++ )
		{
			kzRead(&s, 2); kzRead(gtile[i].filnam, s); gtile[i].filnam[s] = 0; //FIX:possible buffer overflow here
			gtile[i].tt.f = 0;
			gtile[i].namcrc32 = getcrc32z(0, (unsigned char *)gtile[i].filnam);
		}

		//Load sprites
		kzRead(&nnumspris, 4); gst->numspris += nnumspris;
		if( !nnumspris ) for( i = 0; i < gst->numsects; i++ ) { sec[i].headspri = -1; sec[i].owner = -1; } //Hack for loading old format
		if( gst->numspris > gst->malspris )
		{
			i = gst->malspris;
			gst->malspris = std::max(gst->numspris + 1, gst->malspris << 1);
			gst->spri = (Sprite *)realloc(gst->spri, gst->malspris * sizeof(Sprite));
#ifndef STANDALONE
			for( ; i < gst->malspris; i++ ) gst->spri[i].sect = -1;
#endif
		}
		kzRead(&gst->spri[gst->numspris - nnumspris], nnumspris * sizeof(Sprite));
		for( i = gst->numspris - nnumspris; i < gst->numspris; i++ ) gst->spri[i].sect += altsects;


		// | 0 ..       altsects ..  gst->numsects   |
		// |   ^old_sects^    |     ^new_sects^      |
		//
		// |0..gst->numspris-nnumspris..gst->numspris|
		// |  ^old_sprites^   |    ^new_sprites^     |
		//
		// | 0 ..  gnumtiles-nnumtiles .. gnumtiles  |
		// |   ^old_tiles^    |     ^new_tiles^      |

		//Adjust tile indices for new sectors(/walls) & sprites
		for( i = altsects; i < gst->numsects; i++ )
		{
			for( j = 0; j < 2; j++ ) sec[i].surf[j].tilnum += gnumtiles - nnumtiles;
			for( j = 0; j < sec[i].n; j++ ) sec[i].wall[j].surf.tilnum += gnumtiles - nnumtiles;
		}
		for( i = gst->numspris - nnumspris; i < gst->numspris; i++ ) if( gst->spri[i].tilnum >= 0 ) gst->spri[i].tilnum += gnumtiles - nnumtiles;

		//-------------------------------------------------------------------

		//Sprite hacks
		for( i = 0; i < gst->numspris; i++ )
		{
			gst->spri[i].owner = -1;

			//Insert lights
			if( gst->spri[i].flags & (1 << 16) )
			{
				if( gst->light_sprinum < MAXLIGHTS ) gst->light_spri[gst->light_sprinum++] = i;
			}
		}

#ifdef STANDALONE
		for( i = numplayers - 1; i >= 0; i-- ) gst->p[i].sec.n = 0;
#endif
		checknextwalls();
		checksprisect(-1);

#if 0
		//Rebuild hash table from scratch
		memset(gtilehashead, -1, sizeof(gtilehashead));
		for( i = 0; i < gnumtiles; i++ )
		{
			j = (gtile[i].namcrc32 & (sizeof(gtilehashead) / sizeof(gtilehashead[0]) - 1));
			gtile[i].hashnext = gtilehashead[j]; gtilehashead[j] = i;
		}
#else
		compacttilelist(1);
#endif

		kzclose();
		return(1);
	}
	else if( (fileid == 0x00000007) || //Build1 .MAP format 7
		(fileid == 0x00000cbe) )   //Cubes5 .CUB format
	{
		Fatal("не перенес");
		return 0;
	}
	else { return(0); } //MessageBox(ghwnd,"Invalid MAP format",prognam,MB_OK);
}

int LoadMap(const char* filename, int* cursect, glm::vec3* npos, glm::vec3* nrig, glm::vec3* ndow, glm::vec3* nfor)
{
	gst = &sst;
	if( !loadmap((char *)filename) ) return(0);
	npos->x = gst->startpos.x; npos->y = gst->startpos.y; npos->z = gst->startpos.z;
	nrig->x = gst->startrig.x; nrig->y = gst->startrig.y; nrig->z = gst->startrig.z;
	ndow->x = gst->startdow.x; ndow->y = gst->startdow.y; ndow->z = gst->startdow.z;
	nfor->x = gst->startfor.x; nfor->y = gst->startfor.y; nfor->z = gst->startfor.z;
	updatesect(npos->x, npos->y, npos->z, (int *)cursect);
	return(1);
}