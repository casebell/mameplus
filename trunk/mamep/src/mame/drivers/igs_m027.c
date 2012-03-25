/* IGS ARM7 (IGS027A) based Mahjong / Gambling platform(s)
 Driver by Xing Xing

 These games use the IGS027A processor.

 This is an ARM7 with Internal ROM. (Also used on later PGM games)

 In some cases the first part of the Internal ROM is excute only, and
 cannot be read out with a trojan.  It hasn't been confirmed if these
 games make use of that feature.

 To emulate these games the Internal ROM will need dumping
 There are at least 20 other games on this and similar platforms.

*/

#include "emu.h"
#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "machine/nvram.h"


class igs_m027_state : public driver_device
{
public:
	igs_m027_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	UINT32 *m_igs_mainram;
	UINT32 *m_igs_cg_videoram;
	UINT32 *m_igs_tx_videoram;
	UINT32 *m_igs_bg_videoram;
	UINT32 *m_igs_palette32;
	tilemap_t *m_igs_tx_tilemap;
	tilemap_t *m_igs_bg_tilemap;
};



/***************************************************************************

    Video

    0x38001000, 0x380017ff          CG_CONTROL,8 byte per object, 0x100 in total
    0x38001800, 0x380019ff      PALETTE RAM,2 byte per color, 0x100 in total
    0x38004000, 0x38005FFF      TX Video RAM????????1E00??????512x240??????
    0x38006000, 0x38007FFF      BG Video RAM????????1E00??????512x240??????


***************************************************************************/



/* CGLayer */
static WRITE32_HANDLER( igs_cg_videoram_w )
{
	igs_m027_state *state = space->machine().driver_data<igs_m027_state>();
	COMBINE_DATA(&state->m_igs_cg_videoram[offset]);
	//if(data!=0)
	logerror("PC(%08X) CG @%x = %x!\n",cpu_get_pc(&space->device()),offset ,state->m_igs_cg_videoram[offset]);




	/*
    ROM:08020520                 DCW 0x3E                                           ddd1        y
    ROM:08020522                 DCW 0x29                                           ddd2        x
    ROM:08020524                 DCD 0x190BB6                                   ddd3        n
    ROM:08020528                 DCW 0xC                                            ddd4        Y
    ROM:0802052A                 DCW 0xA6                                           ddd5        X

    (ddd5+?)??10bit
    ddd2??9bit
    (ddd4+?)??11bit
    ddd1??8bit
    ddd3??10bit

    8060a4a6 2642ed8f
    A6A46080 8FED4226

    XXXX-XXXX
    XXxx-xxxx
    xxxY-YYYY
    YYYY-YYyy

    yyyy-yynn
    nnnn-nnnn



    */



}


/* TX Layer */
static WRITE32_HANDLER( igs_tx_videoram_w )
{
	igs_m027_state *state = space->machine().driver_data<igs_m027_state>();
	COMBINE_DATA(&state->m_igs_tx_videoram[offset]);
	state->m_igs_tx_tilemap->mark_tile_dirty(offset);
	//if(data!=0)
	//logerror( "TX VIDEO RAM OFFSET %x ,data %x!\n",offset ,state->m_igs_tx_videoram[offset]);
}

static TILE_GET_INFO( get_tx_tilemap_tile_info )
{
	igs_m027_state *state = machine.driver_data<igs_m027_state>();
	//ppppppppNNNNNNNN
	int tileno,colour;
	tileno = state->m_igs_tx_videoram[tile_index] & 0xffff;
	colour = (state->m_igs_tx_videoram[tile_index]>>0x10) & 0xffff;

	SET_TILE_INFO(0,tileno,colour,0);
}

/* BG Layer */
static WRITE32_HANDLER( igs_bg_videoram_w )
{
	igs_m027_state *state = space->machine().driver_data<igs_m027_state>();
	COMBINE_DATA(&state->m_igs_bg_videoram[offset]);
	state->m_igs_bg_tilemap->mark_tile_dirty(offset);
	//if(data!=0)
	logerror("BG VIDEO RAM OFFSET %x ,data %x!\n",offset ,state->m_igs_bg_videoram[offset]);
}

static TILE_GET_INFO( get_bg_tilemap_tile_info )
{
	igs_m027_state *state = machine.driver_data<igs_m027_state>();
	//ppppppppNNNNNNNN
	int tileno,colour;
	tileno = state->m_igs_bg_videoram[tile_index] & 0xffff;
	colour = (state->m_igs_bg_videoram[tile_index]>>0x10) & 0xffff;

	SET_TILE_INFO(0,tileno,colour,0);
}


/* Palette Layer */
static WRITE32_HANDLER( igs_palette32_w )
{
	igs_m027_state *state = space->machine().driver_data<igs_m027_state>();
	space->machine().generic.paletteram.u16=(UINT16 *)state->m_igs_palette32;
	COMBINE_DATA(&state->m_igs_palette32[offset]);
	//paletteram16_xGGGGGRRRRRBBBBB_word_w(offset*2,space->machine().generic.paletteram.u16[offset*2],0);
	//paletteram16_xGGGGGRRRRRBBBBB_word_w(offset*2+1,space->machine().generic.paletteram.u16[offset*2+1],0);
	//if(data!=0)
	//fprintf(stdout,"PALETTE RAM OFFSET %x ,data %x!\n",offset ,state->m_igs_palette32[offset]);
}



static VIDEO_START(igs_majhong)
{
	igs_m027_state *state = machine.driver_data<igs_m027_state>();
	state->m_igs_tx_tilemap= tilemap_create(machine, get_tx_tilemap_tile_info,tilemap_scan_rows, 8, 8,64,32);
	state->m_igs_tx_tilemap->set_transparent_pen(15);
	state->m_igs_bg_tilemap= tilemap_create(machine, get_bg_tilemap_tile_info,tilemap_scan_rows, 8, 8,64,32);
	//state->m_igs_bg_tilemap= tilemap_create(machine, get_bg_tilemap_tile_info,tilemap_scan_rows, 8, 8,64,32);
	//state->m_igs_bg_tilemap->set_transparent_pen(15);
	logerror("Video START OK!\n");
}

