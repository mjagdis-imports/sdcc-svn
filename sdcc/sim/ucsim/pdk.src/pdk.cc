/*
 * Simulator of microcontrollers (pdk.cc)
 *
 * Copyright (C) 1999,99 Drotos Daniel, Talker Bt.
 *
 * To contact author send email to drdani@mazsola.iit.uni-miskolc.hu
 *
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

#include "ddconfig.h"

#include <ctype.h>
#include <stdarg.h> /* for va_list */
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include "i_string.h"

// prj
#include "globals.h"
#include "pobjcl.h"

// sim
#include "simcl.h"

// local
#include "glob.h"
#include "pdkcl.h"
#include "portcl.h"
#include "regspdk.h"

/*******************************************************************/

/*
 * Base type of PDK controllers
 */

cl_pdk::cl_pdk(struct cpu_entry *IType, class cl_sim *asim) : cl_uc(asim) {
  type = IType;
}

int cl_pdk::init(void) {
  cl_uc::init(); /* Memories now exist */

  xtal = 8000000;

  // rom = address_space(MEM_ROM_ID);
  // ram = mem(MEM_XRAM);
  // ram = rom;

  // zero out ram(this is assumed in regression tests)
  // for (int i = 0x0; i < 0x8000; i++) {
  //   ram->set((t_addr)i, 0);
  // }

  return (0);
}

void cl_pdk::reset(void) {
  cl_uc::reset();

  regs.SP = 0x80;
  regs.A = 0;
  regs.Z = 0;
  regs.C = 0;
  regs.AC = 0;
  regs.OV = 0;
  PC = 0x0000;
}

char *cl_pdk::id_string(void) { return ((char *)"pdk14"); }

/*
 * Making elements of the controller
 */
/*
t_addr
cl_pdk::get_mem_size(enum mem_class type)
{
  switch(type)
    {
    case MEM_ROM: return(0x10000);
    case MEM_XRAM: return(0x10000);
    default: return(0);
    }
 return(cl_uc::get_mem_size(type));
}
*/

void cl_pdk::mk_hw_elements(void) {
  // TODO: Add hardware stuff here.
  cl_uc::mk_hw_elements();
}

class cl_memory_chip *c;

void cl_pdk::make_memories(void) {
  class cl_address_space *as;

  ram = rom = as = new cl_address_space("rom", 0, 0x7D0, 16);
  // ram = new cl_address_space("ram", 0, 0x80, 8);

  as->init();
  address_spaces->add(as);

  class cl_address_decoder *ad;
  class cl_memory_chip *chip;

  chip = new cl_memory_chip("rom_chip", 0x7Dd0, 16);
  chip->init();
  memchips->add(chip);

  ad = new cl_address_decoder(as = address_space("rom"), chip, 0, 0x7CF, 0);
  ad->init();
  as->decoders->add(ad);
  ad->activate(0);

  regs8 = new cl_address_space("regs8", 0, 5, 8);
  regs8->init();
  regs8->get_cell(0)->decode((t_mem *)&regs.A);
  regs8->get_cell(1)->decode((t_mem *)&regs.SP);
  regs8->get_cell(2)->decode((t_mem *)&regs.Z);
  regs8->get_cell(3)->decode((t_mem *)&regs.C);
  regs8->get_cell(4)->decode((t_mem *)&regs.AC);
  regs8->get_cell(5)->decode((t_mem *)&regs.OV);
  /* TODO: Add the other registers. */

  address_spaces->add(regs8);

  class cl_var *v;
  vars->add(v = new cl_var(cchars("A"), regs8, 0, ""));
  v->init();
  vars->add(v = new cl_var(cchars("SP"), regs8, 1, ""));
  v->init();
  vars->add(v = new cl_var(cchars("Z"), regs8, 2, ""));
  v->init();
  vars->add(v = new cl_var(cchars("C"), regs8, 3, ""));
  v->init();
  vars->add(v = new cl_var(cchars("AC"), regs8, 4, ""));
  v->init();
  vars->add(v = new cl_var(cchars("OV"), regs8, 5, ""));
  v->init();
}

