#include <stddef.h>
#include <stdint.h>
#include <psxgpu.h>
#include <stdio.h>
#include <psxcd.h>
#include <stdlib.h>

#define SCREEN_XRES	640
#define SCREEN_YRES	480

#define OT_LEN 		8

DISPENV disp;
DRAWENV draw;

char		pribuff[2][65536];
uint32_t	ot[2][OT_LEN];
char*		nextpri;
int			db = 0;

uint32_t*	beeroux = NULL;

static void	init(void);
static uint32_t*	load_tex(TIM_IMAGE* img, const char* path);

int main(int argc, const char **argv) {
	TIM_IMAGE	img;
	POLY_FT4*	sprite;
	
	init();
	
	beeroux = load_tex(&img, "\\ASSETS\\BEEROUX.TIM;1");

	if (img.mode & 0x8)
		LoadImage(img.crect, img.caddr);
	LoadImage(img.prect, img.paddr);

	uint16_t	tex_tpage = getTPage(img.mode, 1, img.prect->x, img.prect->y);
	uint16_t	tex_clut = getClut(img.crect->x, img.crect->y);

	for (;;) {
		ClearOTagR( ot[db], OT_LEN );
		nextpri = pribuff[db];

		sprite = (POLY_FT4*) nextpri;

		setPolyFT4(sprite);
		setXYWH(sprite, 0, 0, 64, 36);
		setUVWH(sprite, 0, 0, 64, 36);
		setRGB0(sprite, 127, 127, 127);

		sprite->tpage = tex_tpage;
		sprite->clut = tex_clut;
		
		addPrim(ot[db]+(OT_LEN-1), sprite);
		++nextpri;

		DrawSync( 0 );
		VSync( 0 );

		PutDrawEnv( &draw );

		SetDispMask( 1 );

		DrawOTag( ot[db]+(OT_LEN-1) );

		db = !db;
	}

	return 0;
}

static void	init(void) {
	printf("Init GPU... ");
	ResetGraph(0);
	printf("Done.\n");

	printf("Set video mode... ");

	SetDefDispEnv( &disp, 0, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &draw, 0, 0, SCREEN_XRES, SCREEN_YRES );
	disp.isinter = 1;

	setRGB0( &draw, 63, 0, 127 );
	draw.isbg = 1;
	draw.dtd = 1;

	PutDispEnv( &disp );
	PutDrawEnv( &draw );

	SetDispMask( 1 );

	printf("Done.\n");

	printf("Init CdRom... ");
	CdInit();
	printf("Done.\n");
}

static uint32_t*	load_tex(TIM_IMAGE* img, const char* path) {
	CdlFILE		loc;
	uint32_t*	buffer;

	printf("loading %s\n", path);

	if (CdSearchFile(&loc, path) == NULL) {
		return NULL;
	}

	buffer = malloc(loc.size);

	CdControl(CdlSetloc, &loc, NULL);

	CdRead(loc.size / 2048, buffer, CdlModeIgnore);

	GetTimInfo(buffer, img);

	if (img->mode & 0x8)
		LoadImage(img->crect, img->caddr);
	LoadImage(img->prect, img->paddr);

	printf("Done\n");

	return buffer;
}
