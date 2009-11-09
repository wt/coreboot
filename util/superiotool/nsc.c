/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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

#include "superiotool.h"

#define CHIP_ID_REG	0x20	/* Super I/O ID (SID) / family */
#define CHIP_REV_REG	0x27	/* Super I/O revision ID (SRID) */

static const struct superio_registers reg_table[] = {
	{0xcf, "PC97307", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x27,EOT},
			{0xcf,MISC,MISC,0x00,0x00,NANA,EOT}},
		{0x0, "Keyboard",
			{0x30,0x31,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{MISC,0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{0x1, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x2, "Real-time clock (RTC), advanced power control (APC)",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{MISC,0x00,0x00,0x70,0x08,0x00,0x04,0x04,EOT}},
		{0x3, "Floppy",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 EOT},
			{MISC,0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x20,0x00,
			 EOT}},
		{0x4, "Parallel port",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0x78,0x07,0x00,0x04,0x04,0xf2,EOT}},
		{0x5, "COM2 / IR",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x6, "COM1",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x7, "GPIO",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{0x8, "Power management",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{EOT}}},
	{0xd0, "PC87317", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,EOT},
			{0xd0,MISC,MISC,0x00,0x00,0x00,EOT}},
		{0x0, "Keyboard",
			{0x30,0x31,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{MISC,0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{0x1, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x2, "Real-time clock (RTC), advanced power control (APC)",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{MISC,0x00,0x00,0x70,0x08,0x00,0x04,0x04,EOT}},
		{0x3, "Floppy",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 EOT},
			{MISC,0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x20,0x00,
			 EOT}},
		{0x4, "Parallel port",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0x78,0x07,0x00,0x04,0x04,0xf2,EOT}},
		{0x5, "COM2 / IR",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x6, "COM1",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x7, "GPIO",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{0x8, "Power management",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{EOT}}},
	{0xdf, "PC97317", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x27,EOT},
			{0xdf,MISC,MISC,0x00,0x00,0x00,NANA,EOT}},
		{0x0, "Keyboard",
			{0x30,0x31,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{MISC,0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{0x1, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x2, "Real-time clock (RTC), advanced power control (APC)",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{MISC,0x00,0x00,0x70,0x08,0x00,0x04,0x04,EOT}},
		{0x3, "Floppy",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 EOT},
			{MISC,0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x20,0x00,
			 EOT}},
		{0x4, "Parallel port",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0x78,0x07,0x00,0x04,0x04,0xf2,EOT}},
		{0x5, "COM2 / IR",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x6, "COM1",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x7, "GPIO",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{0x8, "Power management",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{EOT}}},
	{0xe0, "PC87309", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x27,0x2e,EOT},
			{0xe0,MISC,0x00,NANA,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 EOT},
			{MISC,0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x20,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0x78,0x07,0x00,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2 / IR",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x31,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "Power management",
			{0x30,0x31,0x60,0x61,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x31,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{0x01,0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{EOT}}},
	{0xe1, "PC87360", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x2a,
			 0x2b,0x2c,0x2d,0x2e,EOT},
			{0xe1,0x11,0x00,0x03,0x00,0x00,0x00,NANA,0x00,MISC,
			 0x00,0x00,0x00,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x00,0x00,
			 EOT}},
		{0x8, "ACCESS.bus (ACB)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0x9, "Fan speed control and monitor (FSCM)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xa, "Watchdog timer",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x02,EOT}},
		{EOT}}},
	{0xe2, "PC87351", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x27,0x2e,EOT},
			{0xe2,0x11,0xa1,0x00,MISC,NANA,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,
			 0x40,EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x00,EOT}},
		{0x8, "Fan speed control",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,EOT}},
		{EOT}}},
	{0xe4, "PC87364", {
		{EOT}}},
	{0xe5, "PC87365", {	/* SRID[7..0] == chip revision */
		{EOT}}},
	{0xe8, "PC87363", {
		{EOT}}},
	{0xe9, "PC87366", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x27,0x28,0x2a,0x2b,
			 0x2c,0x2d,0x2e,EOT},
			{0xe9,0x11,0x00,0x03,0x00,0x00,NANA,0x00,MISC,MISC,
			 0x00,MISC,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x00,0x00,
			 EOT}},
		{0x8, "ACCESS.bus (ACB)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0x9, "Fan speed control and monitor (FSCM)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x00,0x00,
			 EOT}},
		{0xa, "Watchdog timer (WDT)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x02,EOT}},
		{0xb, "Game port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x00,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xc, "MIDI port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0x30,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xd, "Voltage level monitor (VLM)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{0xe, "Temperature sensor (TMS)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,EOT}},
		{EOT}}},

	/* SID[7..0]: family, SRID[7..5]: ID, SRID[4..0]: rev. */
	{0xea, "PC8739x", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0xea,0x11,MISC,MISC,MISC,0x80,0x00,NANA,0x00,MISC,
			 0x37,RSVD,RSVD,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2 / FIR",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,0x44,0x01,
			 EOT}},
		{0xa, "Watchdog timer (WDT)",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x02,EOT}},
		{0xb, "Game port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x01,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xc, "MIDI port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0x30,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xf, "X-Bus",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{EOT}}},
	{0xec, "PC87591x", {
		/* SRID[7..5]: 000=PC87591E, 001=PC87591S, 100=PC87591L */
		{EOT}}},
	{0xee, "PC8741x", {
		/* SRID[7..5] is marked as "not applicable for the PC8741x". */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0xee,0x11,0x20,MISC,MISC,MISC,0x00,NANA,0x00,MISC,
			 0x00,RSVD,RSVD,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x70,
			 0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 0xf3,EOT},
			{0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,MISC,0x01,
			 0x00,EOT}},
		{0xf, "X-Bus",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,
			 EOT},
			{0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 EOT}},
		{0x10, "Real-time clock (RTC)",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 0xf1,0xf2,0xf3,EOT},
			{0x00,0x00,0x70,0x00,0x72,0x08,0x00,0x04,0x04,0x00,
			 0x00,0x00,0x00,EOT}},
		{EOT}}},
	{0xf0, "PC87372", {
		{EOT}}},
	{0x0f1, "PC8374L", {
		{NOLDN, NULL,
			{0x10,0x12,0x13,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
			 0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x00,0x00,0x00,0xf1,0x11,0x00,0x00,0x00,RSVD,0x00,
			 MISC,RSVD,0x01,0x2e,RSVD,RSVD,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf8,
			 EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,0x24,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf8,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,0x07,EOT}},
		{0x2, "COM2 / IR",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x50,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,
			 EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 0xf2,0xf3,0xf8,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,MISC,
			 0x00,MISC,0x01,EOT}},
		{0x8, "Health management",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x05,EOT}},
		{EOT}}},
	{0x8f1, "WPCD376I", {
		{NOLDN, NULL,
			{0x10,0x12,0x13,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
			 0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x00,0x00,0x00,0xf1,0x11,0x00,0x00,0x00,RSVD,0x00,
			 MISC,RSVD,0x01,0x2e,RSVD,RSVD,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf8,
			 EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,0x24,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf8,EOT},
			{0x00,0x02,0x78,0x07,0x02,0x04,0x04,0xf2,0x07,EOT}},
		{0x2, "IR",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x50,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,
			 EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 0xf2,0xf3,0xf8,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,MISC,
			 0x00,MISC,0x01,EOT}},
		{EOT}}},
	{0xf2, "PC87427", {
		/* SRID[7..5] is marked as "not applicable for the PC87427". */
		{NOLDN, NULL,
			{0x10,0x12,0x13,0x1d,0x20,0x21,0x22,0x23,0x24,0x25,
			 0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
			 EOT},
			{0x00,0x00,0x00,0x00,0xf2,0x11,0xa0,MISC,MISC,MISC,
			 0x02,NANA,0x00,MISC,0x00,0x00,MISC,MISC,RSVD,RSVD,
			 EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,EOT},
			{0x00,0x03,0xf2,0x06,0x03,0x02,0x04,0x24,0x00,EOT}},
		{0x2, "COM2",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x02,0xf8,0x03,0x03,0x04,0x04,0x02,EOT}},
		{0x3, "COM1",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x03,0xf8,0x04,0x03,0x04,0x04,0x02,EOT}},
		{0x4, "System wake-up control (SWC)",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x70,
			 0x71,0x74,0x75,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x03,0x04,0x04,EOT}},
		{0x5, "Mouse",
			{0x30,0x70,0x71,0x74,0x75,EOT},
			{0x00,0x0c,0x02,0x04,0x04,EOT}},
		{0x6, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 EOT},
			{0x00,0x00,0x60,0x00,0x64,0x01,0x02,0x04,0x04,0x40,
			 EOT}},
		{0x7, "GPIO",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,MISC,
			 0x01,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0x9, "Fan Monitor and Control (FMC)",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,
			 EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x19,0x06,
			 EOT}},
		{0xa, "Watchdog timer (WDT)",
			{0x30,0x50,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,0x00,EOT}},
		{0xf, "X-Bus",
			{0x30,0x60,0x61,0x70,0x71,0x74,0x75,0xf0,0xf1,0xf2,
			 0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,
			 0xfd,0xfe,0xff,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,0x80,0x10,EOT}},
		{0x10, "Real-time clock (RTC)",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,0xf0,
			 0xf1,0xf2,0xf3,0xf6,0xf7,EOT},
			{0x00,0x00,0x70,0x00,0x72,0x08,0x00,0x04,0x04,0x00,
			 0x00,0x00,0x00,MISC,0x00,EOT}},
		{0x14, "Health monitoring and control (HMC)",
			{0x30,0x50,0x60,0x61,0x62,0x63,0x70,0x71,0x74,0x75,
			 0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x04,
			 0x05,EOT}},
		{EOT}}},
	{0xf3, "PC87373", {
		{EOT}}},
	{EOT}
};

