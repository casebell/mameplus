/***************************************************************************

    Model Racing Dribbling hardware

    driver by Aaron Giles

    Games supported:
        * Dribbling

****************************************************************************

    Memory map

****************************************************************************

    ========================================================================
    CPU #1
    ========================================================================
    tbd
    ========================================================================
    Interrupts:
        NMI not connected
        IRQ generated by VBLANK
    ========================================================================

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "machine/8255ppi.h"
#include "dribling.h"



/*************************************
 *
 *  Global variables
 *
 *************************************/

/* referenced by the video hardware */
UINT8 dribling_abca;

static UINT8 dr, ds, sh;
static UINT8 input_mux;
static UINT8 di;



/*************************************
 *
 *  Interrupt generation
 *
 *************************************/

static INTERRUPT_GEN( dribling_irq_gen )
{
	if (di)
		cpu_set_input_line(device, 0, ASSERT_LINE);
}



/*************************************
 *
 *  PPI inputs
 *
 *************************************/

static READ8_DEVICE_HANDLER( dsr_r )
{
	/* return DSR0-7 */
	return (ds << sh) | (dr >> (8 - sh));
}


static READ8_DEVICE_HANDLER( input_mux0_r )
{
	/* low value in the given bit selects */
	if (!(input_mux & 0x01))
		return input_port_read(device->machine, "MUX0");
	else if (!(input_mux & 0x02))
		return input_port_read(device->machine, "MUX1");
	else if (!(input_mux & 0x04))
		return input_port_read(device->machine, "MUX2");
	return 0xff;
}



/*************************************
 *
 *  PPI outputs
 *
 *************************************/

static WRITE8_DEVICE_HANDLER( misc_w )
{
	/* bit 7 = di */
	di = (data >> 7) & 1;
	if (!di)
		cputag_set_input_line(device->machine, "maincpu", 0, CLEAR_LINE);

	/* bit 6 = parata */

	/* bit 5 = ab. campo */
	dribling_abca = (data >> 5) & 1;

	/* bit 4 = ab. a.b.f. */
	/* bit 3 = n/c */

	/* bit 2 = (9) = PC2 */
	/* bit 1 = (10) = PC1 */
	/* bit 0 = (32) = PC0 */
	input_mux = data & 7;
	logerror("%s:misc_w(%02X)\n", cpuexec_describe_context(device->machine), data);
}


static WRITE8_DEVICE_HANDLER( sound_w )
{
	/* bit 7 = stop palla */
	/* bit 6 = contrasto */
	/* bit 5 = calgio a */
	/* bit 4 = fischio */
	/* bit 3 = calcio b */
	/* bit 2 = folla a */
	/* bit 1 = folla m */
	/* bit 0 = folla b */
	logerror("%s:sound_w(%02X)\n", cpuexec_describe_context(device->machine), data);
}


static WRITE8_DEVICE_HANDLER( pb_w )
{
	/* write PB0-7 */
	logerror("%s:pb_w(%02X)\n", cpuexec_describe_context(device->machine), data);
}


static WRITE8_DEVICE_HANDLER( shr_w )
{
	/* bit 3 = watchdog */
	if (data & 0x08)
		watchdog_reset(device->machine);

	/* bit 2-0 = SH0-2 */
	sh = data & 0x07;
}



/*************************************
 *
 *  PPI accessors
 *
 *************************************/

static READ8_HANDLER( ioread )
{
	if (offset & 0x08)
		return ppi8255_r(devtag_get_device(space->machine, "ppi8255_0"), offset & 3);
	else if (offset & 0x10)
		return ppi8255_r(devtag_get_device(space->machine, "ppi8255_1"), offset & 3);
	return 0xff;
}


static WRITE8_HANDLER( iowrite )
{
	if (offset & 0x08)
		ppi8255_w(devtag_get_device(space->machine, "ppi8255_0"), offset & 3, data);
	else if (offset & 0x10)
		ppi8255_w(devtag_get_device(space->machine, "ppi8255_1"), offset & 3, data);
	else if (offset & 0x40)
	{
		dr = ds;
		ds = data;
	}
}



/*************************************
 *
 *  Machine init
 *
 *************************************/

static const ppi8255_interface ppi8255_intf[2] =
{
	{
		DEVCB_HANDLER(dsr_r),
		DEVCB_HANDLER(input_mux0_r),
		DEVCB_NULL,
		DEVCB_NULL,
		DEVCB_NULL,
		DEVCB_HANDLER(misc_w)
	},
	{
		DEVCB_NULL,
		DEVCB_NULL,
		DEVCB_INPUT_PORT("IN0"),
		DEVCB_HANDLER(sound_w),
		DEVCB_HANDLER(pb_w),
		DEVCB_HANDLER(shr_w)
	}
};



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( dribling_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x3fff) AM_RAM AM_BASE(&videoram)
	AM_RANGE(0x4000, 0x7fff) AM_ROM
	AM_RANGE(0xc000, 0xdfff) AM_RAM_WRITE(dribling_colorram_w) AM_BASE(&colorram)
ADDRESS_MAP_END


