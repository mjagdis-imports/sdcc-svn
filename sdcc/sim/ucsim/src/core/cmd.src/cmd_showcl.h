/*
 * Simulator of microcontrollers (cmd.src/showcl.h)
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

#ifndef CMD_CMD_SHOWCL_HEADER
#define CMD_CMD_SHOWCL_HEADER

#include "newcmdcl.h"


extern void set_show_help(class cl_cmd *cmd);

// SHOW COPYING
COMMAND(cl_show_copying_cmd);

// SHOW WARRANTY
COMMAND(cl_show_warranty_cmd);

// SHOW OPTION
COMMAND_ON(app,cl_show_option_cmd);

// SHOW ERROR
COMMAND_ON(app,cl_show_error_cmd);

// SHOW CONSOLE
COMMAND_ON(app,cl_show_console);


// SHOW REG
COMMAND_ON(uc,cl_show_reg_cmd);


#endif

/* End of cmd.src/cmd_showcl.h */
