/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 *
 * Copyright (C) 2006 MSI
 * Written by Bingxun Shi <bingxunshi@gmail.com> for MSI.
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

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdk8_sysconf.h>

#include "mb_sysconf.h"



static void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "COREBOOT";
        static const char productid[12] = "MS9282      ";
        struct mp_config_table *mc;
	struct mb_sysconf_t *m;
	unsigned sbdn;

	int i,j;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
        memset(mc, 0, sizeof(*mc));

        memcpy(mc->mpc_signature, sig, sizeof(sig));
        mc->mpc_length = sizeof(*mc); /* initially just the header */
        mc->mpc_spec = 0x04;
        mc->mpc_checksum = 0; /* not yet computed */
        memcpy(mc->mpc_oem, oem, sizeof(oem));
        memcpy(mc->mpc_productid, productid, sizeof(productid));
        mc->mpc_oemptr = 0;
        mc->mpc_oemsize = 0;
        mc->mpc_entry_count = 0; /* No entries yet... */
        mc->mpc_lapic = LAPIC_ADDR;
        mc->mpe_length = 0;
        mc->mpe_checksum = 0;
        mc->reserved = 0;

        smp_write_processors(mc);

       get_bus_conf();
       sbdn = sysconf.sbdn;
       m = sysconf.mb;

/*Bus:         Bus ID  Type*/
       /* define bus and isa numbers */
        for(j= 0; j < 256 ; j++) {
               if(m->bus_type[j])
                        smp_write_bus(mc, j, "PCI   ");
        }
        smp_write_bus(mc, m->bus_isa, "ISA   ");

/*I/O APICs:   APIC ID Version State           Address*/
        {
                device_t dev;
               struct resource *res;
               uint32_t dword;

                dev = dev_find_slot(m->bus_mcp55[0], PCI_DEVFN(sbdn+ 0x1,0));
                if (dev) {
                       res = find_resource(dev, PCI_BASE_ADDRESS_1);
                       if (res) {
                               smp_write_ioapic(mc, m->apicid_mcp55, 0x11, res->base);
                       }

                       dword = 0x43c6c643;
                       pci_write_config32(dev, 0x7c, dword);

                       dword = 0x81001a00;
                       pci_write_config32(dev, 0x80, dword);

                       dword = 0xd00002d2;
                       pci_write_config32(dev, 0x84, dword);

                }


       }

	mptable_add_isa_interrupts(mc, m->bus_isa, m->apicid_mcp55, 0);

//SMBUS
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+1)<<2)|1, m->apicid_mcp55, 0xa);

//USB1.1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+2)<<2)|0, m->apicid_mcp55, 0x16); // 22

//USB2.0
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+2)<<2)|1, m->apicid_mcp55, 0x17); // 23

//SATA1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|0, m->apicid_mcp55, 0x14); // 20

//SATA2
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|1, m->apicid_mcp55, 0x17); // 23

//SATA3
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+5)<<2)|2, m->apicid_mcp55, 0x15); // 21

//NIC1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+8)<<2)|0, m->apicid_mcp55, 0x16); // 22
//NIC2
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[0], ((sbdn+9)<<2)|0, m->apicid_mcp55, 0x15); // 21

       for(j=7; j>=2; j--) {
               if(!m->bus_mcp55[j]) continue;
               for(i=0;i<4;i++) {
                       smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[j], (0x00<<2)|i, m->apicid_mcp55, 0x10 + (2+j+i+4-sbdn%4)%4);
               }
       }

       for(j=0; j<1; j++)
               for(i=0;i<4;i++) {
                       smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_mcp55[1], ((0x04+j)<<2)|i, m->apicid_mcp55, 0x10 + (2+i+j)%4);
               }

/*Local Ints:  Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN#*/
       smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, m->bus_isa, 0x0, MP_APIC_ALL, 0x0);
       smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, m->bus_isa, 0x0, MP_APIC_ALL, 0x1);
       /* There is no extension information... */

       /* Compute the checksums */
       mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
       mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
       printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
               mc, smp_next_mpe_entry(mc));
       return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
       void *v;
       v = smp_write_floating_table(addr);
       return (unsigned long)smp_write_config_table(v);
}