static SCREEN_UPDATE_IND16(igs_majhong)
{
	igs_m027_state *state = screen.machine().driver_data<igs_m027_state>();
	//??????????
	bitmap.fill(get_black_pen(screen.machine()), cliprect);

	//??????
	state->m_igs_bg_tilemap->draw(bitmap, cliprect, 0,0);

	//CG??????

	//??????
	state->m_igs_tx_tilemap->draw(bitmap, cliprect, 0,0);
	//fprintf(stdout,"Video UPDATE OK!\n");
	return 0;
}

/***************************************************************************

    Blitter

***************************************************************************/
/***************************************************************************

    Memory Maps

***************************************************************************/

static ADDRESS_MAP_START( igs_majhong_map, AS_PROGRAM, 32 )
	AM_RANGE(0x00000000, 0x00003fff) AM_ROM /* Internal ROM */
	AM_RANGE(0x08000000, 0x0807ffff) AM_ROM AM_REGION("user1", 0)/* Game ROM */
	AM_RANGE(0x10000000, 0x100003ff) AM_RAM AM_BASE_MEMBER(igs_m027_state, m_igs_mainram)// main ram for asic?
	AM_RANGE(0x18000000, 0x18007fff) AM_RAM

	AM_RANGE(0x38001000, 0x380017ff) AM_RAM_WRITE(igs_cg_videoram_w) AM_BASE_MEMBER(igs_m027_state, m_igs_cg_videoram)		//0x200 * 1   CG PALETTE?
	AM_RANGE(0x38001800, 0x38001fff) AM_RAM_WRITE(igs_palette32_w) AM_BASE_MEMBER(igs_m027_state, m_igs_palette32)		//0x200 * 1

	AM_RANGE(0x38004000, 0x38005FFF) AM_RAM_WRITE(igs_tx_videoram_w) AM_BASE_MEMBER(igs_m027_state, m_igs_tx_videoram) /* Text Layer */
	AM_RANGE(0x38006000, 0x38007FFF) AM_RAM_WRITE(igs_bg_videoram_w) AM_BASE_MEMBER(igs_m027_state, m_igs_bg_videoram) /* CG Layer */


	AM_RANGE(0x38002010, 0x38002017) AM_RAM		//??????????????
	AM_RANGE(0x38009000, 0x38009003) AM_RAM		//??????????????
	AM_RANGE(0x70000200, 0x70000203) AM_RAM		//??????????????
	AM_RANGE(0x50000000, 0x500003ff) AM_WRITENOP // uploads xor table to external rom here
	AM_RANGE(0xf0000000, 0xF000000f) AM_WRITENOP // magic registers
ADDRESS_MAP_END


/***************************************************************************

    Common functions

***************************************************************************/

/***************************************************************************

    Code Decryption

***************************************************************************/
static const UINT8 sdwx_tab[] =
{
	0x49,0x47,0x53,0x30,0x30,0x35,0x35,0x52,0x44,0x34,0x30,0x32,0x30,0x36,0x32,0x31,
	0x8A,0xBB,0x20,0x67,0x97,0xA5,0x20,0x45,0x6B,0xC0,0xE8,0x0C,0x80,0xFB,0x49,0xAA,
	0x1E,0xAC,0x29,0xF2,0xB9,0x9F,0x01,0x4A,0x8D,0x5F,0x95,0x96,0x78,0xC3,0xF6,0x65,
	0x17,0xBD,0xB6,0x5B,0x25,0x5F,0x6B,0xDE,0x10,0x2E,0x67,0x05,0xDC,0xAC,0xB6,0xBD,
	0x3D,0x20,0x58,0x3D,0xF0,0xA8,0xC0,0xAD,0x5B,0x82,0x8D,0x12,0x65,0x97,0x87,0x7D,
	0x97,0x49,0xDD,0x74,0x74,0x7E,0x9D,0xA1,0x15,0xED,0x75,0xB9,0x09,0xA8,0xA8,0xB0,
	0x6B,0xEA,0x54,0x1B,0x45,0x23,0xE2,0xE5,0x25,0x42,0xCE,0x36,0xFE,0x42,0x99,0xA0,
	0x41,0xF8,0x0B,0x8C,0x3C,0x1B,0xAE,0xE4,0xB2,0x94,0x87,0x02,0xBC,0x08,0x17,0xD9,
	0xE0,0xA4,0x93,0x63,0x6F,0x28,0x5F,0x4A,0x24,0x36,0xD1,0xDA,0xFA,0xDD,0x23,0x26,
	0x4E,0x61,0xB9,0x7A,0x36,0x4D,0x95,0x01,0x20,0xBC,0x18,0xB7,0xAF,0xE4,0xFB,0x92,
	0xD2,0xE3,0x8E,0xEC,0x26,0xCE,0x2F,0x34,0x8F,0xF7,0x0D,0xD6,0x11,0x7F,0x1F,0x68,
	0xF4,0x1D,0x5F,0x16,0x19,0x2D,0x4C,0x4F,0x96,0xFC,0x9F,0xB0,0x99,0x53,0x4C,0x32,
	0x7B,0x41,0xBC,0x90,0x23,0x2E,0x4A,0xFC,0x9E,0x1D,0xFC,0x02,0xFC,0x41,0x83,0xBC,
	0x6D,0xC4,0x75,0x37,0x9D,0xD3,0xC9,0x26,0x4D,0xED,0x93,0xC6,0x32,0x6D,0x02,0x11,
	0x12,0x56,0x97,0x26,0x1D,0x5F,0xA7,0xF8,0x89,0x3F,0x14,0x36,0x72,0x3B,0x48,0x7B,
	0xF1,0xED,0x72,0xB7,0x7A,0x56,0x05,0xDE,0x7B,0x27,0x6D,0xCF,0x33,0x4C,0x14,0x86,
};



