/***************************************************************************
 Domino Block

 Driver by Tomasz Slanina
 some bits by David Haywood
 Fixed Dip Switches and additional infos by Stephh

 Based on the Arkanoid driver

 This is a hacked up version of Arkanoid with high colour backgrounds
 and gameplay modifications.  It runs on custom Korean hardware.

 Button 1 = 'use Domino' - The ball will bounce along the bricks in a
                           horizontal line without coming down
                           until a "hole" or a grey or gold brick

 Button 2 = 'use Rocket' - Your paddle will jump to the top of the screen
                           then back down, destroying everything in its path

 Bonus Lives always at 200000, 500000, then every 300000 (no Dip Switch)

 There are 6 stages of 5 rounds. When these 30 levels are completed,
 you'll have to complete round 1 of each stage with a smaller bat.
 When this stage 7 is completed, the game ends but you can't enter
 your initials if you have achieved a high score !

 It's funny to see that this game, as 'arkanoid', does NOT allow you
 to enter "SEX" as initials (which will be replaced by "H !") ;)

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"

VIDEO_START( dominob )
{
	machine->gfx[0]->color_granularity=8;
}

static UINT8 *bgram;

static void draw_sprites(running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = 0;offs < spriteram_size;offs += 4)
	{
		int sx,sy,code;

		sx = spriteram[offs];
		sy = 248 - spriteram[offs + 1];
		if (flip_screen_x) sx = 248 - sx;
		if (flip_screen_y) sy = 248 - sy;

		code = spriteram[offs + 3] + ((spriteram[offs + 2] & 0x03) << 8)  ;

		drawgfx(bitmap,machine->gfx[0],
				2 * code,
				((spriteram[offs + 2] & 0xf8) >> 3)  ,
				flip_screen_x,flip_screen_y,
				sx,sy + (flip_screen_y ? 8 : -8),
				cliprect,TRANSPARENCY_PEN,0);
		drawgfx(bitmap,machine->gfx[0],
				2 * code + 1,
				((spriteram[offs + 2] & 0xf8) >> 3)  ,
				flip_screen_x,flip_screen_y,
				sx,sy,
				cliprect,TRANSPARENCY_PEN,0);
	}
}


VIDEO_UPDATE( dominob )
{
	/* Convert to tilemaps? */
	{
		int x,y;
		int index=0;
		for(y=0;y<256/32;y++)
	 		for(x=0;x<256/32;x++)
	 		{
	 			drawgfx(	bitmap,
									machine->gfx[1],
									bgram[index]+256*(bgram[index+1]&0xf),
						bgram[index+1]>>4,
						0, 0,
						x*32,y*32,
						cliprect,
						TRANSPARENCY_NONE,0);
				index+=2;
	 		}
	}

	{
		int x,y;
		for(y=0;y<32;y++)
	 		for(x=0;x<32;x++)
	 		{
	 			drawgfx(	bitmap,
						machine->gfx[0],
						videoram[(y*32+x)*2+1]+(videoram[(y*32+x)*2]&7)*256,
						(videoram[(y*32+x)*2]>>3),
						0, 0,
						x*8,y*8,
						cliprect,
						TRANSPARENCY_PEN,0);
	 		}
	}

	draw_sprites(machine, bitmap, cliprect);

	return 0;
}


//UINT8 dominob_paddle_select;
//UINT8 dominob_paddle_value;

WRITE8_HANDLER( dominob_d008_w )
{
	/* is there a purpose on this ? always set to 0x00 (read from 0xc47b in RAM) */
}

READ8_HANDLER( dominob_input_2_r )
{
	return input_port_2_r(offset);
}


static ADDRESS_MAP_START( memmap, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READWRITE(MRA8_ROM, MWA8_NOP) // there are some garbage writes to ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM

	AM_RANGE(0xd000, 0xd000) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0xd001, 0xd001) AM_READWRITE(AY8910_read_port_0_r, AY8910_write_port_0_w)
	AM_RANGE(0xd008, 0xd008) AM_WRITE(dominob_d008_w)
	AM_RANGE(0xd00c, 0xd00c) AM_READ(input_port_0_r)
	AM_RANGE(0xd010, 0xd010) AM_READWRITE(input_port_1_r, MWA8_NOP)
	AM_RANGE(0xd018, 0xd018) AM_READ(dominob_input_2_r) AM_WRITENOP

	AM_RANGE(0xe000, 0xe7ff) AM_RAM AM_BASE(&videoram)
	AM_RANGE(0xe800, 0xe83f) AM_RAM AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0xe840, 0xefff) AM_RAM
	AM_RANGE(0xf000, 0xf07f) AM_RAM AM_BASE(&bgram)
	AM_RANGE(0xf080, 0xf7ff) AM_RAM
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(paletteram_xxxxRRRRGGGGBBBB_le_w) AM_READ(MRA8_RAM) AM_BASE(&paletteram)
	AM_RANGE(0xfc00, 0xffff) AM_RAM
ADDRESS_MAP_END

/* I don't know if this has a purpose - also read in 'arkatayt' but not handled */
READ8_HANDLER( dominob_unk_port02_r )
{
	return 0xff;//mame_rand(Machine);
}

static ADDRESS_MAP_START( portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_FLAGS( AMEF_ABITS(8) )
	AM_RANGE(0x02, 0x02) AM_READ(dominob_unk_port02_r)
ADDRESS_MAP_END


INPUT_PORTS_START( dominob )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )             /* works (subs 2 credits), but starts a 1 player game as START1 */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )              /* SERVICE1 in 'arkanoid' */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )            /* TILT in 'arkanoid' */
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )           /* COIN1 in 'arkanoid' */
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )           /* COIN2 in 'arkanoid' */
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_SPECIAL )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )            /* also works in "demo mode" ! */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )            /* also works in "demo mode" ! */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )            /* player 2 BUTTON1 in 'arkanoid' - only read to select the girl */
	PORT_BIT( 0xf8, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("IN2")      /* Spinner Player 1 */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15)

	PORT_START_TAG("IN3")      /* Spinner Player 2 */ /* No Player 2 */
