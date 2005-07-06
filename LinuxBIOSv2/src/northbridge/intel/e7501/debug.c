/*
 * generic K8 debug code, used by mainboard specific auto.c
 *
 */
#if 1
static void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 16) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 11) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 8) & 7);
}

static void print_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0); 
		dev <= PCI_DEV(0xff, 0x1f, 0x7); 
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		print_debug("\r\n");
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	
	for(i = 0; i < 256; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
#if CONFIG_USE_INIT
                        printk_debug("\r\n%02x:",i);
#else
			print_debug("\r\n");
			print_debug_hex8(i);
			print_debug_char(':');
#endif
		}
		val = pci_read_config8(dev, i);
#if CONFIG_USE_INIT
		printk_debug(" %02x", val);
#else
		print_debug_char(' ');
		print_debug_hex8(val);
#endif
	}
	print_debug("\r\n");
}

static void dump_pci_devices(void)
{
	device_t dev;
	for(dev = PCI_DEV(0, 0, 0); 
		dev <= PCI_DEV(0xff, 0x1f, 0x7); 
		dev += PCI_DEV(0,0,1)) {
		uint32_t id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0xffff) ||
			(((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}

static void dump_pci_devices_on_bus(unsigned busn)
{
        device_t dev;
        for(dev = PCI_DEV(busn, 0, 0);
                dev <= PCI_DEV(busn, 0x1f, 0x7);
                dev += PCI_DEV(0,0,1)) {
                uint32_t id;
                id = pci_read_config32(dev, PCI_VENDOR_ID);
                if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0xffff) ||
                        (((id >> 16) & 0xffff) == 0x0000)) {
                        continue;
                }
                dump_pci_device(dev);
        }
}

static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\r\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
#if CONFIG_USE_INIT
			printk_debug("dimm: %02x.0: %02x", i, device);
#else
			print_debug("dimm: "); 
			print_debug_hex8(i); 
			print_debug(".0: ");
			print_debug_hex8(device);
#endif
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
#if CONFIG_USE_INIT
					printk_debug("\r\n%02x: ", j);
#else
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
#endif
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
#if CONFIG_USE_INIT
				printk_debug("%02x ", byte);
#else
				print_debug_hex8(byte);
				print_debug_char(' ');
#endif
			}
			print_debug("\r\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
#if CONFIG_USE_INIT
                        printk_debug("dimm: %02x.1: %02x", i, device);
#else`
			print_debug("dimm: "); 
			print_debug_hex8(i); 
			print_debug(".1: ");
			print_debug_hex8(device);
#endif
			for(j = 0; j < 128; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
#if CONFIG_USE_INIT
                                        printk_debug("\r\n%02x: ", j);
#else
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
#endif
				}
				status = smbus_read_byte(device, j);
				if (status < 0) {
					break;
				}
				byte = status & 0xff;
#if CONFIG_USE_INIT
                                printk_debug("%02x ", byte);
#else
				print_debug_hex8(byte);
				print_debug_char(' ');
#endif
			}
			print_debug("\r\n");
		}
	}
}
static void dump_smbus_registers(void)
{
	unsigned device;
        print_debug("\r\n");
        for(device = 1; device < 0x80; device++) {
                int j;
		if( smbus_read_byte(device, 0) < 0 ) continue;
#if CONFIG_USE_INIT
		printk_debug("smbus: %02x", device);
#else
                print_debug("smbus: ");
                print_debug_hex8(device);
#endif
                for(j = 0; j < 256; j++) {
                	int status; 
                        unsigned char byte;
                        status = smbus_read_byte(device, j);
                        if (status < 0) {
				break;
                        }
                        if ((j & 0xf) == 0) {
#if CONFIG_USE_INIT
				printk_debug("\r\n%02x: ",j);
#else
                	        print_debug("\r\n");
                                print_debug_hex8(j);
                                print_debug(": ");
#endif
                        }
                        byte = status & 0xff;
#if CONFIG_USE_INIT
                        printk_debug("%02x ", byte);
#else
                        print_debug_hex8(byte);
                        print_debug_char(' ');
#endif
                }
                print_debug("\r\n");
	}	
}

static void dump_io_resources(unsigned port) 
{

	int i;
#if CONFIG_USE_INIT
	printk_debug("%04x:\r\n", port);
#else
        print_debug_hex16(port);
        print_debug(":\r\n");
#endif
        for(i=0;i<256;i++) {
                uint8_t val;
                if ((i & 0x0f) == 0) {
#if CONFIG_USE_INIT
			printk_debug("%02x:", i);
#else
                        print_debug_hex8(i);
                        print_debug_char(':');
#endif
                }
                val = inb(port);
#if CONFIG_USE_INIT
		printk_debug(" %02x",val);
#else
                print_debug_char(' ');
                print_debug_hex8(val);
#endif
                if ((i & 0x0f) == 0x0f) {
                        print_debug("\r\n");
                }
		port++;
        }
}

static void dump_mem(unsigned start, unsigned end)
{
        unsigned i;
	print_debug("dump_mem:");
        for(i=start;i<end;i++) {
		if((i & 0xf)==0) {
#if CONFIG_USE_INIT
			printk_debug("\r\n%08x:", i);
#else	
			print_debug("\r\n");
			print_debug_hex32(i);
			print_debug(":");
#endif
		}
#if CONFIG_USE_INIT	
		printk_debug(" %02x", (unsigned char)*((unsigned char *)i));
#else
		print_debug(" ");
             	print_debug_hex8((unsigned char)*((unsigned char *)i));
#endif
        }
        print_debug("\r\n");
 }
#endif
