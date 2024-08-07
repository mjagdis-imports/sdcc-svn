/*
 * Simulator of microcontrollers (cmd.src/cmdutil.h)
 *
 * Copyright (C) 1999 Drotos Daniel
 * 
 * To contact author send email to dr.dkdb@gmail.com
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

#ifndef CMD_CMDUTIL_HEADER
#define CMD_CMDUTIL_HEADER

#include "ddconfig.h"

#ifdef SOCKET_AVAIL
# include HEADER_SOCKET
#endif


extern char *proc_escape(char *string, int *len);
extern int bool_name(char *s, int *val);

  
#endif

/* End of cmd.src/cmdutil.h */
