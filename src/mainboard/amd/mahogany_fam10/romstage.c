/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

//#define SYSTEM_TYPE 0	/* SERVER */
#define SYSTEM_TYPE 1	/* DESKTOP */
//#define SYSTEM_TYPE 2	/* MOBILE */

#define RAMINIT_SYSINFO 1

#define SET_NB_CFG_54 1

//used by raminit
#define QRANK_DIMM_SUPPORT 1

//used by incoherent_ht
#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 0

//used by init_cpus and fidvid
#define SET_FIDVID 1
#define SET_FIDVID_CORE_RANGE 0

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include "lib/ramtest.c"
#include <cpu/amd/model_10xxx_rev.h>
#include "northbridge/amd/amdfam10/raminit.h"
#include "northbridge/amd/amdfam10/amdfam10.h"

#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdfam10/reset_test.c"

#include <console/loglevel.h>
#include "cpu/x86/bist.h"

static int smbus_read_byte(u32 device, u32 address);

#include "superio/ite/it8718f/it8718f_early_serial.c"

#if CONFIG_USBDEBUG
#include "southbridge/amd/sb700/sb700_enable_usbdebug.c"
#include "pc80/usbdebug_serial.c"
#endif

#include "cpu/x86/mtrr/earlymtrr.c"
#include <cpu/amd/mtrr.h>
#include "northbridge/amd/amdfam10/setup_resource_map.c"

#include "southbridge/amd/rs780/rs780_early_setup.c"
#include "southbridge/amd/sb700/sb700_early_setup.c"
#include "northbridge/amd/amdfam10/debug.c"

static void activate_spd_rom(const struct mem_controller *ctrl)
{
}

static int spd_read_byte(u32 device, u32 address)
{
	int result;
	result = smbus_read_byte(device, address);
	return result;
}

#include "northbridge/amd/amdfam10/amdfam10.h"

#include "northbridge/amd/amdfam10/raminit_sysinfo_in_ram.c"
#include "northbridge/amd/amdfam10/amdfam10_pci.c"

#include "resourcemap.c"
#include "cpu/amd/quadcore/quadcore.c"

#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/microcode/microcode.c"
#include "cpu/amd/model_10xxx/update_microcode.c"
#include "cpu/amd/model_10xxx/init_cpus.c"

#include "northbridge/amd/amdfam10/early_ht.c"
#include "southbridge/amd/sb700/sb700_early_setup.c"

//#include "spd_addr.h"

#define RC00  0
#define RC01  1

#define DIMM0 0x50
#define DIMM1 0x51
#define DIMM2 0x52
#define DIMM3 0x53

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{

	struct sys_info *sysinfo = (struct sys_info *)(CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);
	static const u8 spd_addr[] = {RC00, DIMM0, DIMM2, 0, 0, DIMM1, DIMM3, 0, 0, };
	u32 bsp_apicid = 0;
	u32 val;
	msr_t msr;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		/* mov bsp to bus 0xff when > 8 nodes */
		set_bsp_node_CHtExtNodeCfgEn();
		enumerate_ht_chain();

		sb700_pci_port80();
	}

	post_code(0x30);

	if (bist == 0) {
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo); /* mmconf is inited in init_cpus */
		/* All cores run this but the BSP(node0,core0) is the only core that returns. */
	}

	post_code(0x32);

	enable_rs780_dev8();
	sb700_lpc_init();

	it8718f_enable_serial(0, CONFIG_TTYS0_BASE);
	uart_init();

#if CONFIG_USBDEBUG
	sb700_enable_usbdebug(0);
	early_usbdebug_init();
#endif

	console_init();
	printk(BIOS_DEBUG, "\n");

//	dump_mem(CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x200, CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	// Load MPB
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "*sysinfo range: [%p,%p]\n",sysinfo,sysinfo+1);
	printk(BIOS_DEBUG, "bsp_apicid = %02x \n", bsp_apicid);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

	/* Setup sysinfo defaults */
	set_sysinfo_in_ram(0);

	update_microcode(val);
	post_code(0x33);

	cpuSetAMDMSR();
	post_code(0x34);

	amd_ht_init(sysinfo);
	post_code(0x35);

	/* Setup nodes PCI space and start core 0 AP init. */
	finalize_node_setup(sysinfo);

	/* Setup any mainboard PCI settings etc. */
	setup_mb_resource_map();
	post_code(0x36);

	/* wait for all the APs core0 started by finalize_node_setup. */
	/* FIXME: A bunch of cores are going to start output to serial at once.
	   It would be nice to fixup prink spinlocks for ROM XIP mode.
	   I think it could be done by putting the spinlock flag in the cache
	   of the BSP located right after sysinfo.
	 */
	wait_all_core0_started();

 #if CONFIG_LOGICAL_CPUS==1
	/* Core0 on each node is configured. Now setup any additional cores. */
	printk(BIOS_DEBUG, "start_other_cores()\n");
	start_other_cores();
	post_code(0x37);
	wait_all_other_cores_started(bsp_apicid);
 #endif

	post_code(0x38);

	/* run _early_setup before soft-reset. */
	rs780_early_setup();
	sb700_early_setup();

 #if SET_FIDVID == 1
	msr = rdmsr(0xc0010071);
	printk(BIOS_DEBUG, "\nBegin FIDVID MSR 0xc0010071 0x%08x 0x%08x \n", msr.hi, msr.lo);

	/* FIXME: The sb fid change may survive the warm reset and only
	   need to be done once.*/
	enable_fid_change_on_sb(sysinfo->sbbusn, sysinfo->sbdn);

	post_code(0x39);

	if (!warm_reset_detect(0)) {			// BSP is node 0
		init_fidvid_bsp(bsp_apicid, sysinfo->nodes);
	} else {
		init_fidvid_stage2(bsp_apicid, 0);	// BSP is node 0
	}

	post_code(0x3A);

	/* show final fid and vid */
	msr=rdmsr(0xc0010071);
	printk(BIOS_DEBUG, "End FIDVIDMSR 0xc0010071 0x%08x 0x%08x \n", msr.hi, msr.lo);
 #endif

	rs780_htinit();

	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		print_info("...WARM RESET...\n\n\n");
		soft_reset();
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}

	post_code(0x3B);

	/* It's the time to set ctrl in sysinfo now; */
	printk(BIOS_DEBUG, "fill_mem_ctrl()\n");
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);

	post_code(0x40);

//	die("Die Before MCT init.");

	printk(BIOS_DEBUG, "raminit_amdmct()\n");
	raminit_amdmct(sysinfo);
	post_code(0x41);

/*
	dump_pci_device_range(PCI_DEV(0, 0x18, 0), 0, 0x200);
	dump_pci_device_range(PCI_DEV(0, 0x18, 1), 0, 0x200);
	dump_pci_device_range(PCI_DEV(0, 0x18, 2), 0, 0x200);
	dump_pci_device_range(PCI_DEV(0, 0x18, 3), 0, 0x200);
*/

//	ram_check(0x00200000, 0x00200000 + (640 * 1024));
//	ram_check(0x40200000, 0x40200000 + (640 * 1024));

//	die("After MCT init before CAR disabled.");

	rs780_before_pci_init();
	sb700_before_pci_init();

	post_code(0x42);
	printk(BIOS_DEBUG, "\n*** Yes, the copy/decompress is taking a while, FIXME!\n");
	post_cache_as_ram();	// BSP switch stack to ram, copy then execute LB.
	post_code(0x43);	// Should never see this post code.
}

