/*-------------------------------------------------------------------------
  pragma.c - m6502 pragma processing

  Copyright (C) 2026, Gabriele Gorla

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  -------------------------------------------------------------------------*/
#include "common.h"

#include "dbuf_string.h"
#include "m6502.h"

enum
{
  P_CMAP = 1,
  P_BANK,
  P_CODESEG,
  P_CONSTSEG,
};

static int
do_pragma (int id, const char *name, const char *cp)
{
  struct pragma_token_s token;
  int err = 0;
  int processed = 1;

  init_pragma_token (&token);

  switch (id)
    {
    case P_CMAP:
      {
        int src, dst;
     
        err = 1;
        cp = get_pragma_token (cp, &token);
        if(token.type!=TOKEN_INT)
          break;
        src=token.val.int_val;

        cp = get_pragma_token (cp, &token);
        if(token.type!=TOKEN_INT)
          break;
        dst=token.val.int_val;

        cp = get_pragma_token (cp, &token);
        if(token.type!=TOKEN_EOL)
          break;
        err = 0;
        fprintf(stderr, "#pragma charmap %d %d\n",src, dst);
      }
      break;
    case P_BANK:
      {
        struct dbuf_s buffer;

        dbuf_init (&buffer, 128);

        cp = get_pragma_token (cp, &token);

        switch (token.type)
          {
          case TOKEN_EOL:
            err = 1;
            break;

          case TOKEN_INT:
            dbuf_printf (&buffer, "CODE_%d", token.val.int_val);
            break;

          default:
            {
              const char *str = get_pragma_string (&token);

              dbuf_append_str (&buffer, (0 == strcmp ("BASE", str)) ? "HOME" : str);
            }
            break;
          }

        cp = get_pragma_token (cp, &token);
        if (TOKEN_EOL != token.type)
          {
            err = 1;
            break;
          }

        dbuf_c_str (&buffer);
        options.code_seg = (char *) dbuf_detach (&buffer);
      }
      break;

    case P_CODESEG:
    case P_CONSTSEG:
      {
        char *segname;

        cp = get_pragma_token (cp, &token);
        if (token.type == TOKEN_EOL)
          {
            err = 1;
            break;
          }

        segname = Safe_strdup (get_pragma_string (&token));

        cp = get_pragma_token (cp, &token);
        if (token.type != TOKEN_EOL)
          {
            Safe_free (segname);
            err = 1;
            break;
          }

        if (id == P_CODESEG)
          {
            if (options.code_seg)
              Safe_free (options.code_seg);
            options.code_seg = segname;
          }
        else
          {
            if (options.const_seg)
              Safe_free (options.const_seg);
            options.const_seg = segname;
          }
      }
      break;

    default:
      processed = 0;
      break;
    }

  get_pragma_token (cp, &token);

  if (1 == err)
    werror (W_BAD_PRAGMA_ARGUMENTS, name);

  free_pragma_token (&token);
  return processed;
}

static struct pragma_s pragma_tbl[] = {
  {"charmap", P_CMAP, 0, do_pragma},
  {"bank", P_BANK, 0, do_pragma},
  {"codeseg", P_CODESEG, 0, do_pragma},
  {"constseg", P_CONSTSEG, 0, do_pragma},
  {NULL, 0, 0, NULL},
};

int
m6502_process_pragma (const char *s)
{
  return process_pragma_tbl (pragma_tbl, s);
}