static void sdwx_gfx_decrypt(running_machine &machine)
{
	int i;
	unsigned rom_size = 0x80000;
	UINT8 *src = (UINT8 *) (machine.region("gfx1")->base());
	UINT8 *result_data = auto_alloc_array(machine, UINT8, rom_size);

	for (i=0; i<rom_size; i++)
    	result_data[i] = src[BITSWAP24(i, 23,22,21,20,19,18,17,16,15,14,13,12,11,8,7,6,10,9,5,4,3,2,1,0)];

	for (i=0; i<rom_size; i+=0x200)
	{
		memcpy(src+i+0x000,result_data+i+0x000,0x80);
		memcpy(src+i+0x080,result_data+i+0x100,0x80);
		memcpy(src+i+0x100,result_data+i+0x080,0x80);
		memcpy(src+i+0x180,result_data+i+0x180,0x80);
	}
	auto_free(machine, result_data);
}

/***************************************************************************

    Protection & I/O

***************************************************************************/







/***************************************************************************

    Input Ports

***************************************************************************/

static INPUT_PORTS_START( sdwx )
INPUT_PORTS_END


/***************************************************************************

    Machine Drivers

***************************************************************************/


// for debugging
#if 0
static const gfx_layout charlayout =
{
	8,8,			/* 8 x 8 chars */
	RGN_FRAC(1,1),
	4,				/* 4 bits per pixel */
	{ 0, 1, 2, 3 },    /* planes are packed in a nibble */
	{ 33*4, 32*4, 49*4, 48*4, 1*4, 0*4, 17*4, 16*4 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	32*8	/* 32 bytes per char */
};
#endif

static const gfx_layout gfxlayout_8x8x4 =
{
    8,8,
    RGN_FRAC(1,1),
    4,
    //{ STEP4(0,8) },
    { 24,8,16,0 },
		{ STEP8(7,-1) },
    { STEP8(0,4*8) },
    8*8*4
};

#if 0
static const gfx_layout gfxlayout_16x16x16 =
{
    16,16,
    RGN_FRAC(1,1),
    16,
    { STEP16(0,0) },	// >8planes not supported
    { STEP16(15,-1) },
    { STEP16(0,16*1) },
    16*16*16
};
#endif

static GFXDECODE_START( igs_m027 )
    GFXDECODE_ENTRY( "gfx1", 0, gfxlayout_8x8x4,   0, 16  )
   // GFXDECODE_ENTRY( "gfx2", 0, gfxlayout_16x16x16, 0, 16  )
GFXDECODE_END


static INTERRUPT_GEN( igs_majhong_interrupt )
{
	generic_pulse_irq_line(device, ARM7_FIRQ_LINE, 1);
}


static MACHINE_CONFIG_START( igs_majhong, igs_m027_state )
	MCFG_CPU_ADD("maincpu",ARM7, 20000000)

	MCFG_CPU_PROGRAM_MAP(igs_majhong_map)

	MCFG_CPU_VBLANK_INT("screen", igs_majhong_interrupt)
	//MCFG_NVRAM_ADD_0FILL("nvram")

	MCFG_GFXDECODE(igs_m027)


	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(512, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 512-1, 0, 256-1)
	MCFG_SCREEN_UPDATE_STATIC( igs_majhong )

	MCFG_PALETTE_LENGTH(0x200)

	MCFG_VIDEO_START( igs_majhong )

	/* sound hardware */

MACHINE_CONFIG_END





/***************************************************************************

    ROMs Loading

***************************************************************************/
ROM_START( sdwx )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "sdwx_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "prg.u16", 0x000000, 0x80000, CRC(c94ef6a8) SHA1(69f2f356e05206b0866a9020253d9a112b56316c) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "text.u24", 0x000000, 0x80000, CRC(60b415ac) SHA1(b4475b0ba1e70504cac9ac05078873df0b16495b) )

	ROM_REGION( 0x200000, "gfx2", 0 )
	ROM_LOAD( "cg.u25", 0x000000, 0x200000, CRC(709b9a42) SHA1(18c4b8e159b29c168f5cafb437fe6eb123672471) )

	ROM_REGION( 0x80000, "unknown", 0 )
	ROM_LOAD( "sp.u2", 0x00000, 0x80000, CRC(216b5418) SHA1(b7bc24ced0ccb5476c974420aa506c13b971fc9f) )
ROM_END




ROM_START( sddz )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "sddz_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "ddz_218cn.u17", 0x000000, 0x80000, CRC(3cfe38d5) SHA1(9c7f82ecffbc22879583519d5f753bb35e973ee3) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "ddz_text.u27", 0x000000, 0x80000, CRC(520dc392) SHA1(0ab2620f20af8253806b6ff4e1d9d77a694da17c) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "ddz_ani.u28", 0x000000, 0x400000, CRC(72487508) SHA1(9f4bbc858960ddaae403e4a3330b2345f6fd6cb3))

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "ddz_sp.u4", 0x00000, 0x200000, CRC(7ef65d95) SHA1(345c587cd449d6d06908e9687480be76b2cb2d28) )
ROM_END



ROM_START( lhzb3 )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "lhzb3_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "lhzb3_104.u9", 0x000000, 0x80000, CRC(70d61846) SHA1(662b59702ef6f26129de6b16346786df92f99097) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "lhzb3_text.u17", 0x000000, 0x80000,CRC(a82398a9) SHA1(4d2987f57096b7f24ce6571ed3be6dcb33bce88d) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "m2401.u18", 0x000000, 0x400000,  CRC(81428f18) SHA1(9fb19c8a79cc3443642f4b044e04735df2cb45be) )

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "s2402.u14", 0x00000, 0x100000, CRC(56083fe2) SHA1(62afd651809bf5e639bfda6e5579dbf4b903b664) )
ROM_END


