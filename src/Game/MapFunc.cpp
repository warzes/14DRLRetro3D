#include "stdafx.h"
#include "SectorRenderType.h"
#include "crc32.h"

int shadowtest2_updatelighting = 1;

//          -1      i
//headspri     i      j
//               j     -1
void delspri(int i)
{
	Sprite *spr;
	long j;

#ifdef STANDALONE
	if( (unsigned)i >= (unsigned)gst->numspris ) return;
#else
	if( ((unsigned)i >= (unsigned)gst->malspris) || (gst->spri[i].sect < 0) ) return;
#endif
	spr = gst->spri;

	//Delete sprite i
	if( spr[i].sectp < 0 ) gst->sect[spr[i].sect].headspri = spr[i].sectn;
	else spr[spr[i].sectp].sectn = spr[i].sectn;
	if( spr[i].sectn >= 0 ) spr[spr[i].sectn].sectp = spr[i].sectp;

	for( j = gst->light_sprinum - 1; j >= 0; j-- )
		if( gst->light_spri[j] == i ) gst->light_spri[j] = gst->light_spri[--gst->light_sprinum];

	gst->numspris--;
#ifdef STANDALONE
	//Move sprite numspris to i
	if( i == gst->numspris ) return;

	for( j = gst->light_sprinum - 1; j >= 0; j-- )
		if( gst->light_spri[j] == gst->numspris ) gst->light_spri[j] = i;

	spr[i] = spr[gst->numspris];
	if( spr[i].sectp < 0 ) gst->sect[spr[i].sect].headspri = i;
	else spr[spr[i].sectp].sectn = i;
	if( spr[i].sectn >= 0 ) spr[spr[i].sectn].sectp = i;
#else
	//Add sprite i to blankheadspri list
	gst->spri[i].sectn = gst->blankheadspri;
	gst->spri[i].sectp = -1;
	gst->spri[i].sect = -1;
	if( gst->blankheadspri >= 0 ) gst->spri[gst->blankheadspri].sectp = i;
	gst->blankheadspri = i;
#endif
}

long gettileind(const char *st)
{
	long i, crc32, hashind;

	crc32 = getcrc32z(0, (unsigned char *)st); hashind = (crc32 & (sizeof(gtilehashead) / sizeof(gtilehashead[0]) - 1));
	for( i = gtilehashead[hashind]; i >= 0; i = gtile[i].hashnext )
	{
		if( gtile[i].namcrc32 != crc32 ) continue;
		if( !_stricmp(gtile[i].filnam, st) ) return(i);
	}
	if( gnumtiles >= gmaltiles ) { gmaltiles = std::max(gnumtiles + 1, gmaltiles << 1); gtile = (Tile*)realloc(gtile, gmaltiles * sizeof(Tile)); }
	strcpy(gtile[gnumtiles].filnam, st);

	gtile[gnumtiles].namcrc32 = crc32;
	gtile[gnumtiles].hashnext = gtilehashead[hashind]; gtilehashead[hashind] = gnumtiles;
	gtile[gnumtiles].tt.f = 0;
	gnumtiles++;
	return(gnumtiles - 1);
}

int arewallstouching(int s0, int w0, int s1, int w1)
{
	Sector *sec;
	float x[4], y[4];
	int i;

	sec = gst->sect;

	x[0] = sec[s0].wall[w0].x; y[0] = sec[s0].wall[w0].y; i = sec[s0].wall[w0].n + w0;
	x[1] = sec[s0].wall[i].x; y[1] = sec[s0].wall[i].y;
	x[2] = sec[s1].wall[w1].x; y[2] = sec[s1].wall[w1].y; i = sec[s1].wall[w1].n + w1;
	x[3] = sec[s1].wall[i].x; y[3] = sec[s1].wall[i].y;

	//Make sure x's & y's match (front or back)
	if( (x[0] == x[2]) && (y[0] == y[2]) ) { if( (x[1] != x[3]) || (y[1] != y[3]) ) return(0); }
	else { if( (x[0] != x[3]) || (y[0] != y[3]) || (x[1] != x[2]) || (y[1] != y[2]) ) return(0); }

	//Connect walls only if their z's cross
	for( i = 1; i >= 0; i-- )
		if( std::max(getslopez(&sec[s0], 0, x[i], y[i]), getslopez(&sec[s1], 0, x[i], y[i])) <=
			std::min(getslopez(&sec[s0], 1, x[i], y[i]), getslopez(&sec[s1], 1, x[i], y[i])) ) return(1);

	return(0);
}

