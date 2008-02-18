/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2003 SuSE Linux AG
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

/* From 82801DBM, needs to be fixed to support everything the 82801ER does. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "i82801xx.h"

#define PMBASE_ADDR	0x00000400 /* ACPI Base Address Register */
#define GPIO_BASE_ADDR	0x00000500 /* GPIO Base Address Register */

#define NMI_OFF 0

/* PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

#define PIRQA 0x03
#define PIRQB 0x05
#define PIRQC 0x06
#define PIRQD 0x07
#define PIRQE 0x09
#define PIRQF 0x0A
#define PIRQG 0x0B
#define PIRQH 0x0C

void i82801xx_enable_apic(struct device *dev)
{
	uint32_t reg32;
	volatile uint32_t *ioapic_index = (volatile uint32_t *)0xfec00000;
	volatile uint32_t *ioapic_data = (volatile uint32_t *)0xfec00010;

	/* Set ACPI base address (I/O space). */
	pci_write_config32(dev, PMBASE, (PMBASE_ADDR | 1));

	/* Enable ACPI I/O and power management. */
	pci_write_config8(dev, ACPI_CNTL, 0x10);

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (3 << 7);	/* Enable IOAPIC */
	reg32 |= (1 << 13);	/* Coprocessor error enable */
	reg32 |= (1 << 1);	/* Delayed transaction enable */
	reg32 |= (1 << 2);	/* DMA collection buffer enable */
	pci_write_config32(dev, GEN_CNTL, reg32);
	printk_debug("IOAPIC Southbridge enabled %x\n", reg32);

	*ioapic_index = 0;
	*ioapic_data = (1 << 25);

	*ioapic_index = 0;
	reg32 = *ioapic_data;
	printk_debug("Southbridge APIC ID = %x\n", reg32);
	if (reg32 != (1 << 25))
		die("APIC Error\n");

	/* TODO: From i82801ca, needed/useful on other ICH? */
	*ioapic_index = 3; /* Select Boot Configuration register. */
	*ioapic_data = 1; /* Use Processor System Bus to deliver interrupts. */
}

void i82801xx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
	/* TODO: Explain/#define the real meaning of these magic numbers. */
}

static void i82801xx_pirq_init(device_t dev, uint16_t ich_model)
{
	/* Route PIRQA - PIRQD. */
	pci_write_config8(dev, PIRQA_ROUT, PIRQA);
	pci_write_config8(dev, PIRQB_ROUT, PIRQB);
	pci_write_config8(dev, PIRQC_ROUT, PIRQC);
	pci_write_config8(dev, PIRQD_ROUT, PIRQD);

	/* Route PIRQE - PIRQH (for ICH2-ICH9). */
	if (ich_model >= 0x2440) {
		pci_write_config8(dev, PIRQE_ROUT, PIRQE);
		pci_write_config8(dev, PIRQF_ROUT, PIRQF);
		pci_write_config8(dev, PIRQG_ROUT, PIRQG);
		pci_write_config8(dev, PIRQH_ROUT, PIRQH);
	}
}

static void i82801xx_power_options(device_t dev)
{
	uint8_t byte;
	int pwr_on = -1;
	int nmi_option;

	/* power after power fail */
	/* FIXME this doesn't work! */
	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
	pci_write_config8(dev, GEN_PMCON_3, pwr_on ? 0 : 1);
	printk_info("Set power %s if power fails\n", pwr_on ? "on" : "off");

	/* Set up NMI on errors. */
	byte = inb(0x61);
	byte &= ~(1 << 3);	/* IOCHK# NMI Enable */
	byte &= ~(1 << 2);	/* PCI SERR# Enable */
	outb(byte, 0x61);
	byte = inb(0x70);

	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7);	/* Set NMI. */
		outb(byte, 0x70);
	}
}

static void gpio_init(device_t dev, uint16_t ich_model)
{
	/* Set the value for GPIO base address register and enable GPIO.
	 * Note: ICH-ICH5 registers differ from ICH6-ICH9.
	 */
	if (ich_model <= 0x24D0) {
		pci_write_config32(dev, GPIO_BASE_ICH0_5, (GPIO_BASE_ADDR | 1));
		pci_write_config8(dev, GPIO_CNTL_ICH0_5, 0x10);
	} else if (ich_model >= 0x2640) {
		pci_write_config32(dev, GPIO_BASE_ICH6_9, (GPIO_BASE_ADDR | 1));
		pci_write_config8(dev, GPIO_CNTL_ICH6_9, 0x10);
	}
}