/*
 * Help command interpreter
 */

struct dis_entry *cl_pdk::dis_tbl(void) {
  return (disass_pdk_14);
}

/*struct name_entry *
cl_pdk::sfr_tbl(void)
{
  return(0);
}*/

/*struct name_entry *
cl_pdk::bit_tbl(void)
{
  //FIXME
  return(0);
}*/

int cl_pdk::inst_length(t_addr /*addr*/) {
  return 1;
}
int cl_pdk::inst_branch(t_addr addr) {
  int b;

  get_disasm_info(addr, NULL, &b, NULL, NULL);

  return b;
}

bool cl_pdk::is_call(t_addr addr) {
  struct dis_entry *e;

  get_disasm_info(addr, NULL, NULL, NULL, &e);

  return e ? (e->is_call) : false;
}

int cl_pdk::longest_inst(void) { return 1; }

const char *cl_pdk::get_disasm_info(t_addr addr, int *ret_len, int *ret_branch,
                                    int *immed_offset,
                                    struct dis_entry **dentry) {
  const char *b = NULL;
  uint code;
  int len = 0;
  int immed_n = 0;
  int i;
  int start_addr = addr;
  struct dis_entry *dis_e;

  //code = rom->get(addr++);
  dis_e = NULL;

  switch (code = 0x14) {
      /* Dispatch to appropriate pdk port. */
      /*case 0x13 :
        code= rom->get(addr++);
    i= 0;
    while ((code & disass_pdk_13[i].mask) != disass_pdk13[i].code &&
      disass_pdk_13[i].mnemonic)
      i++;
    dis_e = &disass_pdk_13[i];
    b= disass_pdk_13[i].mnemonic;
    if (b != NULL)
      len += (disass_pdk_13[i].length + 1);
  break;*/

    case 0x14:
      code = rom->get(addr++);
      i = 0;
      while ((code & disass_pdk_14[i].mask) != disass_pdk_14[i].code &&
             disass_pdk_14[i].mnemonic)
        i++;
      dis_e = &disass_pdk_14[i];
      b = disass_pdk_14[i].mnemonic;
      if (b != NULL) len += 1; //(disass_pdk_14[i].length + 1);
      break;

      /*case 0x15 :
        code= rom->get(addr++);
    i= 0;
    while ((code & disass_pdk_15[i].mask) != disass_pdk_15[i].code &&
      disass_pdk_15[i].mnemonic)
      i++;
    dis_e = &disass_pdk_15[i];
    b= disass_pdk_15[i].mnemonic;
    if (b != NULL)
      len += (disass_pdk_15[i].length + 1);
  break;*/

      /*case 0x16 :
        code= rom->get(addr++);
    i= 0;
    while ((code & disass_pdk_16[i].mask) != disass_pdk_16[i].code &&
      disass_pdk_16[i].mnemonic)
      i++;
    dis_e = &disass_pdk_16[i];
    b= disass_pdk_16[i].mnemonic;
    if (b != NULL)
      len += (disass_pdk_16[i].length + 1);
  break;*/

      /*default:
        i= 0;
        while ((code & disass_pdk[i].mask) != disass_pdk[i].code &&
               disass_pdk[i].mnemonic)
          i++;
        dis_e = &disass_pdk[i];
        b= disass_pdk[i].mnemonic;
        if (b != NULL)
          len += (disass_pdk[i].length);
      break;*/
  }

  if (ret_branch) {
    *ret_branch = dis_e->branch;
  }

  if (immed_offset) {
    if (immed_n > 0)
      *immed_offset = immed_n;
    else
      *immed_offset = (addr - start_addr);
  }

  if (len == 0) len = 1;

  if (ret_len) *ret_len = len;

  if (dentry) *dentry = dis_e;

  return b;
}