void checknextwalls()
{
#if 0
	sect_t *sec;
	float f, x0, y0, x1, y1;
	int s0, w0, w0n, s1, w1, w1n;

	sec = gst->sect;

	//Clear all nextsect/nextwalls
	for( s0 = 0; s0 < gst->numsects; s0++ )
		for( w0 = 0; w0 < sec[s0].n; w0++ ) sec[s0].wall[w0].ns = sec[s0].wall[w0].nw = -1;

	for( s1 = 1; s1 < gst->numsects; s1++ )
		for( w1 = 0; w1 < sec[s1].n; w1++ )
		{
			x0 = sec[s1].wall[w1].x;  y0 = sec[s1].wall[w1].y; w1n = sec[s1].wall[w1].n + w1;
			x1 = sec[s1].wall[w1n].x; y1 = sec[s1].wall[w1n].y;
			for( s0 = 0; s0 < s1; s0++ )
				for( w0 = 0; w0 < sec[s0].n; w0++ )
					if( (sec[s0].wall[w0].x == x1) && (sec[s0].wall[w0].y == y1) )
					{
						w0n = sec[s0].wall[w0].n + w0;
						if( (sec[s0].wall[w0n].x == x0) && (sec[s0].wall[w0n].y == y0) )
						{
							sec[s1].wall[w1].ns = s0; //FIX: obsolete: doesn't support SOS
							sec[s1].wall[w1].nw = w0;
							sec[s0].wall[w0].ns = s1;
							sec[s0].wall[w0].nw = w1;
							goto cnw_break2;
						}
					}
		cnw_break2:;
		}
#else
	typedef struct { int w, s; float minpt; } cvertlist_t;
	cvertlist_t *hashead, *hashlist, *subhashlist, vertemp;
	int *hashsiz;
	Sector *sec;
	float f; //WARNING: keep f float for hash trick!
	float x0, y0, x1, y1, fz[8];
	int i, j, k, m, n, r, w, s0, w0, w0n, s1, w1, w1n, s2, w2, lhsiz, hsiz, numwalls, maxchainleng;
	int gap, z, zz, subn;

	sec = gst->sect;

	for( s0 = 0, numwalls = 0; s0 < gst->numsects; s0++ ) numwalls += sec[s0].n;

	for( lhsiz = 4, hsiz = (1 << lhsiz); (hsiz << 1) < numwalls; lhsiz++, hsiz <<= 1 ); //hsiz = 0.5x to 1.0x of numwalls
	hashead = (cvertlist_t *)_alloca(hsiz * sizeof(hashead[0])); memset(hashead, -1, hsiz * sizeof(hashead[0]));
	hashsiz = (int         *)_alloca(hsiz * sizeof(hashsiz[0])); memset(hashsiz, 0, hsiz * sizeof(hashsiz[0]));

	maxchainleng = 0;
	for( s0 = 0; s0 < gst->numsects; s0++ )
		for( w0 = 0; w0 < sec[s0].n; w0++ )
		{
			i = 0; w0n = sec[s0].wall[w0].n + w0;
			//Hash must give same values if w0 and w0n are swapped (commutativity)
			f = sec[s0].wall[w0].x * sec[s0].wall[w0n].x + sec[s0].wall[w0].y * sec[s0].wall[w0n].y;
			k = *(long *)&f;
			//k ^= (*(long *)&sec[s0].wall[w0].x) ^ (*(long *)&sec[s0].wall[w0n].x);
			//k ^= (*(long *)&sec[s0].wall[w0].y) ^ (*(long *)&sec[s0].wall[w0n].y);
			for( j = lhsiz; j < 32; j += lhsiz ) i -= (k >> j);
			i &= (hsiz - 1);

			sec[s0].wall[w0].ns = hashead[i].s; hashead[i].s = s0;
			sec[s0].wall[w0].nw = hashead[i].w; hashead[i].w = w0;
			hashsiz[i]++; if( hashsiz[i] > maxchainleng ) maxchainleng = hashsiz[i];
		}

	//hashhead -> s0w0 -> s1w1 -> s2w2 -> s3w3 -> s4w4 -> -1
	//              A       B       A               B

	hashlist = (cvertlist_t *)_alloca(maxchainleng * sizeof(hashlist[0]));

	//printf("maxchainleng=%d\n",maxchainleng); //FIX

	for( i = 0; i < hsiz; i++ )
	{
		n = 0;
		s0 = hashead[i].s;
		w0 = hashead[i].w;
		while( s0 >= 0 )
		{
			hashlist[n].s = s0;
			hashlist[n].w = w0;

			//for 2nd-level hash!
			w0n = sec[s0].wall[w0].n + w0; x0 = sec[s0].wall[w0].x; x1 = sec[s0].wall[w0n].x;
			if( x0 != x1 ) hashlist[n].minpt = std::min(x0, x1);
			else hashlist[n].minpt = std::min(sec[s0].wall[w0].y, sec[s0].wall[w0n].y);
			n++;

			//Easier to join chains if inited as pointing to self rather than -1
			s1 = sec[s0].wall[w0].ns; sec[s0].wall[w0].ns = s0;
			w1 = sec[s0].wall[w0].nw; sec[s0].wall[w0].nw = w0;
			s0 = s1; w0 = w1;
		}

		if( n >= 2 )
		{
			//Sort points by y's
			for( gap = (n >> 1); gap; gap >>= 1 )
				for( z = 0; z < n - gap; z++ )
					for( zz = z; zz >= 0; zz -= gap )
					{
						if( hashlist[zz].minpt <= hashlist[zz + gap].minpt ) break;
						vertemp = hashlist[zz]; hashlist[zz] = hashlist[zz + gap]; hashlist[zz + gap] = vertemp;
					}

			//printf("//n=%d\n",n); //FIX

			for( zz = n, z = n - 1; z >= 0; z-- )
			{
				if( (z) && (hashlist[z - 1].minpt == hashlist[z].minpt) ) continue;
				subhashlist = &hashlist[z]; subn = zz - z; zz = z;

				//Example: (sector walls overlapping, drawn sideways)
				//   AAA EEE DDD
				//     BBB CCC
				//                                  0    1    2    3    4
				//j=?,w=4,r=5,n=5, s0:?, s1:?, {A->A,B->B,C->C,D->D,E->E}
				//j=?,w=4,r=4,n=5, s0:E, s1:?, {A->A,B->B,C->C,D->D,E->E}
				//j=3,w=4,r=4,n=5, s0:E, s1:D, {A->A,B->B,C->C,D->D,E->E}
				//j=2,w=3,r=4,n=5, s0:E, s1:C, {A->A,B->B,D->D|C->E,E->C}
				//j=1,w=2,r=4,n=5, s0:E, s1:B, {A->A,D->D|B->C,C->E,E->B}
				//j=0,w=2,r=4,n=5, s0:E, s1:A, {A->A,D->D|B->C,C->E,E->B}
				//j=1,w=1,r=3,n=5, s0:C, s1:D, {A->A|D->E,B->C,C->D,E->B}
				//j=0,w=1,r=3,n=5, s0:C, s1:A, {A->A|D->E,B->C,C->D,E->B}
				//j=0,w=0,r=2,n=5, s0:B, s1:A, {A->C,D->E,B->A,C->D,E->B}

				//     s0    s1  s3  s2
				//  +-------+---+---+---+
				//  |   1   | 7 | D | 8 |
				//  |0     2|6 4|C E|B 9|
				//  |   3   | 5 | F | A |
				//  +-------+---+---+---+
				//                           s0,w0  s1,w1
				//cmp: i=5 ,j=1,w=2,r=2,n=3   0,1,   2,3  no
				//cmp: i=5 ,j=0,w=2,r=2,n=3   0,1,   3,2  no
				//cmp: i=5 ,j=0,w=1,r=1,n=3   2,3,   3,2  yes

				//cmp: i=6 ,j=0,w=1,r=1,n=2   0,0,   3,1  no

				//cmp: i=8 ,j=1,w=2,r=2,n=3   0,2,   1,2  yes
				//cmp: i=8 ,j=0,w=1,r=2,n=3   0,2,   2,2  no
				//cmp: i=8 ,j=0,w=1,r=1,n=3   1,2,   2,2  no

				//cmp: i=10,j=0,w=1,r=1,n=2   1,0,   3,0  yes

				//Graph search and connect: fifo is hashlist itself
				//      (write) (read) (total)
				//  0      w      r      n
				//   (left) (fifo) (done)
			//printf("//i=%d:n=%d\n",i,subn); //FIX
				//FIX
			//for(j=0;j<subn;j++)
			//{
			//   s0 = subhashlist[j].s;
			//   w0 = subhashlist[j].w;
			//   x0 = sec[s0].wall[w0].x; x1 = sec[s0].wall[w0n].x; w0n = sec[s0].wall[w0].n+w0;
			//   y0 = sec[s0].wall[w0].y; y1 = sec[s0].wall[w0n].y;
			//   printf("   %2d: %6.1f %6.1f %6.1f %6.1f | %6.1f\n",j,x0,y0,x1,y1,subhashlist[j].minpt);
			//}

				w = subn - 1; r = subn;
				while( w > 0 )
				{
					r--;
					s0 = subhashlist[r].s;
					w0 = subhashlist[r].w;
					for( j = w - 1; j >= 0; j-- )
					{
						s1 = subhashlist[j].s; if( s0 == s1 ) continue; //Don't allow 2-vertex loops to become red lines
						w1 = subhashlist[j].w;
						//printf("//   cmp: j=%2d,w=%2d,r=%2d, %3d,%3d, %3d,%3d, ",j,w,r,s0,w0,s1,w1); //FIX
						if( !arewallstouching(s0, w0, s1, w1) ) { /*printf("no\n");FIX*/ continue; }
						//printf("yes\n"); //FIX

						s2 = sec[s0].wall[w0].ns;
						w2 = sec[s0].wall[w0].nw;
						sec[s0].wall[w0].ns = sec[s1].wall[w1].ns;
						sec[s0].wall[w0].nw = sec[s1].wall[w1].nw;
						sec[s1].wall[w1].ns = s2;
						sec[s1].wall[w1].nw = w2;
						w--; if( w == j ) continue;
						vertemp = subhashlist[w];
						subhashlist[w] = subhashlist[j];
						subhashlist[j] = vertemp;
					}
					if( r == w ) w--;
				}
			}
		}

		//convert disjoint walls (self-linked) back to -1's
		for( j = n - 1; j >= 0; j-- )
		{
			s0 = hashlist[j].s; w0 = hashlist[j].w;
			if( (sec[s0].wall[w0].ns == s0) && (sec[s0].wall[w0].nw == w0) )
			{
				sec[s0].wall[w0].ns = sec[s0].wall[w0].nw = -1;
			}
		}
	}
#endif
	shadowtest2_updatelighting = 1;
}

