#ifndef DEVICE_PCIEXP_H
#define DEVICE_PCIEXP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

unsigned int pciexp_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max);
unsigned int pciexp_scan_bridge(device_t dev, unsigned int max);

extern struct device_operations default_pciexp_ops_bus;

#endif /* DEVICE_PCIEXP_H */