char *cl_pdk::disass(t_addr addr, const char *sep) {
  char work[256], temp[20];
  const char *b;
  char *buf, *p, *t;
  int len = 0;
  int immed_offset = 0;
  struct dis_entry *dis_e;

  p = work;

  b = get_disasm_info(addr, &len, NULL, &immed_offset, &dis_e);

  if (b == NULL) {
    buf = (char *)malloc(30);
    strcpy(buf, "UNKNOWN/INVALID");
    return (buf);
  }

  while (*b) {
    if (*b == '%') {
      b++;
      uint code = rom->get(addr);
      switch (*(b++)) {
        case 'k':  // k    immediate addressing
          sprintf(temp, "#%u", code & ~(unsigned)dis_e->mask);
          break;
        case 'm':  // m    memory addressing
          sprintf(temp, "%u", code & ~(unsigned)dis_e->mask);
          break;
        case 'i':  // i    IO addressing
          // TODO: Maybe add pretty printing.
          sprintf(temp, "[%u]", code & ~(unsigned)dis_e->mask);
          break;
        default:
          strcpy(temp, "%?");
          break;
      }
      t = temp;
      while (*t) *(p++) = *(t++);
    } else
      *(p++) = *(b++);
  }
  *p = '\0';

  p = strchr(work, ' ');
  if (!p) {
    buf = strdup(work);
    return (buf);
  }
  if (sep == NULL)
    buf = (char *)malloc(6 + strlen(p) + 1);
  else
    buf = (char *)malloc((p - work) + strlen(sep) + strlen(p) + 1);
  for (p = work, t = buf; *p != ' '; p++, t++) *t = *p;
  p++;
  *t = '\0';
  if (sep == NULL) {
    while (strlen(buf) < 6) strcat(buf, " ");
  } else
    strcat(buf, sep);
  strcat(buf, p);
  return (buf);
}

void cl_pdk::print_regs(class cl_console_base *con) {
  con->dd_printf("A= 0x%02x(%3d)\n", regs.A, regs.A);
  con->dd_printf("Flag= 0b1111'%c%c%c%c  \n", regs.OV ? '1' : '0',
                 regs.AC ? '1' : '0', regs.C ? '1' : '0',
                 regs.Z ? '1' : '0');
  con->dd_printf("SP= 0x%02x(%3d)\n", regs.SP, regs.SP);

  print_disass(PC, con);
}

/*
 * Execution
 */