double getslopez(Sector *s, int i, double x, double y)
{
	Wall *wal = s->wall;
	return((wal[0].x - x) * s->grad[i].x + (wal[0].y - y) * s->grad[i].y + s->z[i]);
}

//s: sector of sprites to check
	//Pass -1 to check and compact all valid sprites
void checksprisect(int s)
{
	Sector *sec;
	Sprite *spr;
	int w, ns, nw, s0, s1;

	sec = gst->sect;
	spr = gst->spri;
#if 0
	//FIXFIX:Warning: this block has not been tested!
	if( (unsigned)s < (unsigned)gst->numsects )
	{
		for( w = sec[s].headspri; w >= 0; w = nw )
		{
			nw = spr[w].sectn;
			ns = spr[w].sect; updatesect(spr[w].p.x, spr[w].p.y, spr[w].p.z, &ns);
			if( ns != spr[w].sect ) changesprisect(w, ns);
		}
		return;
	}
#endif
	for( s = gst->numsects - 1; s >= 0; s-- ) sec[s].headspri = -1;
	for( w = nw = 0; w < gst->numspris; w++ )
	{
		ns = spr[w].sect; if( ns < 0 ) continue;
		updatesect(spr[w].p.x, spr[w].p.y, spr[w].p.z, &ns);
		if( (unsigned)ns >= (unsigned)gst->numsects ) ns = spr[w].sect;

		spr[nw] = spr[w];
		spr[nw].sect = ns;
		spr[nw].sectn = sec[ns].headspri;
		spr[nw].sectp = -1;
		if( sec[ns].headspri >= 0 ) spr[sec[ns].headspri].sectp = nw;
		sec[ns].headspri = nw;
		nw++;
	}
#ifndef STANDALONE
	gst->blankheadspri = -1;
	for( ; nw < gst->malspris; nw++ )
	{
		gst->spri[nw].sectn = gst->blankheadspri;
		gst->spri[nw].sectp = -1;
		gst->spri[nw].sect = -1;
		if( gst->blankheadspri >= 0 ) gst->spri[gst->blankheadspri].sectp = nw;
		gst->blankheadspri = nw;
	}
#endif
}


