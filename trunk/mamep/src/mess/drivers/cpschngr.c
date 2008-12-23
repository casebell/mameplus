/***************************************************************************

  Capcom System 1
  ===============

  Driver provided by:
  Paul Leaman (paul@vortexcomputing.demon.co.uk)

  M680000 for game, Z80, YM-2151 and OKIM6295 for sound.

  68000 clock speeds are unknown for all games (except where commented)

merged Street Fighter Zero for MESS

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"

#ifdef GAME
#undef GAME
#endif

#define GAME(YEAR,NAME,PARENT,MACHINE,INPUT,INIT,MONITOR,COMPANY,FULLNAME,FLAGS)
MACHINE_DRIVER_EXTERN(cps1_10MHz);
extern void driver_init_cps1(running_machine *machine);

#include "cps1.h"

#if 0
void wof_decode(running_machine *machine)      { }
void dino_decode(running_machine *machine)     { }
void punisher_decode(running_machine *machine) { }
void slammast_decode(running_machine *machine) { }

void forogttn_dummy_function(running_machine *machine)
{
	/* Forgotten Worlds has extra inputs on the B-board CN-MOWS connector for the dial controls. */
	memory_install_write16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x800040, 0x800041, 0, 0, forgottn_dial_0_reset_w);
	memory_install_write16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x800048, 0x800049, 0, 0, forgottn_dial_1_reset_w);
	memory_install_read16_handler (cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x800052, 0x800055, 0, 0, forgottn_dial_0_r);
	memory_install_read16_handler (cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x80005a, 0x80005d, 0, 0, forgottn_dial_1_r);
	cps1_hack_dsw_r(cputag_get_address_space(machine,"main",ADDRESS_SPACE_PROGRAM), 0, 0);
	driver_init_sf2mdt(machine);
}
#endif

/***************************************************************************

  Game driver(s)

***************************************************************************/

#define CODE_SIZE 0x400000

static INPUT_PORTS_START( sfzch )
	PORT_START("IN0")     /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON5) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON5) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE) PORT_NAME(DEF_STR(Pause)) PORT_CODE(KEYCODE_F1)	/* pause */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE  )	/* pause */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON6) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON6) PORT_PLAYER(2)

	PORT_START("DSWA")
	PORT_DIPNAME( 0xff, 0xff, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0xff, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSWB")
	PORT_DIPNAME( 0xff, 0xff, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0xff, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSWC")
	PORT_DIPNAME( 0xff, 0xff, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0xff, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")     /* Player 1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4) PORT_PLAYER(1)

	PORT_START("IN2")      /* Player 2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4) PORT_PLAYER(2)
INPUT_PORTS_END

ROM_START( sfzch )
	ROM_REGION( CODE_SIZE, "main",0 )      /* 68000 code */
	ROM_LOAD16_WORD_SWAP( "sfzch23",        0x000000, 0x80000, CRC(1140743f) SHA1(10bcedb5cca266f2aa3ed99ede6f9a64fc877539))
	ROM_LOAD16_WORD_SWAP( "sfza22",         0x080000, 0x80000, CRC(8d9b2480) SHA1(405305c1572908d00eab735f28676fbbadb4fac6))
	ROM_LOAD16_WORD_SWAP( "sfzch21",        0x100000, 0x80000, CRC(5435225d) SHA1(6b1156fd82d0710e244ede39faaae0847c598376))
	ROM_LOAD16_WORD_SWAP( "sfza20",         0x180000, 0x80000, CRC(806e8f38) SHA1(b6d6912aa8f2f590335d7ff9a8214648e7131ebb))

	ROM_REGION( 0x800000, "gfx", 0 )
	ROMX_LOAD( "sfz01",         0x000000, 0x80000, CRC(0dd53e62) SHA1(5f3bcf5ca0fd564d115fe5075a4163d3ee3226df), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz02",         0x000002, 0x80000, CRC(94c31e3f) SHA1(2187b3d4977514f2ae486eb33ed76c86121d5745), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz03",         0x000004, 0x80000, CRC(9584ac85) SHA1(bbd62d66b0f6909630e801ce5d6331d43f44d741), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz04",         0x000006, 0x80000, CRC(b983624c) SHA1(841106bb9453e3dfb7869c4b0e9149cc610d515a), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz05",         0x200000, 0x80000, CRC(2b47b645) SHA1(bc6426eff5df9417f32666586744626fa544f7b5), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz06",         0x200002, 0x80000, CRC(74fd9fb1) SHA1(7945472591f3c06970e96611a0363ed8f3d52c36), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz07",         0x200004, 0x80000, CRC(bb2c734d) SHA1(97a06935f86f31755d2ffdc5b56bef53944bdecd), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz08",         0x200006, 0x80000, CRC(454f7868) SHA1(eecccba7542d893bc41676246a20aa4914b79bbc), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz10",         0x400000, 0x80000, CRC(2a7d675e) SHA1(0144ba34a29fb08b41c780ce65bb06d25724e88f), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz11",         0x400002, 0x80000, CRC(e35546c8) SHA1(7b08aa3413494d12c5c550263a5f00b64b98e6ab), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz12",         0x400004, 0x80000, CRC(f122693a) SHA1(71ce901d8d30207e506b6a8d6a4e0fcf3a1b0eac), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz13",         0x400006, 0x80000, CRC(7cf942c8) SHA1(a7109facb97a8a11ddf1b4e07de6ff3164d713a1), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz14",         0x600000, 0x80000, CRC(09038c81) SHA1(3461d70902fbfb92ce40f804be6388276a01d153), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz15",         0x600002, 0x80000, CRC(1aa17391) SHA1(b4d0f760a430b7fc4443b6c94da2659315c5b926), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz16",         0x600004, 0x80000, CRC(19a5abd6) SHA1(73ba1de15c883fdc69fd7dccdb58d00ca512d4ea), ROM_GROUPWORD | ROM_SKIP(6) )
	ROMX_LOAD( "sfz17",         0x600006, 0x80000, CRC(248b3b73) SHA1(95810a17b1caf6372b33ed3e4ee8a7e51482c70d), ROM_GROUPWORD | ROM_SKIP(6) )

	ROM_REGION( 0x8000, "stars", 0 )
	ROM_COPY( "gfx", 0x000000, 0x000000, 0x8000 )

	ROM_REGION( 0x28000, "audio",0 ) /* 64k for the audio CPU (+banks) */
	ROM_LOAD( "sfz09",         0x00000, 0x08000, CRC(c772628b) SHA1(ebc5b7c173caf1e151f733f23c1b20abec24e16d))
	ROM_CONTINUE(              0x10000, 0x08000 )

	ROM_REGION( 0x40000, "oki",0 )	/* Samples */
	ROM_LOAD( "sfz18",         0x00000, 0x20000, CRC(61022b2d) SHA1(6369d0c1d08a30ee19b94e52ab1463a7784b9de5))
	ROM_LOAD( "sfz19",         0x20000, 0x20000, CRC(3b5886d5) SHA1(7e1b7d40ef77b5df628dd663d45a9a13c742cf58))
ROM_END

//mamep: tag to identify a console system
SYSTEM_CONFIG_START(cpschngr)
SYSTEM_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME      PARENT    COMPAT  MACHINE   INPUT     INIT    CONFIG  COMPANY     FULLNAME */
CONS( 1995, sfzch,    0,        0,		cps1_10MHz,	  sfzch,    cps1,	cpschngr,	"Capcom", "CPS Changer (Street Fighter Zero)" , 0)
