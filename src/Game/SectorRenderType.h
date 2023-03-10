#pragma once

#include "Engine.h"


constexpr const int CharSize = sizeof(char);
constexpr const int ShortSize = sizeof(short);
constexpr const int IntSize = sizeof(int);
constexpr const int LongSize = sizeof(long);
constexpr const int PtrSize = sizeof(void*);

// Map format

#pragma pack(push,1)

typedef struct { float x, y; } point2d;
typedef struct { float x, y, z, u, v; int n; } kgln_t;
typedef struct { float x, y, z; } point3d;
typedef struct { double x, y, z; } dpoint3d;
typedef struct { int f; int p, x, y; } tiletype;

struct tiltyp
{
	long f, p, x, y, z; 
	float shsc; //shsc=suggested height scale
	tiltyp *lowermip;
};

typedef struct { tiltyp c, z; point3d p, r, d, f, h; } cam_t;

struct Tile
{
	char filnam[240]; //Must fit packet header, sector&wall index, null terminator in 256 byte packet
	tiltyp tt; //union! if (!tt.p), it's a 3D model, tt.f points to raw model data, tt.x is type
	long namcrc32, hashnext;
};
extern Tile *gtile;
extern long gnumtiles, gmaltiles, gtilehashead[1024];

struct Surface // текстура/материал
{
	long tilnum, tilanm/*???*/;
	long flags; //Bit0:Blocking, Bit2:RelativeAlignment, Bit5:1Way, Bit16:IsParallax, Bit17:IsSkybox
	long tag;
	point2d uv[3];
	unsigned short asc, rsc, gsc, bsc; //4096 is no change
};
constexpr int SizeSurfaceData = sizeof(Surface);

struct Wall
{
	float x, y;
	long n, ns, nw; //n:rel. wall ind.; ns&nw:nextsect&nextwall_of_sect
	long owner; //for dragging while editing, other effects during game
	long surfn;
	Surface surf;
	Surface* xsurf; //additional malloced surfs when (surfn > 1)
};
constexpr int SizeWallData = sizeof(Wall);

// в оригинале сборка x86, я собираю под x64, где размер указателя больше, поэтому при загрузке временные объекты
struct priwall
{
	float x, y;
	long n, ns, nw; //n:rel. wall ind.; ns&nw:nextsect&nextwall_of_sect
	long owner; //for dragging while editing, other effects during game
	long surfn;
	Surface surf;
	uint32_t error;
};
constexpr int SizepriwallData = sizeof(priwall);

inline void CopyToWall(const priwall& fromWall, Wall& toWall)
{
	toWall.x = fromWall.x;
	toWall.y = fromWall.y;

	toWall.n = fromWall.n;
	toWall.ns = fromWall.ns;
	toWall.nw = fromWall.nw;

	toWall.owner = fromWall.owner;

	toWall.surfn = fromWall.surfn;

	toWall.surf = fromWall.surf;
	toWall.xsurf = nullptr;
}

struct Sprite
{
	point3d p, r, d, f;      //Position, orientation (right, down, forward)
	point3d v, av;           //Position velocity, Angular velocity (direction=axis, magnitude=vel)
	float fat, mas, moi;     //Physics (moi=moment of inertia)
	long tilnum;             //Model file. Ex:"TILES000.ART|64","CARDBOARD.PNG","CACO.KV6","HAND.KCM","IMP.MD3"
	unsigned short asc, rsc, gsc, bsc; //Color scales. 4096 is no change
	long owner, tag;
	long tim, otim;          //Time (in milliseconds) for animation
	long flags;              //Bit0:Blocking, Bit2:1WayOtherSide, Bit5,Bit4:Face/Wall/Floor/.., Bit6:1side, Bit16:IsLight, Bit17-19:SpotAx(1-6), Bit20-29:SpotWid, Bit31:Invisible
	long sect, sectn, sectp; //Current sector / doubly-linked list of indices
};

struct Sector
{
	float minx, miny, maxx, maxy; //bounding box
	float z[2];      //ceil&flor height // TODO: rename to CeilingHeight and FloorHeight
	point2d grad[2]; //ceil&flor grad. grad.x = norm.x/norm.z, grad.y = norm.y/norm.z
	Surface surf[2];  //ceil&flor texture info
	Wall *wall;
	long n, nmax;    //n:numwalls, nmax:walls malloced (nmax >= n)
	long headspri;   //head sprite index (-1 if none)
	long foglev;
	long owner;      //for dragging while editing, other effects during game
};
constexpr int SizeSectorData = sizeof(Sector);

