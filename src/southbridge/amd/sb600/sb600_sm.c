/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <pc80/mc146818rtc.h>
#include <bitops.h>
#include <arch/io.h>
#include <cpu/x86/lapic.h>
#include <stdlib.h>
#include "sb600.h"
#include "sb600_smbus.c"

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

struct ioapicreg {
	u32 reg;
	u32 value_low, value_high;
};

static struct ioapicreg ioapicregvalues[] = {
#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)
	/* IO-APIC virtual wire mode configuration */
	/* mask, trigger, polarity, destination, delivery, vector */
	{0, DISABLED, NONE},
	{1, DISABLED, NONE},
	{2, DISABLED, NONE},
	{3, DISABLED, NONE},
	{4, DISABLED, NONE},
	{5, DISABLED, NONE},
	{6, DISABLED, NONE},
	{7, DISABLED, NONE},
	{8, DISABLED, NONE},
	{9, DISABLED, NONE},
	{10, DISABLED, NONE},
	{11, DISABLED, NONE},
	{12, DISABLED, NONE},
	{13, DISABLED, NONE},
	{14, DISABLED, NONE},
	{15, DISABLED, NONE},
	{16, DISABLED, NONE},
	{17, DISABLED, NONE},
	{18, DISABLED, NONE},
	{19, DISABLED, NONE},
	{20, DISABLED, NONE},
	{21, DISABLED, NONE},
	{22, DISABLED, NONE},
	{23, DISABLED, NONE},
	/* Be careful and don't write past the end... */
};

static void setup_ioapic(u32 ioapic_base)
{
	int i;
	u32 value_low, value_high;
	volatile u32 *l;
	struct ioapicreg *a = ioapicregvalues;

	ioapicregvalues[0].value_high = lapicid() << (56 - 32);

	printk_debug("lapicid = %016x\n", ioapicregvalues[0].value_high);

	l = (u32 *)ioapic_base;

	for (i = 0; i < ARRAY_SIZE(ioapicregvalues);
	     i++, a++) {
		l[0] = (a->reg * 2) + 0x10;
		l[4] = a->value_low;
		value_low = l[4];
		l[0] = (a->reg * 2) + 0x11;
		l[4] = a->value_high;
		value_high = l[4];
		if ((i == 0) && (value_low == 0xffffffff)) {
			printk_warning("IO APIC not responding.\n");
			return;
		}
	}
}

