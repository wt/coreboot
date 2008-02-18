/*
 * This file is part of the coreboot project.
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

#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <arch/io.h>
#include "i82801db.h"
#include "i82801db_smbus.h"

static int lsmbus_read_byte(struct bus *bus, device_t dev, uint8_t address)
{
	unsigned device;
	struct resource *res;

	device = dev->path.u.i2c.device;
	res = find_resource(bus->dev, 0x20);
	
	return do_smbus_read_byte(res->base, device, address);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte  = lsmbus_read_byte,
};
static struct pci_operations lops_pci = {
	/* The subsystem id follows the ide controller */
	.set_subsystem = 0,
};
static struct device_operations smbus_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = scan_static_bus,
	.enable           = i82801db_enable,
	.ops_pci          = &lops_pci,
	.ops_smbus_bus    = &lops_smbus_bus,
};

static const struct pci_driver smbus_driver __pci_driver = {
	.ops    = &smbus_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_SMB,
};