// в оригинале сборка x86, я собираю под x64, где размер указателя больше, поэтому при загрузке временные объекты
struct prisect
{
	float minx, miny, maxx, maxy; //bounding box
	float z[2];      //ceil&flor height
	point2d grad[2]; //ceil&flor grad. grad.x = norm.x/norm.z, grad.y = norm.y/norm.z
	Surface surf[2];  //ceil&flor texture info
	uint32_t error;
	long n, nmax;    //n:numwalls, nmax:walls malloced (nmax >= n)
	long headspri;   //head sprite index (-1 if none)
	long foglev;
	long owner;      //for dragging while editing, other effects during game
};

constexpr int SizeprisectData = sizeof(prisect);

inline void CopyToSector(const prisect& from, Sector& to)
{
	to.minx = from.minx;
	to.miny = from.miny;
	to.maxx = from.maxx;
	to.maxy = from.maxy;

	to.z[0] = from.z[0];
	to.z[1] = from.z[1];

	to.grad[0] = from.grad[0];
	to.grad[1] = from.grad[1];

	to.surf[0] = from.surf[0];
	to.surf[1] = from.surf[1];
	to.wall = nullptr;

	to.n = from.n;
	to.nmax = from.nmax;

	to.headspri = from.headspri;
	to.foglev = from.foglev;
	to.owner = from.owner;
}

#ifdef STANDALONE
#define TYPEMESSNUM 8
#define TYPEMESSLENG 256
#endif

struct PlayerData
{
	//screen/camera state
	int xres, yres;
	float ghx, ghy, ghz, zoom, ozoom;
	point3d ipos, irig, idow, ifor;
	point3d npos, nrig, ndow, nfor; //for 2d/3d swap animation
	point3d grdc, grdu, grdv, grdn; //center,u,v,normal
	float dgridlock, dgridlock3d, compact2d, goalheight2d, height2d;
	int cursect, rendheight, rendinterp, editmode;

#ifdef STANDALONE
	//input state
	float fcmousx, fcmousy;
	int bstatus, obstatus;
	char skeystatus[256 >> 3]; //1 bit per key

	//editing state
	int playerindex, showdebug, showedges3d, grabmode, grabsect, grabwall, grabcf, circnum;
	int gridlock, boolfunc, gotcopy, docollide;
	short selrgbbak[4];
	float selrgbrad, selrgbintens;
	point2d selcorn;
	surf_t copysurf[1]; //array form allows easier expansion later
	spri_t copyspri[1];

	//personal prototype sector
	sect_t sec;
	int startstate, startsect, startwall;

	//typing state
	int typemode, typecurs, typehighlight, typeowritemode;

	char emoticon_hair;  //0=O->, 1=O-+
	char emoticon_eyes;  //0=o_o, 1=o.O, 2=O.o, 3=O.O, |4=wink_right, |8=wink_left
	char emoticon_nose;  //0=-, 1=(none)
	char emoticon_mouth; //0=|, 1=), 2=(, 3=/, 4=P

	int flashlighton;
#endif
};

struct GameState
{
	point3d startpos, startrig, startdow, startfor;
	int numsects, malsects; Sector* sect;

	int numspris, malspris; Sprite* spri;
	int blankheadspri;

#define MAXLIGHTS 256
	int light_spri[MAXLIGHTS], light_sprinum; //List of active light sprite indices

	PlayerData p;

	int rseed;
#ifdef STANDALONE
	//typemess&nick are not in the player structure to make gamestate_crc32 easier to optimize
	char typemess[MAXPLAYERS][TYPEMESSLENG], nick[MAXPLAYERS][64];
	char chatmess[TYPEMESSNUM][TYPEMESSLENG], chatmessowner[TYPEMESSNUM];
	int chatmessn;
#endif
};
extern GameState sst, pst, *gst;

struct vertlist_t { int w, s; };

//Build2 shared global variables:
struct Build2
{
	double gammaval; //1.0=no change, useful range={0.0..~4.0)
	//----------------------- DATA coming from BUILD2.C -----------------------

		//Clipmove hit point info (use this after calling clipmove):
	double clipmaxcr; //clipmove always calls findmaxcr even with no movement
	dpoint3d cliphit[3];
	long cliphitnum, clipsect[3], clipwall[3];

	//----------------------- DATA provided to BUILD2.C -----------------------

	double fattestsprite; //For sprite collision: when to cross sectors
};

extern Build2 build2;

#pragma pack(pop)

void Start();

//Map functions:
int LoadMap(const char* filename, int* cursect, glm::vec3* npos, glm::vec3* nrig, glm::vec3* ndow, glm::vec3* nfor);
void delspri(int i);
long gettileind(const char *filnam);
void checknextwalls();
double getslopez(Sector *s, int i, double x, double y);
void checksprisect(int s); //s: sector of sprites to check; -1 to check all sprites


void updatesect(float x, float y, float z, int *cursect);