int cl_pdk::exec_inst(void) {
  t_mem code;

  if (fetch(&code)) {
    return (resBREAKPOINT);
  }
  tick(1);

#define CODE_MASK(op, m) ((code & ~(m)) == (op))

  if (code == 0x0000) {
    // nop
  } else if (CODE_MASK(0x0200, 0x7F)) {
    // ret k
    regs.A = code & 0xFF;
    regs.SP -= 2;
    PC = get_mem(regs.SP) | (get_mem(regs.SP + 1) << 8);
  } else if (code == 0x007A) {
    // ret
    regs.SP -= 2;
    PC = get_mem(regs.SP) | (get_mem(regs.SP + 1) << 8);
  } else if (CODE_MASK(0x2F00, 0xFF)) {
    // mov a, k
    regs.A = code & 0xFF;
  } else if (CODE_MASK(0x0180, 0x3F)) {
    // mov i, a
    store_io(code & 0x3F, regs.A);
  } else if (CODE_MASK(0x01C0, 0x3F)) {
    // mov a, i
    regs.A = get_io(code & 0x3F);
  } else if (CODE_MASK(0x0B80, 0x7F)) {
    // mov m, a
    ram->write(code & 0x7F, regs.A);
  } else if (CODE_MASK(0x0F80, 0x7F)) {
    // mov a, m
    regs.A = get_mem(code & 0x7F);
  } else
  // TODO: ldt16
  // TODO: stt16
  if ((CODE_MASK(0x380, 0x7E)) && !(code % 2)) {
    // idxm a, m
    regs.A = get_mem(code & 0x7E);
  } else if ((CODE_MASK(0x380, 0x7E)) && code % 2) {
    // idxm m, a
    regs.A = get_mem(code & 0x7E);
  } else if (CODE_MASK(0x1380, 0x7F)) {
    // xch m
    int mem = get_mem(code & 0x7F);
    ram->write(code & 0x7F, regs.A);
    regs.A = mem;
  } else if (code == 0x0072) {
    // pushaf
    u8_t flag = ~u8_t(0xF);
    flag |= regs.Z;
    flag |= regs.C << 1;
    flag |= regs.AC << 2;
    flag |= regs.OV << 3;
    ram->write(regs.SP++, flag);
    ram->write(regs.SP++, regs.A);
  } else if (code == 0x0073) {
    // popaf
    regs.A = get_mem(--regs.SP);
    u8_t flag = get_mem(--regs.SP);
    regs.Z = flag & 1;
    regs.C = flag & 2;
    regs.AC = flag & 4;
    regs.OV = flag & 8;
  } else if (CODE_MASK(0x2800, 0xFF)) {
    // add a, k
    regs.A = add_to(regs.A, code & 0xFF);
  } else if (CODE_MASK(0x0C00, 0x7F)) {
    // add a, m
    regs.A = add_to(regs.A, get_mem(code & 0x7F));
  } else if (CODE_MASK(0x0800, 0x7F)) {
    // add m, a
    int addr = code & 0x7F;
    ram->write(addr, add_to(regs.A, get_mem(addr)));
  } else if (CODE_MASK(0x2900, 0xFF)) {
    // sub a, k
    regs.A = sub_to(regs.A, code & 0xFF);
  } else if (CODE_MASK(0x0C80, 0x7F)) {
    // sub a, m
    regs.A = sub_to(regs.A, get_mem(code & 0x7F));
  } else if (CODE_MASK(0x0880, 0x7F)) {
    // sub m, a
    int addr = code & 0x7F;
    ram->write(addr, sub_to(get_mem(addr), regs.A));
  } else if (CODE_MASK(0x2800, 0xFF)) {
    // addc a, k
    regs.A = add_to(regs.A + regs.C, code & 0xFF);
  } else if (CODE_MASK(0x0C00, 0x7F)) {
    // addc a, m
    regs.A = add_to(regs.A + regs.C, get_mem(code & 0x7F));
  } else if (CODE_MASK(0x0800, 0x7F)) {
    // addc m, a
    int addr = code & 0x7F;
    ram->write(addr, add_to(regs.A + regs.C, get_mem(addr)));
  } else if (code == 0x0060) {
    // addc a
    regs.A = add_to(regs.A, regs.C); 
  } else if (CODE_MASK(0x1000, 0x7F)) {
    // addc m
    int addr = code & 0x7F;
    ram->write(addr, add_to(get_mem(addr), regs.C));
  } else if (CODE_MASK(0x2900, 0xFF)) {
    // subc a, k
    regs.A = sub_to(regs.A - regs.C, code & 0xFF);
  } else if (CODE_MASK(0x0C80, 0x7F)) {
    // subc a, m
    regs.A = sub_to(regs.A - regs.C, get_mem(code & 0x7F));
  } else if (CODE_MASK(0x0880, 0x7F)) {
    // subc m, a
    int addr = code & 0x7F;
    ram->write(addr, sub_to(get_mem(addr), regs.A - regs.C));
  } else if (code == 0x0061) {
    // subc a
    regs.A = sub_to(regs.A, regs.C); 
  } else if (CODE_MASK(0x1080, 0x7F)) {
    // subc m
    int addr = code & 0x7F;
    ram->write(addr, sub_to(get_mem(addr), regs.C));
  } else if (CODE_MASK(0x1200, 0x7F)) {
    // inc m
    int addr = code & 0x7F;
    ram->write(addr, add_to(get_mem(addr), 1));
  } else if (CODE_MASK(0x1280, 0x7F)) {
    // dec m
    int addr = code & 0x7F;
    ram->write(addr, sub_to(get_mem(addr), 1));
  } else if (CODE_MASK(0x1300, 0x7F)) {
    // clear m
    ram->write(code & 0x7F, 0);
  } else if (code == 0x006A) {
    // sr a
    regs.C = regs.A & 1;
    regs.A >>= 1;
  } else if (CODE_MASK(0x1500, 0x7F)) {
    // sr m
    int value = get_mem(code & 0x7F);
    regs.C = value & 1;
    ram->write(code & 0x7F, value >> 1);
  } else if (code == 0x006B) {
    // sl a
    regs.C = (regs.A & 0x80) >> 7;
    regs.A <<= 1;
  } else if (CODE_MASK(0x1580, 0x7F)) {
    // sl m
    int value = get_mem(code & 0x7F);
    regs.C = (value & 0x80) >> 7;
    ram->write(code & 0x7F, value << 1);
  } else if (code == 0x006A) {
    // src a
    int c = regs.A & 1;
    regs.A >>= 1;
    regs.A |= regs.C << 7;
    regs.C = c;
  } else if (CODE_MASK(0x1500, 0x7F)) {
    // src m
    int value = get_mem(code & 0x7F);
    int c = value & 1;
    ram->write(code & 0x7F, (value >> 1) | (regs.C << 7));
    regs.C = c;
  } else if (code == 0x006D) {
    // slc a
    int c = regs.A & 0x80;
    regs.A <<= 1;
    regs.A |= regs.C;
    regs.C = c >> 7;
  } else if (CODE_MASK(0x1500, 0x7F)) {
    // slc m
    int value = get_mem(code & 0x7F);
    int c = value & 0x80;
    ram->write(code & 0x7F, (value << 1) | regs.C);
    regs.C = c >> 7;
  } else if (CODE_MASK(0x2C00, 0xFF)) {
    // and a, k
    regs.A &= code & 0xFF;
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0E00, 0x7F)) {
    // and a, m
    regs.A &= get_mem(code & 0x7F);
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0A00, 0x7F)) {
    // and m, a
    int store = regs.A & get_mem(code & 0x7F);
    regs.Z = !store;
    ram->write(code & 0x7F, store);
  } else if (CODE_MASK(0x2D00, 0xFF)) {
    // or a, k
    regs.A |= code & 0xFF;
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0E80, 0x7F)) {
    // or a, m
    regs.A |= get_mem(code & 0x7F);
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0A80, 0x7F)) {
    // or m, a
    int store = regs.A | get_mem(code & 0x7F);
    regs.Z = !store;
    ram->write(code & 0x7F, store);
  } else if (CODE_MASK(0x2E00, 0xFF)) {
    // xor a, k
    regs.A ^= code & 0xFF;
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0F00, 0x7F)) {
    // xor a, m
    regs.A ^= get_mem(code & 0x7F);
    regs.Z = !regs.A;
  } else if (CODE_MASK(0x0B00, 0x7F)) {
    // xor m, a
    int store = regs.A ^ get_mem(code & 0x7F);
    regs.Z = !store;
    ram->write(code & 0x7F, store);
  } else if (CODE_MASK(0x00C0, 0x3F)) {
    // xor io, a
    assert(!"not implemented");
  } else if (code == 0x0068) {
    // not a
    regs.A = ~regs.A;
  } else if (CODE_MASK(0x1400, 0x7F)) {
    // not m
    ram->write(code & 0x7F, ~get_mem(code & 0x7F));
  } else if (code == 0x0069) {
    // neg a
    regs.A = -regs.A;
  } else if (CODE_MASK(0x1480, 0x7F)) {
    // neg m
    ram->write(code & 0x7F, -get_mem(code & 0x7F));
  } else if (CODE_MASK(0x2400, 0x3F)) {
    // set0 m, n
    const u8_t bit = 0x1C0 >> 6;
    const u8_t addr = code & 0x3F;
    ram->write(addr, get_mem(addr) & ~(1 << bit));
  } else if (CODE_MASK(0x2400, 0x3F)) {
    // set1 m, n
    const u8_t bit = 0x1C0 >> 6;
    const u8_t addr = code & 0x3F;
    ram->write(addr, get_mem(addr) | (1 << bit));
  } else
  // TODO: t0sn
  // TODO: t1sn
  if (CODE_MASK(0x2A00, 0xFF)) {
    // ceqsn a, k
    sub_to(regs.A, code & 0xFF);
    if (regs.A == (code & 0xFF))
      ++PC;
  } else if (CODE_MASK(0x1700, 0x7F)) {
    // ceqsn a, m
    int addr = code & 0x7F;
    sub_to(regs.A, get_mem(addr));
    if (regs.A == get_mem(addr))
      ++PC;
  } else if (CODE_MASK(0x2B00, 0xFF)) {
    // cneqsn a, k
    sub_to(regs.A, code & 0xFF);
    if (regs.A != (code & 0xFF))
      ++PC;
  } else if (CODE_MASK(0x1780, 0x7F)) {
    // cneqsn a, m
    int addr = code & 0x7F;
    sub_to(regs.A, get_mem(addr));
    if (regs.A != get_mem(addr))
      ++PC;
  } else if (code == 0x0062) {
    // izsn
    regs.A = add_to(regs.A, 1);
    if (!regs.A)
      ++PC;
  } else if (CODE_MASK(0x1100, 0x7F)) {
    // izsn m
    const int addr = code & 0x7F;
    int result = add_to(get_mem(addr), 1);
    ram->write(addr, result);
    if (!result)
      ++PC;
  } else if (code == 0x0063) {
    // dzsn
    regs.A = sub_to(regs.A, 1);
    if (!regs.A)
      ++PC;
  } else if (CODE_MASK(0x1100, 0x7F)) {
    // dzsn m
    const int addr = code & 0x7F;
    int result = sub_to(get_mem(addr), 1);
    ram->write(addr, result);
    if (!result)
      ++PC;
  } else if (CODE_MASK(0x3800, 0x7FF)) {
    // call k
    ram->write(regs.SP, PC);
    ram->write(regs.SP + 1, PC >> 8);
    PC = code & 0x7FF;
    regs.SP += 2;
  } else if (CODE_MASK(0x3000, 0x7FF)) {
    // goto k
    PC = code & 0x7FF;
  } else if (CODE_MASK(0x0600, 0x7F)) {
    // comp a, m
    sub_to(regs.A, get_mem(code & 0x7F));
  } else if (CODE_MASK(0x0680, 0x7F)) {
    // comp m, a
    sub_to(get_mem(code & 0x7F), regs.A);
  } else if (CODE_MASK( 0x0700, 0x7F)) {
    // nadd a, m
    regs.A = add_to(get_mem(code & 0x7F), -regs.A);
  } else if (CODE_MASK(0x0780, 0x7F)) {
    // nadd m, a
    int addr = code & 0x7F;
    ram->write(addr, add_to(-get_mem(addr), regs.A));
  } else if (code == 0x006E) {
    // swap
    int high = regs.A & 0xF;
    regs.A = (high << 4) | (regs.A >> 4);
  } else if (code == 0x0067) {
    // pcadd
    PC += regs.A;
  }
  // TODO: engint
  // TODO: disint
  // TODO: stopsys
  // TODO: stopexe
  // TODO: reset
  // TODO: wdreset
  // TODO: swapc
  else if (code == 0x0006) {
    // ldsptl
    regs.A = rom->get(regs.SP) & 0xFF;
  } else if (code == 0x0007) {
    // ldspth
    regs.A = (rom->get(regs.SP) & 0xFF00) >> 8;
  }
  else {
    PC = rom->inc_address(PC, -1);

    sim->stop(resINV_INST);
    return (resINV_INST);
  }
  return (resGO);
}

/* End of pdk.src/pdk.cc */
