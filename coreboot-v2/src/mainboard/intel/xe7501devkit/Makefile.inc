ROMCCFLAGS := -mcpu=p4
obj-$(CONFIG_HAVE_ACPI_TABLES) += acpi_tables.o
include $(src)/mainboard/Makefile.romccboard.inc
