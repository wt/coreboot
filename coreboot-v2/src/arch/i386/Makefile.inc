#######################################################################
# Take care of subdirectories
subdirs-y += boot
subdirs-y += init
subdirs-y += lib
subdirs-y += smp

obj-y += ../../option_table.o

ifdef POST_EVALUATION
#######################################################################
# Build the final rom image

$(obj)/coreboot.rom: $(obj)/coreboot.bootblock $(obj)/coreboot_ram $(CBFSTOOL)
	$(Q)rm -f $@
	$(Q)$(CBFSTOOL) $@ create $(shell expr 1024 \* $(CONFIG_COREBOOT_ROMSIZE_KB)) 131072 $(obj)/coreboot.bootblock
	$(Q)$(CBFSTOOL) $@ add-stage  $(obj)/coreboot_ram normal/coreboot_ram $(CBFS_COMPRESS_FLAG)
	$(Q)if [ -f fallback/coreboot_apc ]; \
	then \
		$(CBFSTOOL) $@ add-stage fallback/coreboot_apc fallback/coreboot_apc $(CBFS_COMPRESS_FLAG); \
	fi
	$(Q)$(CBFSTOOL) $@ add-stage  $(obj)/coreboot_ram fallback/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	$(Q)printf "    PAYLOAD    none (as specified by user)\n"
else
	printf "    PAYLOAD    $(CONFIG_FALLBACK_PAYLOAD_FILE) $(COMPRESSFLAG)\n"
	$(Q)$(CBFSTOOL) ./build/coreboot.rom add-payload $(CONFIG_FALLBACK_PAYLOAD_FILE)  fallback/payload $(CBFS_COMPRESS_FLAG)
	printf "    PAYLOAD    $(CONFIG_NORMAL_PAYLOAD_FILE) $(COMPRESSFLAG)\n"
	$(Q)$(CBFSTOOL) ./build/coreboot.rom add-payload $(CONFIG_NORMAL_PAYLOAD_FILE)  normal/payload $(CBFS_COMPRESS_FLAG)
	$(CBFSTOOL) ./build/coreboot.rom print
endif


#######################################################################
# Build the bootblock

BOOTBLOCK_SIZE=65536

$(obj)/coreboot.bootblock: $(obj)/coreboot.strip
	$(Q)printf "    CREATE     $(subst $(obj)/,,$(@))\n"
	$(Q)dd if=$< of=$(obj)/coreboot.bootblock.one obs=$(BOOTBLOCK_SIZE) conv=sync
	$(Q)cat $(obj)/coreboot.bootblock.one $(obj)/coreboot.bootblock.one > $(obj)/coreboot.bootblock

$(obj)/coreboot.strip: $(obj)/coreboot
	$(Q)printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $< $@

$(obj)/ldscript.ld: $(ldscripts) $(obj)/ldoptions
	$(Q)printf 'INCLUDE "ldoptions"\n' > $@
	$(Q)printf '$(foreach ldscript,$(ldscripts),INCLUDE "$(ldscript)"\n)' >> $@

$(obj)/crt0_includes.h: $(crt0s)
	$(Q)printf '$(foreach crt0,$(obj)/config.h $(crt0s),#include "$(crt0)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(src)/arch/i386/init/crt0.S.lb $(obj)/crt0_includes.h
	$(CC) -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/i386/include -I$(obj) -include $(obj)/config.h -I. -I$(src) $< > $@.new && mv $@.new $@

$(obj)/coreboot: $(initobjs) $(obj)/ldscript.ld
	$(Q)printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(Q)$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/ldscript.ld $(initobjs)
	$(Q)$(NM) -n $(obj)/coreboot | sort > $(obj)/coreboot.map

#######################################################################
# i386 specific tools

$(obj)/option_table.h $(obj)/option_table.c: $(obj)/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	$(Q)printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(Q)$(obj)/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --header $(obj)/option_table.h --option $(obj)/option_table.c

$(obj)/build_opt_tbl: $(top)/util/options/build_opt_tbl.c $(top)/src/include/pc80/mc146818rtc.h $(top)/src/include/boot/coreboot_tables.h
	$(Q)printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -include $(obj)/config.h $< -o $@

#######################################################################
# Build the coreboot_ram (stage 2)

$(obj)/coreboot_ram: $(obj)/coreboot_ram.o $(src)/config/coreboot_ram.ld #ldoptions
	$(Q)printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(Q)$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/config/coreboot_ram.ld $(obj)/coreboot_ram.o
	$(Q)$(NM) -n $(obj)/coreboot_ram | sort > $(obj)/coreboot_ram.map

$(obj)/coreboot_ram.o: $(obj)/arch/i386/lib/c_start.o $(drivers) $(obj)/coreboot.a $(LIBGCC_FILE_NAME)
	$(Q)printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(Q)$(CC) -nostdlib -r -o $@ $(obj)/arch/i386/lib/c_start.o $(drivers) -Wl,-\( $(obj)/coreboot.a $(LIBGCC_FILE_NAME) -Wl,-\)

$(obj)/coreboot.a: $(objs)
	$(Q)printf "    AR         $(subst $(obj)/,,$(@))\n"
	$(Q)rm -f $(obj)/coreboot.a
	$(Q)$(AR) cr $(obj)/coreboot.a $(objs)


#######################################################################
# done

endif