void i82801xx_rtc_init(struct device *dev)
{
	uint8_t reg8;
	uint32_t reg32;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~(1 << 1);	/* Preserve the power fail state. */
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	reg32 = pci_read_config32(dev, GEN_STS);
	rtc_failed |= reg32 & (1 << 2);
	rtc_init(rtc_failed);

	/* Enable access to the upper 128 byte bank of CMOS RAM. */
	pci_write_config8(dev, RTC_CONF, 0x04);
}

void i82801xx_lpc_route_dma(struct device *dev, uint8_t mask)
{
	uint16_t reg16;
	int i;

	reg16 = pci_read_config16(dev, PCI_DMA_CFG);
	reg16 &= 0x300;
	for (i = 0; i < 8; i++) {
		if (i == 4)
			continue;
		reg16 |= ((mask & (1 << i)) ? 3 : 1) << (i * 2);
	}
	pci_write_config16(dev, PCI_DMA_CFG, reg16);
}

static void i82801xx_lpc_decode_en(device_t dev, uint16_t ich_model)
{
	/* Decode 0x3F8-0x3FF (COM1) for COMA port, 0x2F8-0x2FF (COM2) for COMB.
	 * LPT decode defaults to 0x378-0x37F and 0x778-0x77F.
	 * Floppy decode defaults to 0x3F0-0x3F5, 0x3F7.
	 * We also need to set the value for LPC I/F Enables Register.
	 * Note: ICH-ICH5 registers differ from ICH6-ICH9.
	 */
	if (ich_model <= 0x24D0) {
		pci_write_config8(dev, COM_DEC, 0x10);
		pci_write_config16(dev, LPC_EN_ICH0_5, 0x300F);
	} else if (ich_model >= 0x2640) {
		pci_write_config8(dev, LPC_IO_DEC, 0x10);
		pci_write_config16(dev, LPC_EN_ICH6_9, 0x300F);
	}
}

static void enable_hpet(struct device *dev)
{
#ifdef HPET_PRESENT
	uint32_t reg32;
	uint32_t code = (0 & 0x3);

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (1 << 17);	/* Enable HPET. */
	/*
	 * Bits [16:15]	Memory Address Range
	 * 00		FED0_0000h - FED0_03FFh
	 * 01		FED0_1000h - FED0_13FFh
	 * 10		FED0_2000h - FED0_23FFh
	 * 11		FED0_3000h - FED0_33FFh
	 */
	reg32 &= ~(3 << 15);	/* Clear it */
	reg32 |= (code << 15);
	/* TODO: reg32 is never written to anywhere? */
	printk_debug("Enabling HPET @0x%x\n", HPET_ADDR | (code << 12));
#endif
}

static void lpc_init(struct device *dev)
{
	uint16_t ich_model = pci_read_config16(dev, PCI_DEVICE_ID);

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	i82801xx_enable_apic(dev);

	i82801xx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801xx_pirq_init(dev, ich_model);

	/* Setup power options. */
	i82801xx_power_options(dev);

	/* Set the state of the GPIO lines. */
	gpio_init(dev, ich_model);

	/* Initialize the real time clock. */
	i82801xx_rtc_init(dev);

	/* Route DMA. */
	i82801xx_lpc_route_dma(dev, 0xff);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Setup decode ports and LPC I/F enables. */
	i82801xx_lpc_decode_en(dev, ich_model);

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);
}

static void i82801xx_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void i82801xx_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations lpc_ops = {
	.read_resources		= i82801xx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= i82801xx_lpc_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.enable			= i82801xx_enable,
};

static const struct pci_driver i82801aa_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2410,
};

static const struct pci_driver i82801ab_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2420,
};

static const struct pci_driver i82801ba_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2440,
};

static const struct pci_driver i82801ca_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2480,
};

static const struct pci_driver i82801db_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24c0,
};

static const struct pci_driver i82801dbm_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24cc,
};

/* 82801EB and 82801ER */
static const struct pci_driver i82801ex_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24d0,
};