//  PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15) PORT_COCKTAIL

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x06, 0x06, "Difficulty 1" )              /* code at 0x1a82 - table at 0x1aa4 (4 * 8 bytes) */
	PORT_DIPSETTING(    0x06, DEF_STR( Easy ) )             /* starting speed = 0x02/0x03/0x03/0x03/0x03 - max speed = 0x09 */
	PORT_DIPSETTING(    0x04, DEF_STR( Medium ) )           /* starting speed = 0x03/0x04/0x04/0x04/0x04 - max speed = 0x09 */
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )             /* starting speed = 0x03/0x04/0x05/0x05/0x05 - max speed = 0x0b */
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )          /* starting speed = 0x04/0x05/0x06/0x06/0x06 - max speed = 0x0b */
	PORT_DIPNAME( 0x18, 0x18, "Difficulty 2" )              /* code at 0x03b5 - table at 0x040c (4 * 16 bytes) */
	PORT_DIPSETTING(    0x18, DEF_STR( Easy ) )             /* increase speed when 0x30/0x30/0x30/0x40/0x40/0x50/0x50/0x50/0x50/0x80 - when max speed, speed = 0x06 */
	PORT_DIPSETTING(    0x10, DEF_STR( Medium ) )           /* increase speed when 0x30/0x30/0x30/0x30/0x40/0x40/0x40/0x40/0x40/0x90 - when max speed, speed = 0x07 */
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )             /* increase speed when 0x30/0x30/0x20/0x20/0x30/0x30/0x30/0x40/0x40/0xa0 - when max speed, speed = 0x08 */
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )          /* increase speed when 0x20/0x20/0x20/0x20/0x20/0x30/0x30/0x30/0x40/0xc0 - when max speed, speed = 0x09 */
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x60, "1" )
	PORT_DIPSETTING(    0x40, "3" )
	PORT_DIPSETTING(    0x20, "5" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x80, 0x80, "Striptease" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3)},
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout bglayout =
{
	32,32,
	RGN_FRAC(1,4),
	4,
	{
		RGN_FRAC(3,4),
		RGN_FRAC(2,4),
		RGN_FRAC(1,4),
		RGN_FRAC(0,4) },
	{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 ,8*32,9*32,10*32,11*32,12*32,13*32,14*32,15*32,
		16*32,17*32,18*32,19*32,20*32,21*32,22*32,23*32,24*32,25*32,26*32,27*32,28*32,29*32,30*32,31*32},
	16*32*2
};

static GFXDECODE_START( dominob )
	GFXDECODE_ENTRY(REGION_GFX1, 0, charlayout,   0, 0x20)
	GFXDECODE_ENTRY(REGION_GFX2, 0, bglayout,     0x100, 0x10)
GFXDECODE_END


static struct AY8910interface ay8910_interface =
{
	input_port_5_r,
	input_port_4_r
};


static MACHINE_DRIVER_START( dominob )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,8000000/2)
	MDRV_CPU_PROGRAM_MAP(memmap, 0)
	MDRV_CPU_IO_MAP(portmap,0)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 32*8-1)
	MDRV_GFXDECODE(dominob)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_START(dominob)
	MDRV_VIDEO_UPDATE(dominob)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD(AY8910, 8000000/4 /* guess */)
	MDRV_SOUND_CONFIG(ay8910_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( dominob )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "u81",   0x0000, 0x10000, CRC(709b7a29) SHA1(7c95cbaf669a0885101a48e937868c245f87567e) )

	ROM_REGION( 0x18000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "u33",   0x00000, 0x8000, CRC(359c98de) SHA1(5c96dfb538c6b25530582f8c2a0cb20d85c39f68) )
	ROM_LOAD( "u34",   0x08000, 0x8000, CRC(0031f713) SHA1(9341f84081e2d8954e476236e93e49b4d8819b8f) )
	ROM_LOAD( "u35",   0x10000, 0x8000, CRC(6eb87657) SHA1(40ff9d6f21ade48b16f0cefea08a9364a4ee9144) )

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "u111",   0x00000, 0x40000, CRC(b835be84) SHA1(bbb744a28df00017f81d6eac12b00b5f3aca3a8b) )
	ROM_CONTINUE(0x00000,0x40000) // 1ST AND 2ND HALF IDENTICAL
	ROM_LOAD( "u112",   0x40000, 0x40000, CRC(60d7bfd7) SHA1(9f6475ce717e3d5a42aaaacc3ec340e74b7e40b4) )
	ROM_CONTINUE(0x40000,0x40000) // 1ST AND 2ND HALF IDENTICAL
	ROM_LOAD( "u113",   0x80000, 0x40000, CRC(3c999d1e) SHA1(e10c61d7868d9c902e337c9947506761878e31a5) )
	ROM_CONTINUE(0x80000,0x40000) // 1ST AND 2ND HALF IDENTICAL
	ROM_LOAD( "u114",   0xc0000, 0x40000, CRC(2364124e) SHA1(55dc6c46f54655c574a96f5c920e4e1f2e05fd5d) )
	ROM_CONTINUE(0xc0000,0x40000) // 1ST AND 2ND HALF IDENTICAL
ROM_END

GAME( 1990, dominob,  0,       dominob,  dominob,  0, ROT0, "Wonwoo Systems", "Domino Block", 0 )
