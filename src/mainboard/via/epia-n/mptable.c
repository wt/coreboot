/* generated by MPTable, version 2.0.15*/
/* as modified by RGM for coreboot */
#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "COREBOOT";
        static const char productid[12] = "P4DPE       ";
        struct mp_config_table *mc;
        int isa_bus;

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
        mptable_write_buses(mc, NULL, &isa_bus);

/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, 2, 0x20, IO_APIC_ADDR);
	{
		device_t dev;
		struct resource *res;
		dev = dev_find_slot(1, PCI_DEVFN(0x1e,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 3, 0x20, res->base);
			}
		}
		dev = dev_find_slot(1, PCI_DEVFN(0x1c,0));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 4, 0x20, res->base);
			}
		}
                dev = dev_find_slot(4, PCI_DEVFN(0x1e,0));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 5, 0x20, res->base);
			}
                }
                dev = dev_find_slot(4, PCI_DEVFN(0x1c,0));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 8, 0x20, res->base);
			}
                }
	}
	mptable_add_isa_interrupts(mc, isa_bus, 0x2, 0);

/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x40, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x40, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x41, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x41, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x42, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x46, 0x2, 0x16);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x1, 0x0, 0x2, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x48, 0x2, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x3d, 0x2, 0x14);
/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, 0x0, 0x0, MP_APIC_ALL, 0x1);
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