void probe_idregs_nsc(uint16_t port)
{
	uint8_t id, rev;
	uint16_t magic;

	probing_for("NSC", "", port);

	OUTB(CHIP_ID_REG, port);
	if (INB(port) != CHIP_ID_REG) {
		if (verbose)
			printf(NOTFOUND "port=0x%02x, port+1=0x%02x\n",
			       INB(port), INB(port + 1));
		return;
	}
	id = INB(port + 1);

	OUTB(CHIP_REV_REG, port);
	if (INB(port) != CHIP_REV_REG) {
		printf("Warning: Can't get chip revision. Setting to 0xff.\n");
		rev = 0xff;
	} else {
		rev = INB(port + 1);
	}

	/*
	 * NSC PC8374L has ID 0xf1, rev 0b000xxxxx according to datasheet.
	 * Winbond WPCD376I has ID 0xf1, rev 0b100xxxxx according to datasheet.
	 */
	if (id == 0xf1)
		magic = ((rev & 0xe0) << 4) | id;
	else
		magic = id;

	if (superio_unknown(reg_table, magic)) {
		if (verbose)
			printf(NOTFOUND "sid=0x%02x, srid=0x%02x\n", id, rev);
		return;
	}

	printf("Found %s %s (sid=0x%02x, srid=0x%02x) at 0x%x\n",
	       magic == 0x8f1 ? "Winbond" : "NSC",
	       get_superio_name(reg_table, magic), id, rev, port);
	chip_found = 1;

	dump_superio(magic == 0x8f1 ? "Winbond" : "NSC",
		     reg_table, port, magic, LDN_SEL);
}

void print_nsc_chips(void)
{
	print_vendor_chips("NSC", reg_table);
}