static ADDRESS_MAP_START( io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0xff) AM_READWRITE(ioread, iowrite)
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( dribling )
	PORT_START("MUX0")	/* IN0 (mux 0) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(1)

	PORT_START("MUX1")	/* IN0 (mux 1) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(2)

	PORT_START("MUX2")	/* IN0 (mux 2) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN0")	/* IN0 */
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( dribling )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, 5000000)
	MDRV_CPU_PROGRAM_MAP(dribling_map,0)
	MDRV_CPU_IO_MAP(io_map,0)
	MDRV_CPU_VBLANK_INT("screen", dribling_irq_gen)

	MDRV_PPI8255_ADD( "ppi8255_0", ppi8255_intf[0] )
	MDRV_PPI8255_ADD( "ppi8255_1", ppi8255_intf[1] )

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_UPDATE_BEFORE_VBLANK)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 255, 40, 255)

	MDRV_PALETTE_LENGTH(256)

	MDRV_PALETTE_INIT(dribling)
	MDRV_VIDEO_UPDATE(dribling)

	/* sound hardware */
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( dribling )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "5p.bin",  0x0000, 0x1000, CRC(0e791947) SHA1(57bc4f4e9e1fe3fbac1017601c9c75029b2601a4) )
	ROM_LOAD( "5n.bin",  0x1000, 0x1000, CRC(bd0f223a) SHA1(f9fbc5670a8723c091d61012e545774d315eb18f) ) //
	ROM_LOAD( "5l.bin",  0x4000, 0x1000, CRC(1fccfc85) SHA1(c0365ad54144414218f52209173b858b927c9626) )
	ROM_LOAD( "5k.bin",  0x5000, 0x1000, CRC(737628c4) SHA1(301fda413388c26da5b5150aec2cefc971801749) ) //
	ROM_LOAD( "5h.bin",  0x6000, 0x1000, CRC(30d0957f) SHA1(52135e12094ee1c8828a48c355bdd565aa5895de) ) //

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "3p.bin",  0x0000, 0x1000, CRC(208971b8) SHA1(f91f3ea04d75beb58a61c844472b4dba53d84c0f) )
	ROM_LOAD( "3n.bin",  0x1000, 0x1000, CRC(356c9803) SHA1(8e2ce52f32b33886f4747dadf3aeb78148538173) )

	ROM_REGION( 0x600, "proms", 0 )
	ROM_LOAD( "prom_3c.bin", 0x0000, 0x0400, CRC(25f068de) SHA1(ea4c56c47fe8153069acb9df80df0b099f3b81f1) )
	ROM_LOAD( "prom_3e.bin", 0x0400, 0x0100, CRC(73eba798) SHA1(7be0e253624df53092e26c28eb18afdcf71434aa) )
	ROM_LOAD( "prom_2d.bin", 0x0500, 0x0100, CRC(5d8c57c6) SHA1(abfb54812d66a36e797be47653dadda4843e8a90) )
ROM_END


ROM_START( driblino )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "5p.bin",       0x0000, 0x1000, CRC(0e791947) SHA1(57bc4f4e9e1fe3fbac1017601c9c75029b2601a4) )
	ROM_LOAD( "dribblng.5n",  0x1000, 0x1000, CRC(5271e620) SHA1(ebed8e31057bb8492840a6e3b8bc453f7cb67243) )
	ROM_LOAD( "5l.bin",       0x4000, 0x1000, CRC(1fccfc85) SHA1(c0365ad54144414218f52209173b858b927c9626) )
	ROM_LOAD( "dribblng.5j",  0x5000, 0x1000, CRC(e535ac5b) SHA1(ba13298378f1e5b2b40634874097ad29c402fdea) )
	ROM_LOAD( "dribblng.5h",  0x6000, 0x1000, CRC(e6af7264) SHA1(a015120d85461e599c4bb9626ebea296386a31bb) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "3p.bin",  0x0000, 0x1000, CRC(208971b8) SHA1(f91f3ea04d75beb58a61c844472b4dba53d84c0f) )
	ROM_LOAD( "3n.bin",  0x1000, 0x1000, CRC(356c9803) SHA1(8e2ce52f32b33886f4747dadf3aeb78148538173) )

	ROM_REGION( 0x600, "proms", 0 )
	ROM_LOAD( "prom_3c.bin", 0x0000, 0x0400, CRC(25f068de) SHA1(ea4c56c47fe8153069acb9df80df0b099f3b81f1) )
	ROM_LOAD( "prom_3e.bin", 0x0400, 0x0100, CRC(73eba798) SHA1(7be0e253624df53092e26c28eb18afdcf71434aa) )
	ROM_LOAD( "prom_2d.bin", 0x0500, 0x0100, CRC(5d8c57c6) SHA1(abfb54812d66a36e797be47653dadda4843e8a90) )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1983, dribling, 0,        dribling, dribling, 0, ROT0, "Model Racing", "Dribbling", GAME_NO_SOUND )
GAME( 1983, driblino, dribling, dribling, dribling, 0, ROT0, "Model Racing (Olympia license)", "Dribbling (Olympia)", GAME_NO_SOUND )