ROM_START( lhzb4 )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "lhzb4_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "lhzb4_104.u17", 0x000000, 0x80000, CRC(6f349bbb) SHA1(54cf895889ef0f208637ba732ede696ca3603ee0) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "lhzb4_text.u27", 0x000000, 0x80000, CRC(8488b039) SHA1(59bc9eccba810fcac2a53866b2da1e71bfd8a6e7) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "a05501.u28", 0x000000, 0x400000, CRC(f78b3714) SHA1(c73d8e50b04126bc4f91783384713624ed133ee2) )

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "w05502.u5", 0x00000, 0x200000, CRC(467f677e) SHA1(63927c0d606176c0e22db89ea3a9777ed702abbd) )
ROM_END



ROM_START( klxyj )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "klxyj_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "klxyj_104.u16", 0x000000, 0x80000, CRC(8cb9bdc2) SHA1(5a13d0ff6488a938617a9ea89e7cf607539a1f49) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "klxyj_text.u24", 0x000000, 0x80000, CRC(22dcebd0) SHA1(0383f017135230d020d12c8c6cc3aeb136fe9106) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "a4202.u25", 0x000000, 0x400000, CRC(97a68f85) SHA1(177c8c23fd0d585b24a71359ede005ac9a2e4d4d) )

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "w4201.u2", 0x00000, 0x100000, CRC(464f11ab) SHA1(56e45bd31f667fc30387fcd4c940a94819b7ef0f) )
ROM_END


ROM_START( mgfx )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "mgfx_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "mgfx_101.u10", 0x000000, 0x80000, CRC(897c88a1) SHA1(0f7a7808b9503ff28ad32c0b8e071cb24cff59b1) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "mgfx_text.u9", 0x000000, 0x80000, CRC(e41e7768) SHA1(3d0add7c75c23533309e799fd8853c815e6f811c) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "mgfx_ani.u17", 0x000000, 0x400000, CRC(9fc75f4d) SHA1(acb600739dcf252a5210e28ec96d749573061b27) )

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "mgfx_sp.u14", 0x00000, 0x100000, CRC(9bb28fc8) SHA1(6368753c29607f2d212d68c5cca3f10aa069649b) )
ROM_END


/*

Big D2
IGS, 2000

PCB Layout
----------

IGS PCB NO-0267
|------------------------------------------|
|M2601.U17  PAL |-------|           RESET  |
|    M2603.U18  |       |                  |
|               |IGS027A|                  |
|               |       |         BATT_3.6V|
|               |-------|                  |
|                          W24257          |
|J                                S2602.U14|
|A         |-------|                       |
|M T2604.U9|       |                       |
|M         |IGS031 |  P2600.U10            |
|A         |       |                 M6295 |
|          |-------|                       |
|                                          |
|       22MHz     W24257                   |
|                                          |
|                        8255         VOL  |
|    DSW1(8)                               |
|        DSW2(8)             LM7805        |
|                                 UPC1242H |
|------------------------------------------|
Notes:
      W24257     - Winbond 32kx8 SRAM (SOJ28)
      Custom ICs -
                  IGS027A - ARM7/9? based CPU (QFP120, labelled 'J8')
                  IGS033  - likey GFX processor. Appears to be linked to the 3.6V battery. However,
                  the battery was dead and the PCB still works, so maybe the battery is not used? (QFP208)
      ROMs -
            P2600.U10 - 27C4096 EPROM, Main program
            M2601.U17 - 32MBit DIP42 MaskROM, read as 27C322, GFX (stamped 'IMAGE')
            M2603.U18 - 4MBit DIP40 EPROM, read as 27C4096, GFX (stamped 'IMAGE')
            S2602.U14 - 8MBit DIP32 MaskROM, read as MX27C8000, Oki M6295 sound data (stamped 'SPEECH')
            T2604.U9  - 4MBit DIP40 MaskROM, read as 27C4096, GFX (stamped 'TEXT')

*/

ROM_START( bigd2 )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "bigd2_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "p2600.u10", 0x000000, 0x80000, CRC(9ad34135) SHA1(54717753d1296efe49946369fd4a27181f19dbc0) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "t2604.u9", 0x000000, 0x80000, CRC(5401a52d) SHA1(05b47a4b39939c1d5904e3fbd5cc56d6ee9b7953) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "m2601.u17", 0x000000, 0x400000, CRC(89736e3f) SHA1(6a22e2eb10d2c740cf21640c43a8caf4c72d3be7) )
	ROM_LOAD( "m2603.u18", 0x000000, 0x080000, CRC(fb2e91a8) SHA1(29b2f0ce3749539cbe4cfb5c40b240cc7f6147f1) )

	ROM_REGION( 0x200000, "unknown", 0 )
	ROM_LOAD( "s2602.u14", 0x00000, 0x100000, CRC(f137028c) SHA1(0e4114222820bca2f7026fa653e2b96a489a0183) )
ROM_END

/*


Gone Fishing II
IGS PCB-0388-05-FW

   +--------------------------------------------+
+--+               8-Liner Connecter            +---+
|                                                   |
|    +---------------+                            +-+
|    |   IGS 0027A   |     +------+               |
+-+  |    Plug-in    |     | IGS  |               +-+
  |  |   Daughter    |     | 025  |                P|
+-+  |     Card      |     +------+                r|
|    +---------------+ +---+                       i|
|J                     |   |                       n|
|A   +---+ +---+ +---+ |   |                       t|
|M   |   | |   | |   | | U |                      +-+
|M   |   | |   | |   | | 1 |                +---+ |
|A   | U | | U | | U | | 2 |                |   | +-+
|    | 1 | | 1 | | 1 | |   |         +----+ | U |   |
|C   | 5 | | 7 | | 4 | |   |         |Oki | | 1 |   |
|o   |   | |   | | * | |   |         |6295| | 3 |   |
|n   |   | |   | |   | +---+         +----+ |   |   |
|n   |   | |   | |   |                      +---+   |
|e   +---+ |   | +---+                              |
|c         +---+                                    |
|t                        62257                     |
|e                                                  |
|r       +-------+                                  |
|        |       |                                  |
|        |  IGS  |                                  |
|        |  031  |     61256                        |
+-+      |       |          PAL     V3021           |
  |      +-------+                                  |
+-+                                      X1    SW4  |
|                                                   |
| JP11                        SW3 SW2 SW1      BT1  |
|                                                   |
+---------------------------------------------------+


U12 - Program rom   - 27C4096
U15 - Text graphics - 27C4096
U17 - Char graphics - 27C160
U23 - Sound samples - 27C040

SW1-SW3 are unpopulated
U14* Not used (27C4096) or else it's U16 and 27C160 type EPROM

   X1 - 32.768kHZ OSC
V3021 - Micro Electronic Ultra Low Power 1-Bit 32kHz RTC (Real Time Clock)
  PAL - ATF22V10C at U26 labeled FW U26
  BT1 - 3.6V battery
  SW4 - Toggle switch
 JP11 - 4 Pin header (HD4-156)

IGS 025  - Custom programmed A8B1723(?)
IGS 0027 - Custom programmed ARM9

*/



