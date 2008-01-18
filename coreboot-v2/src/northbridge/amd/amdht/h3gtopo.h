/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software * you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation * version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program * if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef HTTOPO_H
#define HTTOPO_H

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *			DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *			TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*
 *   0
 */
static u8 const amdHtTopologySingleNode[] = {
	0x01,
	0x00, 0xFF	// Node 0
};

/*
 *   0---1
 */
static u8 const amdHtTopologyDualNode[] = {
	0x02,
	0x02, 0xFF, 0x00, 0x11,	// Node 0
	0x00, 0x00, 0x01, 0xFF	// Node 1
};

/*
 *   2
 *   |
 *   |
 *   0---1
 */
static u8 const amdHtTopologyThreeLine[] = {
	0x03,
	0x06, 0xFF, 0x04, 0x11, 0x02, 0x22,	// Node 0
	0x00, 0x00, 0x01, 0xFF, 0x00, 0x00,	// Node 1
	0x00, 0x00, 0x00, 0x00, 0x01, 0xFF	// Node 2
};

/*
 *   2
 *   |\
 *   |  \
 *   0---1
 */
static u8 const amdHtTopologyTriangle[] = {
	0x03,
	0x06, 0xFF, 0x00, 0x11, 0x00, 0x22,	// Node 0
	0x00, 0x00, 0x05, 0xFF, 0x00, 0x22,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x03, 0xFF	// Node 2
};

/*
 *   2   3
 *   |\  |
 *   |  \|
 *   0---1
 */
static u8 const amdHtTopologyFourDegenerate[] = {
	0x04,
	0x06, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x11,	// Node 0
	0x08, 0x00, 0x0D, 0xFF, 0x08, 0x22, 0x05, 0x33,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x03, 0xFF, 0x00, 0x11,	// Node 2
	0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x02, 0xFF	// Node 3
};

/*
 *   2---3
 *   |\ /|
 *   |/ \|
 *   0---1
 */
static u8 const amdHtTopologyFourFully[] = {
	0x04,
	0x0E, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33,	// Node 0
	0x00, 0x00, 0x0D, 0xFF, 0x00, 0x22, 0x00, 0x33,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x0B, 0xFF, 0x00, 0x33,	// Node 2
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x07, 0xFF	// Node 3
};


/*
 *   2---3
 *   |\  |
 *   |  \|
 *   0---1
 */
static u8 const amdHtTopologyFourKite[] = {
	0x04,
	0x06, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x11,	// Node 0
	0x08, 0x00, 0x0D, 0xFF, 0x00, 0x22, 0x00, 0x33,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x0B, 0xFF, 0x01, 0x33,	// Node 2
	0x00, 0x22, 0x00, 0x11, 0x00, 0x22, 0x06, 0xFF	// Node 3
};


/*
 *   2   3
 *   |   |
 *   |   |
 *   0---1
 */
static u8 const amdHtTopologyFourLine[] = {
	0x04,
	0x06, 0xFF, 0x04, 0x11, 0x02, 0x22, 0x04, 0x11,	// Node 0
	0x08, 0x00, 0x09, 0xFF, 0x08, 0x00, 0x01, 0x33,	// Node 1
	0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00,	// Node 2
	0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x02, 0xFF	// Node 3
};


/*
 *   2---3
 *   |   |
 *   |   |
 *   0---1
 */
static u8 const amdHtTopologyFourSquare[] = {
	0x04,
	0x06, 0xFF, 0x00, 0x11, 0x02, 0x22, 0x00, 0x22,	// Node 0
	0x00, 0x00, 0x09, 0xFF, 0x00, 0x33, 0x01, 0x33,	// Node 1
	0x08, 0x00, 0x00, 0x00, 0x09, 0xFF, 0x00, 0x33,	// Node 2
	0x00, 0x11, 0x04, 0x11, 0x00, 0x22, 0x06, 0xFF,	// Node 3
};


/*
 *   2---3
 *   |\
 *   |  \
 *   0   1
 */
static u8 const amdHtTopologyFourStar[] = {
	0x04,
	0x04, 0xFF, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22,	// Node 0
	0x00, 0x22, 0x04, 0xFF, 0x00, 0x22, 0x00, 0x22,	// Node 1
	0x0A, 0x00, 0x09, 0x11, 0x0B, 0xFF, 0x03, 0x33,	// Node 2
	0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x04, 0xFF,	// Node 3
};


static u8 const amdHtTopologyFiveFully[] = {
	0x05,
	0x1E, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44,	// Node 0
	0x00, 0x00, 0x1D, 0xFF, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x1B, 0xFF, 0x00, 0x33, 0x00, 0x44,	// Node 2
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x17, 0xFF, 0x00, 0x44,	// Node 3
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x0F, 0xFF	// Node 4
};


/*
 *
 *  4
 *  |\
 *  |  \
 *  2   3
 *  |   |
 *  0---1
 */