//Pass z as >1e30 to make updatesect ignore height return first sector containing (x,y)
void updatesect(float x, float y, float z, int *cursect)
{
	Sector *sec;
	long *gotsect;
	int i, s, w, ns, nw, allsec, cnt, *secfif, secfifw, secfifr;

	sec = gst->sect;
	s = (*cursect);
	//if( (unsigned)s >= (unsigned)gst->numsects ) //reference invalid; brute force search
	//{
	//	for( s = gst->numsects - 1; s >= 0; s-- )
	//		if( insidesect(x, y, sec[s].wall, sec[s].n) )
	//			if( (z > 1e30) || ((z >= getslopez(&sec[s], 0, x, y)) && (z <= getslopez(&sec[s], 1, x, y))) ) break;
	//	(*cursect) = s; return;
	//}

	//if( insidesect(x, y, sec[s].wall, sec[s].n) )
	//	if( (z > 1e30) || ((z >= getslopez(&sec[s], 0, x, y)) && (z <= getslopez(&sec[s], 1, x, y))) ) return;

	//w = (((gst->numsects + 31) >> 5) << 2);
	//gotsect = (long *)_alloca(w);
	//memset(gotsect, 0, w); gotsect[s >> 5] |= (1 << s);
	//secfif = (int *)_alloca(gst->numsects * sizeof(secfif[0]));
	//secfifw = secfifr = 0;

	//(*cursect) = -1; allsec = gst->numsects - 1;
	//for( cnt = gst->numsects - 1; cnt > 0; cnt-- )
	//{
	//	for( w = sec[s].n - 1; w >= 0; w-- )
	//	{
	//		ns = sec[s].wall[w].ns;
	//		nw = sec[s].wall[w].nw;
	//		while( ((unsigned)ns < (unsigned)gst->numsects) && (ns != s) )
	//		{
	//			if( !(gotsect[ns >> 5] & (1 << ns)) )
	//			{
	//				gotsect[ns >> 5] |= (1 << ns);
	//				secfif[secfifw] = ns; secfifw++;
	//			}
	//			i = ns;
	//			ns = sec[i].wall[nw].ns;
	//			nw = sec[i].wall[nw].nw;
	//		}
	//	}

	//	if( secfifr < secfifw )
	//	{
	//		s = secfif[secfifr]; secfifr++;
	//	} //breadth-first
	//	else
	//	{     //fifo was empty.. must be some disjoint sectors
	//		while( (allsec >= 0) && (gotsect[allsec >> 5] & (1 << allsec)) ) allsec--;
	//		s = allsec; if( s < 0 ) break;
	//		gotsect[s >> 5] |= (1 << s);
	//	}

	//	if( insidesect(x, y, sec[s].wall, sec[s].n) )
	//		if( (z > 1e30) || ((z >= getslopez(&sec[s], 0, x, y)) && (z <= getslopez(&sec[s], 1, x, y))) )
	//		{
	//			(*cursect) = s; return;
	//		}
	//}
}