ROM_START( gonefsh2 )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "gonefsh2_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "gfii_v-904uso.u12", 0x000000, 0x80000, CRC(ef0f6735) SHA1(0add92599b0989f3e50dc64e32ce234b4bd87d33) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "gfii_text.u15", 0x000000, 0x80000, CRC(b48118fd) SHA1(e718d23ce5f7f41ab94df2d05cdd3adbf27eef89) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "gfii_cg.u17", 0x000000, 0x200000, CRC(2568359c) SHA1(f1f240246e53496bf624c84f7cae3edb9675579f) )

	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "gfii_sp.u13", 0x00000, 0x080000, CRC(61da1d58) SHA1(0a79578f0daf15f0efe2b0eeac59a60d8372a644) )
ROM_END

/*


Chess Challenge II

IGS PCB-0388-04-FW

   +--------------------------------------------+
+--+               8-Liner Connecter            +---+
|                                                   |
|    +---------------+                            +-+
|    |   IGS 0027A   |     +------+               |
+-+  |    Plug-in    |     | IGS  |               +-+
  |  |   Daughter    |     | 025  |                P|
+-+  |     Card      |     +------+                r|
|    +---------------+ +---+                       i|
|J                     |   |                       n|
|A   +---+ +---+ +---+ |   |                       t|
|M   |   | |   | |   | | U |                      +-+
|M   |   | |   | |   | | 1 |                +---+ |
|A   | U | | U | | U | | 2 |                |   | +-+
|    | 1 | | 1 | | 1 | |   |         +----+ | U |   |
|C   | 5 | | 7 | | 4 | |   |         |Oki | | 1 |   |
|o   |   | |   | | * | |   |         |6295| | 3 |   |
|n   |   | |   | |   | +---+         +----+ |   |   |
|n   |   | |   | |   |                      +---+   |
|e   +---+ |   | +---+                              |
|c         +---+                                    |
|t                        62257                     |
|e                                                  |
|r       +-------+                                  |
|        |       |                                  |
|        |  IGS  |                                  |
|        |  031  |     61256                        |
+-+      |       |          PAL     V3021           |
  |      +-------+                                  |
+-+                                      X1    SW4  |
|                                                   |
| JP11                        SW3 SW2 SW1      BT1  |
|                                                   |
+---------------------------------------------------+


U12 - Program rom   - 27C4096
U15 - Text graphics - 27C4096
U17 - Char graphics - 27C160
U23 - Sound samples - 27C040

SW1-SW3 are unpopulated
U14* Not used (27C4096) or else it's U16 and 27C160 type EPROM

   X1 - 32.768K OSC
V3021 - Micro Electronic Ultra Low Power 1-Bit 32kHz RTC (Real Time Clock)
  PAL - ATF22V10C at U26 labeled FW U26
  BT1 - 3.6V battery
  SW4 - Toggle switch
 JP11 - 4 Pin header (HD4-156)

IGS 025  - Custom programmed A8B1723(?)
IGS 0027 - Custom programmed ARM9

*/

ROM_START( chessc2 )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "chessc2_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "ccii_v-707uso.u12", 0x000000, 0x80000, CRC(5937b67b) SHA1(967b3adf6f5bf92d63ec460d595e473898a78372) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "ccii_text.u15", 0x000000, 0x80000, CRC(25fed033) SHA1(b321c4994f609906597c3f7d5cdfc2dca63cd340) )

	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "ccii_cg.u17", 0x000000, 0x200000, CRC(47e45157) SHA1(4459799a4a6c30a2d0a3ad9ac54e92b62221e10b) )

	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "ccii_sp.u13", 0x00000, 0x080000,  CRC(220a7b71) SHA1(7dab7baa97c20b83763cf46ef0a6e5e8c4d6a348) )
ROM_END





ROM_START( haunthig )
	ROM_REGION( 0x04000, "maincpu", 0 )
	/* Internal rom of IGS027A ARM based MCU */
	ROM_LOAD( "haunthig_igs027a", 0x00000, 0x4000, NO_DUMP )

	ROM_REGION( 0x80000, "user1", 0 ) // external ARM data / prg
	ROM_LOAD( "hauntedhouse_ver-101us.u34", 0x000000, 0x80000, CRC(4bf045d4) SHA1(78c848fd69961df8d9b75f92ad57c3534fbf08db) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "haunted-h_text.u15", 0x000000, 0x80000, CRC(c23f48c8) SHA1(0cb1b6c61611a081ae4a3c0be51812045ff632fe) )

	// are these PGM-like sprites?
	ROM_REGION( 0x400000, "gfx2", 0 )
	ROM_LOAD( "haunted-h_cg.u32", 0x000000, 0x400000, CRC(e0ea10e6) SHA1(e81be78fea93e72d4b1f4c0b58560bda46cf7948) )
	ROM_REGION( 0x400000, "gfx3", 0 )
	ROM_LOAD( "haunted-h_ext.u12", 0x000000, 0x400000, CRC(662eb883) SHA1(831ebe29e1e7a8b2c2fff7fbc608975771c3486c) )


	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "haunted-h_sp.u3", 0x00000, 0x200000,  CRC(fe3fcddf) SHA1(ac57ab6d4e4883747c093bd19d0025cf6588cb2c) )