static u8 const amdHtTopologyFiveTwistedLadder[] = {
	0x05,
	0x06, 0xFF, 0x04, 0x11, 0x02, 0x22, 0x00, 0x11, 0x00, 0x22,	// Node0
	0x08, 0x00, 0x09, 0xFF, 0x08, 0x00, 0x01, 0x33, 0x00, 0x30,	// Node1
	0x10, 0x00, 0x10, 0x00, 0x11, 0xFF, 0x00, 0x40, 0x01, 0x44,	// Node2
	0x00, 0x11, 0x00, 0x11, 0x00, 0x14, 0x12, 0xFF, 0x02, 0x44,	// Node3
	0x00, 0x22, 0x00, 0x23, 0x00, 0x22, 0x04, 0x33, 0x0C, 0xFF	// Node4
};


static u8 const amdHtTopologySixFully[] = {
	0x06,
	0x3E, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55,	// Node 0
	0x00, 0x00, 0x3D, 0xFF, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x3B, 0xFF, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55,	// Node 2
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x37, 0xFF, 0x00, 0x44, 0x00, 0x55,	// Node 3
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x2F, 0xFF, 0x00, 0x55,	// Node 4
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x1F, 0xFF	// Node 5
};

/*
 *
 *  4   5
 *  |\ /|
 *  |/ \|
 *  2   3
 *  |   |
 *  0---1
 */
static u8 const amdHtTopologySixTwistedLadder[] = {
	0x06,
	0x06, 0xFF, 0x04, 0x11, 0x02, 0x22, 0x00, 0x11, 0x02, 0x22, 0x00, 0x12,	// Node0
	0x08, 0x00, 0x09, 0xFF, 0x00, 0x00, 0x01, 0x33, 0x00, 0x03, 0x01, 0x33,	// Node1
	0x30, 0x00, 0x00, 0x00, 0x31, 0xFF, 0x00, 0x54, 0x21, 0x44, 0x00, 0x55,	// Node2
	0x00, 0x11, 0x30, 0x11, 0x00, 0x45, 0x32, 0xFF, 0x00, 0x44, 0x12, 0x55,	// Node3
	0x00, 0x22, 0x00, 0x32, 0x08, 0x22, 0x00, 0x33, 0x0C, 0xFF, 0x00, 0x32,	// Node4
	0x00, 0x23, 0x00, 0x33, 0x00, 0x22, 0x04, 0x33, 0x00, 0x23, 0x0C, 0xFF	// Node5
};


static u8 const amdHtTopologySevenFully[] = {
	0x07,
	0x7E, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66,	// Node 0
	0x00, 0x00, 0x7D, 0xFF, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0x7B, 0xFF, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66,	// Node 2
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x77, 0xFF, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66,	// Node 3
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x6F, 0xFF, 0x00, 0x55, 0x00, 0x66,	// Node 4
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x5F, 0xFF, 0x00, 0x66,	// Node 5
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x3F, 0xFF,	// Node 6
};


/*  6
 *  |
 *  4   5
 *  |\ /|
 *  |/ \|
 *  2   3
 *  |   |
 *  0---1
 */
static u8 const amdHtTopologySevenTwistedLadder[] = {
	0x07,
	0x06, 0xFF, 0x00, 0x11, 0x02, 0x22, 0x00, 0x12, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22,	// Node0
	0x00, 0x00, 0x09, 0xFF, 0x00, 0x03, 0x01, 0x33, 0x00, 0x33, 0x00, 0x33, 0x00, 0x33,	// Node1
	0x30, 0x00, 0x00, 0x50, 0x31, 0xFF, 0x00, 0x54, 0x21, 0x44, 0x01, 0x55, 0x21, 0x44,	// Node2
	0x00, 0x41, 0x30, 0x11, 0x00, 0x45, 0x32, 0xFF, 0x02, 0x44, 0x12, 0x55, 0x02, 0x44,	// Node3
	0x48, 0x22, 0x40, 0x33, 0x48, 0x22, 0x40, 0x33, 0x4C, 0xFF, 0x40, 0x32, 0x0C, 0x66,	// Node4
	0x00, 0x22, 0x04, 0x33, 0x00, 0x22, 0x04, 0x33, 0x00, 0x23, 0x0C, 0xFF, 0x00, 0x23,	// Node5
	0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x10, 0xFF 	// Node6
};


/*
 *   5--4
 *  /####\
 * 6######3
 * |######|
 * 7######2
 *  \####/
 *   0--1
 */
static u8 const amdHtTopologyEightFully [] = {
	0x08,
	0xFE, 0xFF, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66, 0x00, 0x77,	// Node 0
	0x00, 0x00, 0xFD, 0xFF, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66, 0x00, 0x77,	// Node 1
	0x00, 0x00, 0x00, 0x11, 0xFB, 0xFF, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66, 0x00, 0x77,	// Node 2
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0xF7, 0xFF, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66, 0x00, 0x77,	// Node 3
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0xEF, 0xFF, 0x00, 0x55, 0x00, 0x66, 0x00, 0x77,	// Node 4
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0xDF, 0xFF, 0x00, 0x66, 0x00, 0x77,	// Node 5
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0xBF, 0xFF, 0x00, 0x77,	// Node 6
	0x00, 0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x33, 0x00, 0x44, 0x00, 0x55, 0x00, 0x66, 0x7F, 0xFF	// Node 7
};


