/*-------------------------------------------------------------------------
   features.h - STM8 specific features.

   Copyright (C) 2001, Michael Hope, 2013, Philipp Klaus Krause

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#ifndef __SDCC_ASM_STM8_FEATURES_H
#define __SDCC_ASM_STM8_FEATURES_H   1

#define _REENTRANT
#define _CODE
#define _AUTOMEM
#define _STATMEM

#define _SDCC_MANGLES_SUPPORT_FUNS	1
#define _SDCC_Z80_STYLE_LIB_OPT		1

#define __SDCC_NONBANKED

#endif