/*
* SB600 enables all USB controllers by default in SMBUS Control.
* SB600 enables SATA by default in SMBUS Control.
*/
static void sm_init(device_t dev)
{
	u8 byte;
	u8 byte_old;
	u32 dword;
	u32 ioapic_base;
	u32 on;
	u32 nmi_option;

	printk_info("sm_init().\n");

	ioapic_base = pci_read_config32(dev, 0x74) & (0xffffffe0);	/* some like mem resource, but does not have  enable bit */
	setup_ioapic(ioapic_base);

	dword = pci_read_config8(dev, 0x62);
	dword |= 1 << 2;
	pci_write_config8(dev, 0x62, dword);

	dword = pci_read_config32(dev, 0x78);
	dword |= 1 << 9;
	pci_write_config32(dev, 0x78, dword);	/* enable 0xCD6 0xCD7 */

	/* bit 10: MultiMediaTimerIrqEn */
	dword = pci_read_config8(dev, 0x64);
	dword |= 1 << 10;
	pci_write_config8(dev, 0x64, dword);
	/* enable serial irq */
	byte = pci_read_config8(dev, 0x69);
	byte |= 1 << 7;		/* enable serial irq function */
	byte &= ~(0xF << 2);
	byte |= 4 << 2;		/* set NumSerIrqBits=4 */
	pci_write_config8(dev, 0x69, byte);

	byte = pm_ioread(0x61);
	byte |= 1 << 1;		/* Set to enable NB/SB handshake during IOAPIC interrupt for AMD K8/K7 */
	pm_iowrite(0x61, byte);

	/* disable SMI */
	byte = pm_ioread(0x53);
	byte |= 1 << 3;
	pm_iowrite(0x53, byte);

	/* power after power fail */
	on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pm_ioread(0x74);
	byte &= ~0x03;
	if (on) {
		byte |= 2;
	}
	byte |= 1 << 2;
	pm_iowrite(0x74, byte);
	printk_info("set power %s after power fail\n", on ? "on" : "off");

	/* sb600 rpr:2.3.3: */
	byte = pm_ioread(0x9A);
	byte |= 1 << 5 | 1 << 4 | 1 << 2;
	pm_iowrite(0x9A, byte);

	byte = pm_ioread(0x8F);
	byte |= 1 << 5;
	byte &= ~(1 << 4);
	pm_iowrite(0x8F, byte);

	pm_iowrite(0x8B, 0x01);
	pm_iowrite(0x8A, 0x90);
	pm_iowrite(0x88, 0x10);	/* A21 */

	byte = pm_ioread(0x7C);
	byte |= 1 << 0;
	pm_iowrite(0x7C, byte);

	byte = pm_ioread(0x68);
	byte &= ~(1 << 1);
	/* 2.6 */
	byte |= 1 << 2;
	pm_iowrite(0x68, byte);

	/* 2.6 */
	byte = pm_ioread(0x65);
	byte &= ~(1 << 7);
	pm_iowrite(0x65, byte);

	/* 2.3.4 */
	byte = pm_ioread(0x52);
	byte &= ~0x2F;
	byte |= 0x8;
	pm_iowrite(0x52, byte);

	byte = pm_ioread(0x8D);
	byte &= ~(1 << 6);
	pm_iowrite(0x8D, byte);

	byte = pm_ioread(0x61);
	byte &= ~(1 << 2);
	pm_iowrite(0x61, byte);

	byte = pm_ioread(0x42);
	byte &= ~(1 << 2);
	pm_iowrite(0x42, byte);

	/* Set up NMI on errors */
	byte = inb(0x70);	/* RTC70 */
	byte_old = byte;
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7);	/* set NMI */
		printk_info("++++++++++set NMI+++++\n");
	} else {
		byte |= (1 << 7);	/* Can not mask NMI from PCI-E and NMI_NOW */
		printk_info("++++++++++no set NMI+++++\n");
	}
	byte &= ~(1 << 7);
	if (byte != byte_old) {
		outb(byte, 0x70);
	}

	/* 2.10 IO Trap Settings */
	abcfg_reg(0x10090, 1 << 16, 1 << 16);

	/* ab index */
	pci_write_config32(dev, 0xF0, AB_INDX);
	/* Initialize the real time clock */
	rtc_init(0);

	/*3.4 Enabling IDE/PCIB Prefetch for Performance Enhancement */
	abcfg_reg(0x10060, 9 << 17, 9 << 17);
	abcfg_reg(0x10064, 9 << 17, 9 << 17);

	/* 3.5 Enabling OHCI Prefetch for Performance Enhancement */
	abcfg_reg(0x80, 1 << 0, 1<< 0);

	/* 3.6 B-Link Client's Credit Variable Settings for the Downstream Arbitration Equation */
	/* 3.7 Enabling Additional Address Bits Checking in Downstream */
	abcfg_reg(0x9c, 3 << 0, 3 << 0);

	/* 3.8 Set B-Link Prefetch Mode */
	abcfg_reg(0x80, 3 << 17, 3 << 17);

	/* 3.9 Enabling Detection of Upstream Interrupts */
	abcfg_reg(0x94, 1 << 20,1 << 20);

	/* 3.10: Enabling Downstream Posted Transactions to Pass Non-Posted
	 *  Transactions for the K8 Platform (for All Revisions) */
	abcfg_reg(0x10090, 1 << 8, 1 << 8);

	/* 3.11:Programming Cycle Delay for AB and BIF Clock Gating */
	/* 3.12: Enabling AB and BIF Clock Gating */
	abcfg_reg(0x10054, 0xFFFF0000, 0x1040000);
	abcfg_reg(0x54, 0xFF << 16, 4 << 16);
	printk_info("3.11, ABCFG:0x54\n");
	abcfg_reg(0x54, 1 << 24, 1 << 24);
	printk_info("3.12, ABCFG:0x54\n");
	abcfg_reg(0x98, 0x0000FF00, 0x00004700);

	/* 3.13:Enabling AB Int_Arbiter Enhancement (for All Revisions) */
	abcfg_reg(0x10054, 0x0000FFFF, 0x07FF);

	/* 3.14:Enabling L1 on A-link Express */
	axcfg_reg(0x68, 0x00000003, 0x2);
	axindxp_reg(0xa0, 0x0000F000, 0x6000);

	abcfg_reg(0x10098, 0xFFFFFFFF, 0x4000);
	abcfg_reg(0x04, 0xFFFFFFFF, 0x6);
	printk_info("sm_init() end\n");

	/* Enable NbSb virtual channel */
	axcfg_reg(0x114, 0x3f << 1, 0 << 1);
	axcfg_reg(0x120, 0x7f << 1, 0x7f << 1);
	axcfg_reg(0x120, 7 << 24, 1 << 24);
	axcfg_reg(0x120, 1 << 31, 1 << 31);
	abcfg_reg(0x50, 1 << 3, 1 << 3);
}