ROM_END

static void pgm_create_dummy_internal_arm_region(running_machine &machine)
{
	UINT16 *temp16 = (UINT16 *)machine.region("maincpu")->base();

	// fill with RX 14
	int i;
	for (i=0;i<0x4000/2;i+=2)
	{
		temp16[i] = 0xff1e;
		temp16[i+1] = 0xe12f;

	}

	// jump straight to external area
	temp16[(0x0000)/2] = 0xd088;
	temp16[(0x0002)/2] = 0xe59f;
	temp16[(0x0004)/2] = 0x0680;
	temp16[(0x0006)/2] = 0xe3a0;
	temp16[(0x0008)/2] = 0xff10;
	temp16[(0x000a)/2] = 0xe12f;
	temp16[(0x0090)/2] = 0x0400;
	temp16[(0x0092)/2] = 0x1000;
}


static void sdwx_decrypt(running_machine &machine)
{

	int i;
	UINT16 *src = (UINT16 *) machine.region("user1")->base();

	int rom_size = 0x80000;

	for(i=0; i<rom_size/2; i++)
	{
		UINT16 x = src[i];

		if((i & 0x000480) != 0x000080)  x ^= 0x0001;
		if((i & 0x004008) == 0x004008)  x ^= 0x0002;
		if((i & 0x000030) == 0x000010)  x ^= 0x0004;
		if((i & 0x000242) != 0x000042)  x ^= 0x0008;
		if((i & 0x008100) == 0x008000)  x ^= 0x0010;
		if((i & 0x022004) != 0x000004)  x ^= 0x0020;
		if((i & 0x011800) != 0x010000)  x ^= 0x0040;
		if((i & 0x004820) == 0x004820)  x ^= 0x0080;

		x ^= sdwx_tab[(i >> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 hauntedh_tab[0x100] = {
	0x49, 0x47, 0x53, 0x30, 0x32, 0x35, 0x34, 0x52, 0x44, 0x34, 0x30, 0x36, 0x30, 0x35, 0x32, 0x36,
	0x6C, 0x65, 0x33, 0xFD, 0x7A, 0x71, 0x3D, 0xB8, 0x07, 0xF1, 0x86, 0x96, 0x19, 0x5A, 0xA2, 0x05,
	0x49, 0xB1, 0xED, 0x2E, 0x7C, 0x7A, 0x65, 0x8B, 0xE1, 0xE3, 0xC8, 0xAA, 0x2B, 0x32, 0xEE, 0x3F,
	0x10, 0x6C, 0x69, 0x70, 0x02, 0x47, 0x5B, 0x5D, 0x2D, 0x52, 0x97, 0xEF, 0xB1, 0x63, 0xFB, 0xE3,
	0x21, 0x41, 0x0C, 0x17, 0x3C, 0x93, 0xD4, 0x13, 0xEB, 0x08, 0xF9, 0xDB, 0x7A, 0xC8, 0x1E, 0xF4,
	0x1B, 0x1B, 0x7F, 0xB4, 0x98, 0x59, 0xC8, 0xCF, 0x58, 0x12, 0x36, 0x1F, 0x96, 0x7D, 0xF0, 0xB3,
	0xDC, 0x26, 0xA8, 0x1C, 0xC6, 0xD4, 0x6E, 0xF3, 0xF5, 0xB9, 0xD4, 0xAF, 0x52, 0xDD, 0x48, 0xA5,
	0x85, 0xCC, 0xAD, 0x60, 0xB4, 0x7F, 0x3C, 0x24, 0x80, 0x88, 0x9B, 0xBD, 0x3E, 0x82, 0x3B, 0x8D,
	0x73, 0xB8, 0xF7, 0xD5, 0x92, 0x15, 0xeb, 0x43, 0xF9, 0x4C, 0x91, 0xBD, 0x29, 0x48, 0x22, 0x6D,
	0x45, 0xD6, 0x2C, 0x0D, 0xCE, 0x91, 0x70, 0x74, 0x9D, 0x0E, 0xFE, 0x62, 0x22, 0x49, 0x94, 0x88,
	0xDB, 0x50, 0x33, 0xDB, 0x18, 0x2E, 0x03, 0x1B, 0xED, 0x1A, 0x69, 0x9E, 0x78, 0xE1, 0x66, 0x62,
	0x54, 0x91, 0x33, 0x52, 0x5E, 0x67, 0x1B, 0xD9, 0xA7, 0xFB, 0x98, 0xA5, 0xBA, 0xAA, 0xB1, 0xBD,
	0x0F, 0x44, 0x93, 0xC6, 0xCF, 0xF7, 0x6F, 0x91, 0xCA, 0x7B, 0x93, 0xEA, 0xB6, 0x7F, 0xCC, 0x9C,
	0xAB, 0x54, 0xFB, 0xC8, 0xDB, 0xD9, 0xF5, 0x68, 0x96, 0xA7, 0xA1, 0x1F, 0x7D, 0x7D, 0x4C, 0x43,
	0x06, 0xED, 0x50, 0x2D, 0x30, 0x48, 0xE6, 0xC0, 0x88, 0xC8, 0x48, 0x38, 0x5D, 0xFC, 0x0a, 0x35,
	0x3F, 0x79, 0xBA, 0x07, 0xBE, 0xBF, 0xB7, 0x3B, 0x61, 0x69, 0x4F, 0x67, 0xE5, 0x9A, 0x1D, 0x33
};

static void hauntedh_decrypt(running_machine &machine)
{
	int i;
	UINT16 *src = (UINT16 *) machine.region("user1")->base();

	int rom_size = 0x080000;

	for(i=0; i<rom_size/2; i++) {
    		UINT16 x = src[i];

		if ((i & 0x040480) != 0x000080) x ^= 0x0001;
	//  if ((i & 0x104008) == 0x104008) x ^= 0x0002;
	//  if ((i & 0x080030) == 0x080010) x ^= 0x0004;
		if ((i & 0x000042) != 0x000042) x ^= 0x0008;
	//  if ((i & 0x048100) == 0x048000) x ^= 0x0010;
		if ((i & 0x002004) != 0x000004) x ^= 0x0020;
		if ((i & 0x001800) != 0x000000) x ^= 0x0040;
		if ((i & 0x004820) == 0x004820) x ^= 0x0080;

		x ^= hauntedh_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 chessc2_tab[0x100] = {
	0x49, 0x47, 0x53, 0x30, 0x30, 0x38, 0x32, 0x52, 0x44, 0x34, 0x30, 0x32, 0x31, 0x32, 0x31, 0x31,
	0x28, 0xCA, 0x9C, 0xAD, 0xBB, 0x2D, 0xF0, 0x41, 0x6E, 0xCE, 0xAD, 0x73, 0xAE, 0x1C, 0xD1, 0x14,
	0x6F, 0x9A, 0x75, 0x18, 0xA8, 0x91, 0x68, 0xe4, 0x09, 0xF4, 0x0F, 0xD7, 0xFF, 0x93, 0x7D, 0x1B,
	0xEB, 0x84, 0xce, 0xAD, 0x9E, 0xCF, 0xC9, 0xAB, 0x18, 0x59, 0xb6, 0xde, 0x82, 0x13, 0x7C, 0x88,
	0x69, 0x63, 0xFF, 0x6F, 0x3C, 0xD2, 0xB9, 0x29, 0x09, 0xF8, 0x97, 0xAA, 0x74, 0xA5, 0x16, 0x0D,
	0xF9, 0x51, 0x9E, 0x9f, 0x63, 0xC6, 0x1E, 0x32, 0x8C, 0x0C, 0xE9, 0xA0, 0x56, 0x95, 0xD1, 0x9D,
	0xEA, 0xA9, 0x82, 0xC3, 0x30, 0x15, 0x21, 0xD8, 0x8F, 0x10, 0x25, 0x61, 0xE6, 0x6D, 0x75, 0x6D,
	0xCB, 0x08, 0xC3, 0x9B, 0x03, 0x6A, 0x28, 0x6D, 0x42, 0xBF, 0x00, 0xd2, 0x24, 0xFA, 0x08, 0xEE,
	0x6B, 0x46, 0xB7, 0x2C, 0x7B, 0xB0, 0xDA, 0x86, 0x13, 0x14, 0x73, 0x14, 0x4D, 0x45, 0xD3, 0xD4,
	0xD9, 0x80, 0xF5, 0xB8, 0x76, 0x13, 0x1E, 0xF6, 0xB1, 0x4A, 0xB3, 0x8B, 0xE2, 0x9A, 0x5A, 0x11,
	0x64, 0x11, 0x55, 0xC3, 0x14, 0xFD, 0x1B, 0xCe, 0x0C, 0xDC, 0x38, 0xDA, 0xA1, 0x84, 0x66, 0xD9,
	0x9b, 0x93, 0xED, 0x0F, 0xB4, 0x19, 0x38, 0x62, 0x53, 0x85, 0xB9, 0xE5, 0x89, 0xCd, 0xFE, 0x9E,
	0x4D, 0xE2, 0x14, 0x9F, 0xF4, 0x53, 0x1C, 0x46, 0xf4, 0x40, 0x2C, 0xCC, 0xDa, 0x82, 0x69, 0x15,
	0x88, 0x18, 0x62, 0xB7, 0xB4, 0xD5, 0xAF, 0x4B, 0x9E, 0x48, 0xCA, 0xF4, 0x11, 0xEC, 0x2D, 0x2C,
	0x9D, 0x91, 0xAD, 0xDA, 0x13, 0x0A, 0x16, 0x86, 0x41, 0x18, 0x08, 0x01, 0xef, 0x97, 0x11, 0x1f,
	0x1A, 0xE7, 0x0C, 0xC9, 0x6D, 0x9D, 0xB9, 0x49, 0x0B, 0x6B, 0x9E, 0xD4, 0x72, 0x4D, 0x1D, 0x59
};

static void chessc2_decrypt(running_machine &machine)
{
	int i;
	UINT16 *src = (UINT16 *) machine.region("user1")->base();

	int rom_size = 0x80000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if ((i & 0x040480) != 0x000080) x ^= 0x0001;
		if ((i & 0x004008) == 0x004008) x ^= 0x0002;
	//  if ((i & 0x080030) == 0x080010) x ^= 0x0004;
		if ((i & 0x000242) != 0x000042) x ^= 0x0008;
		if ((i & 0x008100) == 0x008000) x ^= 0x0010;
		if ((i & 0x002004) != 0x000004) x ^= 0x0020; // correct??
		if ((i & 0x011800) != 0x010000) x ^= 0x0040;
		if ((i & 0x004820) == 0x004820) x ^= 0x0080;

		x ^= chessc2_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 klxyj_tab[0x100] = {
	0x49, 0x47, 0x53, 0x30, 0x30, 0x30, 0x38, 0x52, 0x44, 0x34, 0x30, 0x31, 0x30, 0x39, 0x32, 0x34,
	0x3F, 0x0F, 0x66, 0x9A, 0xBF, 0x0D, 0x06, 0x55, 0x09, 0x01, 0xEB, 0x72, 0xEB, 0x9B, 0x89, 0xFA,
	0x24, 0xD1, 0x5D, 0xCA, 0xE6, 0x8A, 0x8C, 0xE0, 0x92, 0x8D, 0xBF, 0xE4, 0xAF, 0xAA, 0x3E, 0xFA,
	0x2B, 0x27, 0x4B, 0xC7, 0xD6, 0x6D, 0xC1, 0xC2, 0x1C, 0xF4, 0xED, 0xBD, 0x03, 0x6C, 0xAD, 0xB3,
	0x65, 0x2D, 0xC7, 0xD3, 0x6E, 0xE0, 0x8C, 0xCE, 0x59, 0x6F, 0xAE, 0x5E, 0x66, 0x2B, 0x5E, 0x17,
	0x20, 0x3D, 0xA9, 0x72, 0xCD, 0x4F, 0x14, 0x17, 0x35, 0x7B, 0x77, 0x6B, 0x98, 0x73, 0x17, 0x5A,
	0xEA, 0xF2, 0x07, 0x66, 0x51, 0x64, 0xC1, 0xF0, 0xE2, 0xD1, 0x00, 0xC6, 0x97, 0x0F, 0xE0, 0xEE,
	0x94, 0x28, 0x39, 0xB2, 0x9B, 0x0A, 0x38, 0xED, 0xCC, 0x6E, 0x40, 0x94, 0xA2, 0x0A, 0x00, 0x88,
	0x2B, 0xFA, 0xD5, 0x9A, 0x87, 0x6C, 0x62, 0xDF, 0xA4, 0x8B, 0x6D, 0x37, 0x38, 0xAE, 0xFD, 0x18,
	0xFF, 0xC2, 0xB2, 0xA0, 0x37, 0xF5, 0x64, 0xDB, 0x59, 0xA5, 0x00, 0x51, 0x19, 0x88, 0x9F, 0xD4,
	0xA0, 0x1C, 0xE7, 0x88, 0x08, 0x51, 0xA7, 0x33, 0x19, 0x75, 0xAE, 0xC7, 0x42, 0x61, 0xEC, 0x2D,
	0xDB, 0xE2, 0xCC, 0x54, 0x9A, 0x6A, 0xD1, 0x7A, 0x53, 0xF8, 0x6F, 0xBA, 0xF4, 0x45, 0x2C, 0xD7,
	0xC0, 0x30, 0xF7, 0x47, 0xCC, 0x6B, 0xC8, 0x83, 0xB7, 0x67, 0x7A, 0x8E, 0xAD, 0x7E, 0xE5, 0xC4,
	0x9F, 0x60, 0x40, 0xE5, 0xBC, 0xC0, 0xB5, 0x61, 0x33, 0x3F, 0x46, 0xE6, 0x2D, 0x98, 0xDF, 0x28,
	0x05, 0x0E, 0xBC, 0xF0, 0xCA, 0x13, 0xFE, 0x68, 0xF7, 0x3A, 0x89, 0xA5, 0x71, 0x5F, 0x21, 0x76,
	0xC2, 0x14, 0xC5, 0x6C, 0x95, 0x4f, 0x4f, 0x2A, 0x71, 0x52, 0x3C, 0xEE, 0xAA, 0xDB, 0xf1, 0x00
};

static void klxyj_decrypt(running_machine &machine)
{
	int i;
	UINT16 *src = (UINT16 *) machine.region("user1")->base();

	int rom_size = 0x80000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if ((i & 0x040480) != 0x000080) x ^= 0x0001;
		if ((i & 0x004008) == 0x004008) x ^= 0x0002;
	//  if ((i & 0x080030) == 0x080010) x ^= 0x0004;
		if ((i & 0x000242) != 0x000042) x ^= 0x0008;
		if ((i & 0x008100) == 0x008000) x ^= 0x0010;
		if ((i & 0x002004) != 0x000004) x ^= 0x0020;
		if ((i & 0x011800) != 0x010000) x ^= 0x0040;
		if ((i & 0x004820) == 0x004820) x ^= 0x0080;

		x ^= klxyj_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}

static DRIVER_INIT( igs_m027 )
{
	pgm_create_dummy_internal_arm_region(machine);
}

static DRIVER_INIT( sdwx )
{
	sdwx_decrypt(machine);
	sdwx_gfx_decrypt(machine);
	pgm_create_dummy_internal_arm_region(machine);
}

static DRIVER_INIT( klxyj )
{
	klxyj_decrypt(machine);
	//sdwx_gfx_decrypt(machine);
	pgm_create_dummy_internal_arm_region(machine);
}

static DRIVER_INIT( chessc2 )
{
	chessc2_decrypt(machine);
	//sdwx_gfx_decrypt(machine);
	pgm_create_dummy_internal_arm_region(machine);
}

static DRIVER_INIT( hauntedh )
{
	hauntedh_decrypt(machine);
	//sdwx_gfx_decrypt(machine);
	pgm_create_dummy_internal_arm_region(machine);
}


/***************************************************************************

    Game Drivers

***************************************************************************/

GAME( 2002,  sdwx,		0, igs_majhong, sdwx, sdwx,        ROT0, "IGS", "Sheng Dan Wu Xian", GAME_NO_SOUND | GAME_NOT_WORKING ) // aka Christmas 5 Line?
GAME( 200?,  sddz,		0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Super Dou Di Zhu",  GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 2000,  bigd2,		0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Big D2",  GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  lhzb3,		0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Long Hu Zheng Ba 3", GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  lhzb4,		0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Long Hu Zheng Ba 4", GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  klxyj,		0, igs_majhong, sdwx, klxyj,       ROT0, "IGS", "Kuai Le Xi You Ji",  GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 2000,  mgfx,		0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Man Guan Fu Xing",   GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  gonefsh2,	0, igs_majhong, sdwx, igs_m027,    ROT0, "IGS", "Gone Fishing 2",   GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  chessc2,	0, igs_majhong, sdwx, chessc2,     ROT0, "IGS", "Chess Challenge 2",   GAME_NO_SOUND | GAME_NOT_WORKING )
GAME( 200?,  haunthig,	0, igs_majhong, sdwx, hauntedh,    ROT0, "IGS", "Haunted House (IGS)",   GAME_NO_SOUND | GAME_NOT_WORKING )
