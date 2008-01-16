#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <string.h>
#include <serial_subr.h>


static int init_bytes(void)
{
	return 0;
}
static void fini_bytes(void)
{
}
static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	return ttys0_rx_bytes(vdest, count);
}
static byte_offset_t skip_bytes(byte_offset_t count)
{
	int64_t i;
	for(i = 0; i < count; i++) {
		unsigned char byte;
		byte = ttys0_rx_byte();
	}
}

static struct stream serial_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
};