static int lsmbus_recv_byte(device_t dev)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(device_t dev, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(device_t dev, u8 address)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(device_t dev, u8 address, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_write_byte(res->base, device, address, val);
}
static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte = lsmbus_recv_byte,
	.send_byte = lsmbus_send_byte,
	.read_byte = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static void sb600_sm_read_resources(device_t dev)
{
	struct resource *res;
	u8 byte;

	/* rpr2.14: Hides SM bus controller Bar1 where stores HPET MMIO base address */
	byte = pm_ioread(0x55);
	byte |= 1 << 7;
	pm_iowrite(0x55, byte);

	/* Get the normal pci resources of this device */
	/* pci_dev_read_resources(dev); */

	byte = pm_ioread(0x55);
	byte &= ~(1 << 7);
	pm_iowrite(0x55, byte);

	/* apic */
	res = new_resource(dev, 0x74);
	res->base  = 0xfec00000;
	res->size = 256 * 0x10;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;

	res = new_resource(dev, 0x14); /* hpet */
	res->base  = 0xfed00000;	/* reset hpet to widely accepted address */
	res->size = 0x400;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;
	/* dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER; */

	/* smbus */
	res = new_resource(dev, 0x10);
	res->base  = 0xB00;
	res->size = 0x10;
	res->limit = 0xFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED;


	compact_resources(dev);

}
static void sb600_sm_set_resources(struct device *dev)
{
	struct resource *res;
	u8 byte;

	pci_dev_set_resources(dev);


	/* rpr2.14: Make HPET MMIO decoding controlled by the memory enable bit in command register of LPC ISA bridage */
	byte = pm_ioread(0x52);
	byte |= 1 << 6;
	pm_iowrite(0x52, byte);

	res = find_resource(dev, 0x74);
	pci_write_config32(dev, 0x74, res->base | 1 << 3);

	res = find_resource(dev, 0x14);
	pci_write_config32(dev, 0x14, res->base);

	res = find_resource(dev, 0x10);
	pci_write_config32(dev, 0x10, res->base | 1);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};
static struct device_operations smbus_ops = {
	.read_resources = sb600_sm_read_resources,
	.set_resources = sb600_sm_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_static_bus,
	/* .enable           = sb600_enable, */
	.ops_pci = &lops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};
static struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_SM,
};