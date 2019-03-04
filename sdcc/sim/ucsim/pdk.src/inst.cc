/*
 * Simulator of microcontrollers (inst.cc)
 */
/* This file is part of microcontroller simulator: ucsim.

UCSIM is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

UCSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with UCSIM; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA. */
/*@1@*/

#include <stdlib.h>
#include <cassert>
#include "ddconfig.h"
#include "stdio.h"

// local
#include "pdkcl.h"
#include "regspdk.h"

unsigned char cl_pdk::add_to(unsigned char initial, int value) {
  // Zero.
  regs.Z = initial + value == 0 && regs.C;

  if (initial + value < initial) {
    if (regs.C) {
      // Actual overflow of accumulator.
      regs.OV = true;
      regs.C = false;
    } else {
      regs.OV = false;
      regs.C = true;
    }
  } else {
    regs.OV = false;
  }

  // TODO: Have support for auxiliary carry.

  return initial + value;
}

unsigned char cl_pdk::sub_to(unsigned char initial, int value) {
  // Zero.
  regs.Z = initial - value == 0 && !regs.C;

  if (initial - value > initial) {
    if (!regs.C) {
      // Actual overflow of accumulator.
      regs.OV = true;
      regs.C = true;
    } else {
      regs.OV = false;
      regs.C = false;
    }
  } else {
    regs.OV = false;
  }

  // TODO: Add support for auxiliary carry.

  return initial - value;
}

int cl_pdk::get_mem(unsigned int addr) {
  vc.rd++;
  return ram->read((t_addr)(addr));
}

unsigned char cl_pdk::get_io(t_addr addr) {
  // TODO: Support the rest.
  if (addr == 0x00) {
    // ACC status flag
    unsigned char value = (unsigned char)~0u;
    value &= 0xF;
    value |= regs.Z;
    value |= regs.C << 1;
    value |= regs.AC << 2;
    value |= regs.OV << 3;
    return value;
  } else if (addr == 0x02) {
    // Stack Pointer
    return regs.SP;
  }
  assert(!"not implemented");
}

void cl_pdk::store_io(t_addr addr, unsigned char value) {
  // TODO: The rest.
  if (addr == 0x00) {
    // ACC status flag
    regs.Z = value & 1;
    regs.C = value & 2;
    regs.AC = value & 4;
    regs.OV = value & 8;
  } else if (addr == 0x02) {
    // Stack Pointer
    regs.SP = value;
  } else {
    assert(!"not implemented");
  }
}

/* End of pdk.src/inst.cc */
