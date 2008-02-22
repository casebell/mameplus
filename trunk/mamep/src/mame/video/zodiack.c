/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "includes/espial.h"

UINT8 *zodiack_videoram2;
UINT8 *zodiack_attributesram;
UINT8 *zodiack_bulletsram;
size_t zodiack_bulletsram_size;

static tilemap *bg_tilemap, *fg_tilemap;

WRITE8_HANDLER( zodiack_videoram_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}

WRITE8_HANDLER( zodiack_videoram2_w )
{
	zodiack_videoram2[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
}

WRITE8_HANDLER( zodiack_attributes_w )
{
	if ((offset & 1) && zodiack_attributesram[offset] != data)
	{
		int i;

		for (i = offset / 2;i < videoram_size; i += 32)
		{
			tilemap_mark_tile_dirty(bg_tilemap, i);
			tilemap_mark_tile_dirty(fg_tilemap, i);
		}
	}

	zodiack_attributesram[offset] = data;
}

WRITE8_HANDLER( zodiack_flipscreen_w )
{
	if (flip_screen != (~data & 0x01))
	{
		flip_screen_set(~data & 0x01);
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}
}

PALETTE_INIT( zodiack )
{
	int i;

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x31);

	/* create a lookup table for the palette */
	for (i = 0; i < 0x30; i++)
	{
		int bit0, bit1, bit2;
		int r, g, b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* blue component */
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	/* white for bullets */
	colortable_palette_set_color(machine->colortable, 0x30, RGB_WHITE);

	for (i = 0; i < 0x20; i++)
		if ((i & 3) == 0)
			colortable_entry_set_value(machine->colortable, i, 0);

	for (i = 0; i < 0x10; i += 2)
	{
		colortable_entry_set_value(machine->colortable, 0x20 + i, 32 + (i / 2));
		colortable_entry_set_value(machine->colortable, 0x21 + i, 40 + (i / 2));
	}

	/* bullet */
	colortable_entry_set_value(machine->colortable, 0x30, 0);
	colortable_entry_set_value(machine->colortable, 0x31, 0x30);
}

static TILE_GET_INFO( get_bg_tile_info )
{
	int code = zodiack_videoram2[tile_index];
	int color = (zodiack_attributesram[2 * (tile_index % 32) + 1] >> 4) & 0x07;

	SET_TILE_INFO(0, code, color, 0);
}

static TILE_GET_INFO( get_fg_tile_info )
{
	int code = videoram[tile_index];
	int color = zodiack_attributesram[2 * (tile_index % 32) + 1] & 0x07;

	SET_TILE_INFO(3, code, color, 0);
}

VIDEO_START( zodiack )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	fg_tilemap = tilemap_create(get_fg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(fg_tilemap, 0);
	tilemap_set_scroll_cols(fg_tilemap, 32);

	flip_screen = 0;
}

static void draw_bullets(running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = 0; offs < zodiack_bulletsram_size; offs += 4)
	{
		int x, y;

		x = zodiack_bulletsram[offs + 3] + 7;
		y = 255 - zodiack_bulletsram[offs + 1];

		if (flip_screen && percuss_hardware)
		{
			y = 255 - y;
		}

		drawgfx(
			bitmap,
			machine->gfx[2],
			0,	/* this is just a dot, generated by the hardware */
			0,
			0,0,
			x,y,
			cliprect,TRANSPARENCY_PEN,0);
	}
}

static void draw_sprites(running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = spriteram_size - 4; offs >= 0; offs -= 4)
	{
		int flipx, flipy, sx, sy, spritecode;

		sx = 240 - spriteram[offs + 3];
		sy = 240 - spriteram[offs];
		flipx = !(spriteram[offs + 1] & 0x40);
		flipy = spriteram[offs + 1] & 0x80;
		spritecode = spriteram[offs + 1] & 0x3f;

		if (flip_screen && percuss_hardware)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		drawgfx(bitmap, machine->gfx[1],
			spritecode,
			spriteram[offs + 2] & 0x07,
			flipx, flipy,
			sx, sy,
			//flip_screen[0] ? &spritevisibleareaflipx : &spritevisiblearea,TRANSPARENCY_PEN,0);
			//&spritevisiblearea,TRANSPARENCY_PEN,0);
			cliprect, TRANSPARENCY_PEN, 0);
	}
}

VIDEO_UPDATE( zodiack )
{
	int i;

	for (i = 0; i < 32; i++)
		tilemap_set_scrolly(fg_tilemap, i, zodiack_attributesram[i * 2]);

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);
	draw_bullets(machine, bitmap, cliprect);
	draw_sprites(machine, bitmap, cliprect);
	return 0;
}