/*  6---7
 *  |   |
 *  4---5
 *  |   |
 *  2---3
 *  |   |
 *  0---1
 */
static u8 const amdHtTopologyEightStraightLadder[] = {
	0x08,
	0x06, 0xFF, 0x00, 0x11, 0x02, 0x22, 0x00, 0x22, 0x02, 0x22, 0x00, 0x22, 0x02, 0x22, 0x00, 0x22,	// Node0
	0x00, 0x00, 0x09, 0xFF, 0x00, 0x33, 0x01, 0x33, 0x00, 0x33, 0x01, 0x33, 0x00, 0x33, 0x01, 0x33,	// Node1
	0x18, 0x00, 0x00, 0x00, 0x19, 0xFF, 0x00, 0x33, 0x09, 0x44, 0x00, 0x44, 0x09, 0x44, 0x00, 0x44,	// Node2
	0x00, 0x11, 0x24, 0x11, 0x00, 0x22, 0x26, 0xFF, 0x00, 0x55, 0x06, 0x55, 0x00, 0x55, 0x06, 0x55,	// Node3
	0x60, 0x22, 0x00, 0x22, 0x60, 0x22, 0x00, 0x22, 0x64, 0xFF, 0x00, 0x55, 0x24, 0x66, 0x00, 0x66,	// Node4
	0x00, 0x33, 0x90, 0x33, 0x00, 0x33, 0x90, 0x33, 0x00, 0x44, 0x98, 0xFF, 0x00, 0x77, 0x18, 0x77,	// Node5
	0x80, 0x44, 0x00, 0x44, 0x80, 0x44, 0x00, 0x44, 0x80, 0x44, 0x00, 0x44, 0x90, 0xFF, 0x00, 0x77,	// Node6
	0x00, 0x55, 0x40, 0x55, 0x00, 0x55, 0x40, 0x55, 0x00, 0x55, 0x40, 0x55, 0x00, 0x66, 0x60, 0xFF	// Node7
};


/*  6---7
 *  |   |
 *  4   5
 *  |\ /|
 *  |/ \|
 *  2   3
 *  |   |
 *  0---1
 */
static u8 const amdHtTopologyEightTwistedLadder[] = {
	0x08,
	0x06, 0xFF, 0x00, 0x11, 0x02, 0x22, 0x00, 0x12, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22, 0x00, 0x22,	// Node0
	0x00, 0x00, 0x09, 0xFF, 0x00, 0x03, 0x01, 0x33, 0x00, 0x33, 0x00, 0x33, 0x00, 0x33, 0x00, 0x33,	// Node1
	0x30, 0x00, 0x00, 0x50, 0x31, 0xFF, 0x00, 0x54, 0x21, 0x44, 0x01, 0x55, 0x21, 0x44, 0x01, 0x55,	// Node2
	0x00, 0x41, 0x30, 0x11, 0x00, 0x45, 0x32, 0xFF, 0x02, 0x44, 0x12, 0x55, 0x02, 0x44, 0x12, 0x55,	// Node3
	0x48, 0x22, 0x40, 0x33, 0x48, 0x22, 0x40, 0x33, 0x4C, 0xFF, 0x00, 0x32, 0x0C, 0x66, 0x00, 0x36,	// Node4
	0x80, 0x22, 0x84, 0x33, 0x80, 0x22, 0x84, 0x33, 0x00, 0x23, 0x8C, 0xFF, 0x00, 0x27, 0x0C, 0x77,	// Node5
	0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x44, 0x80, 0x44, 0x00, 0x74, 0x90, 0xFF, 0x00, 0x77,	// Node6
	0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x65, 0x40, 0x55, 0x00, 0x66, 0x60, 0xFF	// Node7
};

static const u8 * const amd_topo_list[] = {
	amdHtTopologySingleNode,
	amdHtTopologyDualNode,
	amdHtTopologyThreeLine,
	amdHtTopologyTriangle,
	amdHtTopologyFourLine,
	amdHtTopologyFourStar,
	amdHtTopologyFourDegenerate,
	amdHtTopologyFourSquare,
	amdHtTopologyFourKite,
	amdHtTopologyFourFully,
	amdHtTopologyFiveFully,
	amdHtTopologySixFully,
	amdHtTopologySevenFully,
	amdHtTopologyEightFully,
	amdHtTopologyEightTwistedLadder,
	amdHtTopologyEightStraightLadder,
	NULL			// NULL to mark end of list
};

/*----------------------------------------------------------------------------
 *			FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
void getAmdTopolist(u8 ***p);


#endif	 /* HTTOPO_H */

