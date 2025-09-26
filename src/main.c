/*
 * PSn00bSDK basic graphics example
 * (C) 2020-2023 Lameguy64, spicyjpeg - MPL licensed
 *
 * A comprehensive "advanced hello world" example showing how to set up the
 * screen with double buffering, draw basic graphics (a bouncing square) and use
 * PSn00bSDK's debug font API to quickly print some text, all while following
 * best practices. This is not necessarily the simplest hello world example and
 * may look daunting at first glance, but it is a good starting point for more
 * complex programs.
 *
 * In order to avoid cluttering the program with global variables (as many Sony
 * SDK examples and other PSn00bSDK examples written before this one do) two
 * custom structures are employed:
 *
 * - a RenderBuffer structure containing the DISPENV and DRAWENV objects that
 *   represent the location of the framebuffer in VRAM, as well as the ordering
 *   table (OT) used to sort GPU commands/primitives by their Z index and the
 *   actual buffer commands will be written to;
 * - a RenderContext structure holding two RenderBuffer instances plus some
 *   variables to keep track of which buffer is currently being drawn and how
 *   much of its primitive buffer has been filled up so far.
 *
 * A C++ version of this example is also available (see examples/hellocpp).
 */

#include <stddef.h>
#include <stdint.h>
#include <psxgpu.h>
#include <stdio.h>

#define SCREEN_XRES	640
#define SCREEN_YRES	480

#define OT_LEN 		8

DISPENV disp;
DRAWENV draw;

char		pribuff[2][65536];		/* Primitive packet buffers */
uint32_t	ot[2][OT_LEN];			/* Ordering tables */
char		*nextpri;				/* Pointer to next packet buffer offset */
int			db = 0;					/* Double buffer index */

extern const uint32_t	beeroux[];

static void	init(void);

int main(int argc, const char **argv) {
	printf("Hello world!\n");
	TIM_IMAGE	img;
	POLY_FT4*	sprite;

	init();

	GetTimInfo(beeroux, &img);

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
	ResetGraph( 0 );
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
}
