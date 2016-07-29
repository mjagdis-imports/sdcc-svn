/*
 * Simulator of microcontrollers (cmd.src/newcmdposix.cc)
 *
 * Copyright (C) 1999,99 Drotos Daniel, Talker Bt.
 * Copyright (C) 2006, Borut Razem - borut.razem@siol.net
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

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include "i_string.h"

// prj
#include "globals.h"
#include "utils.h"

// sim
#include "simcl.h"
#include "argcl.h"
#include "appcl.h"

// local
#include "newcmdposixcl.h"


/*
 * Command console
 *____________________________________________________________________________
 */

cl_console::cl_console(const char *_fin, const char *_fout, class cl_app *the_app)
{
  app= the_app;
  fin= 0;
  if (_fin)
    {
      fin= mk_io(_fin, cchars("r"));
    }
  fout= 0;
  if (_fout)
    {
      fout= mk_io(_fout, cchars("w"));
    }
  prompt= 0;
  set_flag(~CONS_NONE, false);
  if ((fin && fin->tty) && (fout && fout->tty))
    {
      set_interactive(true);
      fin->echo(fout);
      fin->cooked();
    }
  else
    ;
  frout= 0;
  id= 0;
  lines_printed= new cl_ustrings(100, 100, "console_cache");
}

cl_console::cl_console(cl_f *_fin, cl_f *_fout, class cl_app *the_app)
{
  app= the_app;
  fin= _fin;
  fout= _fout;
  prompt= 0;
  set_flag(~CONS_NONE, false);
  if ((fin && fin->tty) && (fout && fout->tty))
    {
      set_interactive(true);
      fin->echo(fout);
      fin->cooked();
    }
  else
    ;
  frout= 0;
  id= 0;
  lines_printed= new cl_ustrings(100, 100, "console_cache");
}

class cl_console *
cl_console::clone_for_exec(char *_fin)
{
  class cl_f *fi= 0, *fo= 0;

  if (!_fin)
    return(0);
  if (fi= mk_io(_fin, "r"), !fi)
    {
      fprintf(stderr, "Can't open `%s': %s\n", _fin, strerror(errno));
      return(0);
    }
  
  if ((fo= fout->copy("a")) == 0)
    {
      delete fi;
      fprintf(stderr, "Can't re-open output file: %s\n", strerror(errno));
      return(0);
    }
    
  class cl_console *con= new cl_sub_console(this, fi, fo, app);
  return(con);
}

/*
void
cl_console::set_id(int new_id)
{
  char *s;

  id= new_id;
  set_name(s= format_string("console%d fin=%s,%d fout=%s,%d",
			    id,
			    fin?fin->get_file_name():"",
			    fin?fin->file_id:-1,
			    fout?fout->get_file_name():"",
			    fout?fout->file_id:-1));
  free(s);
}
*/

cl_console::~cl_console(void)
{
  un_redirect();
  if (fout)
    {
      //if (flags & CONS_PROMPT)
      //fout->write_str("\n");
      //fout->flush();
      deb("deleting fout:%d of console %d\n", fout->file_id, id);
      delete fout;
    }
  if (fin)
    {
      deb("deleting fin:%d of console %d\n", fin->file_id, id);
      delete fin;
    }
  delete prompt_option;
  delete null_prompt_option;
  delete debug_option;
}


/*
 * Output functions
 */

void
cl_console::redirect(char *fname, char *mode)
{
  frout= mk_io(fname, mode);
  set_flag(CONS_REDIRECTED, true);
}

void
cl_console::un_redirect(void)
{
  set_flag(CONS_REDIRECTED, false);
  if (!frout)
    return;
  delete frout;
  frout= 0;
}

/*
 * Input functions
 */

bool
cl_console::input_avail(void)
{
  bool ret= false;
  if (input_active())
    {
      ret= fin->input_avail();
      if (ret)
	;
    }
  return ret;
}

bool
cl_console::need_check(void)
{
  return fin && (fin->type == F_CONSOLE);
}

bool
cl_console::set_cooked(bool new_val)
{
  if (!fin)
    return false;
  if (new_val)
    {
      if (!fin->get_cooking())
	fin->interactive(fout);
    }
  else
    {
      // raw
      fin->raw();
    }
  return fin->get_cooking();
}


/* Return
   -1 if EOF (and buffer is empty) or ^C found
   0  buffer is not ready yet
   1  buffer filled (EOL detected)
*/

int
cl_console::read_line(void)
{
  int i= 0;
  char b[2]= { 0, 0 };

  do {
    i= fin->read(b, 1);
    if (i < -1)
      {
	deb("read_line(con=%d,fid=%d) error\n", id, fin->file_id);
	return -1;
      }
    if (i == 0)
      {
	// EOF
	if (lbuf.len() == 0)
	  return -1;
	// execute last line, assuming \n
	return 1;
      }
    if (i > 0)
      {
	// Got a char
	if (b[0] == 3)
	  // ^C, drop and close
	  return -1;
	if ((b[0] == '\n') ||
	    (b[0] == '\r'))
	  {
	    if (nl &&
		(nl != b[0]))
	      {
		deb("readline: skip %d after %d\n", b[0], nl);
		nl= 0;
		continue;
	      }
	    nl= b[0];
	    /*
	    if (lbuf.len() != 0)
	      {
		b[0]= '\n';
		lbuf+= b;
	      }
	    */
	    return 1;
	  }
	lbuf+= b;
      }
  }
  while (i > 0);
  return 0;
}


/*
 * This console listen on a socket and can accept connection requests
 */
//#ifdef SOCKET_AVAIL

cl_listen_console::cl_listen_console(int serverport, class cl_app *the_app)
{
  app= the_app;
  fin= mk_srv(serverport);
  fout= frout= 0;
}

/*
void
cl_listen_console::set_id(int new_id)
{
  char *s;

  id= new_id;
  set_name(s= format_string("listen_console%d port=%d",
			    id,
			    fin?fin->server_port:-1));
  free(s);
}
*/

int
cl_listen_console::proc_input(class cl_cmdset *cmdset)
{
  //int newsock;
  
  class cl_commander_base *cmd;
  cl_f *in, *out;
  
  cmd= app->get_commander();

  srv_accept(fin, &in, &out);
  deb("Listener %d created in:%d out:%d\n", fin->file_id,in->file_id,out->file_id);
  class cl_console_base *c= new cl_console(in, out, app);
  c->set_flag(CONS_INTERACTIVE, true);
  in->interactive(out);
  cmd->add_console(c);
  return(0);
}

//#endif /* SOCKET_AVAIL */


/*
 * Sub-console
 */
/*
cl_sub_console::cl_sub_console(class cl_console_base *the_parent,
                               FILE *fin, FILE *fout, class cl_app *the_app):
  cl_console(fin, fout, the_app)
{
  parent= the_parent;
}
*/
cl_sub_console::cl_sub_console(class cl_console_base *the_parent,
                               class cl_f *fin, class cl_f *fout, class cl_app *the_app):
  cl_console(fin, fout, the_app)
{
  parent= the_parent;
}

cl_sub_console::~cl_sub_console(void)
{
  class cl_commander_base *c= app->get_commander();

  if (parent && c)
    {
      c->activate_console(parent);
    }
}

int
cl_sub_console::init(void)
{
  class cl_commander_base *c= app->get_commander();

  if (parent && c)
    {
      c->deactivate_console(parent);
    }
  cl_console::init();
  set_flag(CONS_ECHO, true);
  return(0);
}

/*
void
cl_sub_console::set_id(int new_id)
{
  char *s;

  id= new_id;
  set_name(s= format_string("sub_console%d (of %d) fin=%s,%d fout=%s,%d",
			    id,
			    parent?parent->get_id():-1,
			    fin?fin->get_file_name():"",
			    fin?fin->file_id:-1,
			    fout?fout->get_file_name():"",
			    fout?fout->file_id:-1));
  free(s);
}
*/

/*
 * Command interpreter
 *____________________________________________________________________________
 */

int
cl_commander::init(void)
{
  class cl_optref console_on_option(this);
  class cl_optref config_file_option(this);
  class cl_optref port_number_option(this);
  class cl_console_base *con;

  console_on_option.init();
  console_on_option.use("console_on");
  config_file_option.init();
  config_file_option.use("config_file");
  port_number_option.init();

  cl_base::init();
  set_name("Commander");
  active_inputs= new cl_list(10, 5, "active_inputs");
  check_list= new cl_list(10, 5, "check_list");
  
  bool need_config= true;

  if (port_number_option.use("port_number"))
    add_console(new cl_listen_console(port_number_option.get_value((long)0), app));

  char *Config= config_file_option.get_value("");
  char *cn= console_on_option.get_value("");

  if (cn)
    {
      if (strcmp(cn, "-") == 0)
	{
	  class cl_f *in, *out;
	  in= cp_io(fileno(stdin), cchars("r"));
	  out= cp_io(fileno(stdout), cchars("w"));
	  in->interactive(out);
	  add_console(con= new cl_console(in, out, app));
	  config_console= exec_on(con, Config);
	  need_config= false;
	  if (in->tty)
	    con->set_flag(CONS_INTERACTIVE, true);
	}
      else
	{
	  add_console(con= new cl_console(cn, cn, app));
	  config_console= exec_on(con, Config);
	  need_config= false;
	}
    }
  if (cons->get_count() == 0)
    {
      class cl_f *in, *out;
      in= cp_io(fileno(stdin), cchars("r"));
      out= cp_io(fileno(stdout), cchars("w"));
      in->interactive(out);
      add_console(con= new cl_console(in, out, app));
      config_console= exec_on(con, Config);
      need_config= false;
      if (in->tty)
	con->set_flag(CONS_INTERACTIVE, true);
    }
  if (need_config &&
      Config &&
      *Config)
    {
      class cl_f *i, *o;
      i= mk_io(Config, "r");
      o= cp_io(fileno(stderr), "w");
      con= new cl_console(/*fc*/i, /*stderr*/o, app);
      con->set_flag(CONS_NOWELCOME|CONS_ECHO, true);
      //exec_on(con, Config);
      config_console= con;
      add_console(con);
    }
  return(0);
}

void
cl_commander::update_active(void)
{
  int i;
  
  active_inputs->disconn_all();
  check_list->disconn_all();

  if (config_console)
    {
      if (!cons->index_of(config_console, NULL))
	config_console= 0;
    }

  //printf("List of active cons: ");
  for (i= 0; i < cons->count; i++)
    {
      class cl_console *c=
	(class cl_console *)cons->at(i);
      class cl_f *f= c->fin;

      if (config_console &&
	  (config_console != c))
	continue;
      
      if (c->input_active() &&
	  f)
	{
	  active_inputs->add(f);
	  //printf("%d,",c->get_id());
	}
      if (c->need_check() &&
	  f)
	check_list->add(f);
    }
  //printf("\n");
  //printf("List of check cons: ");
  /*for (i= 0; i < check_list->count; i++)
    {
      class cl_console *c=
	(class cl_console *)cons->at(i);
      printf("%d,", c->get_id());
      }*/
  //printf("\n");
}

int
cl_commander::input_avail(void)
{
  class cl_list *avail= new cl_list(10,5,"avail");
  bool ret= check_inputs(active_inputs, avail);
  avail->disconn_all();
  delete avail;
  return ret;
}

int
cl_commander::wait_input(void)
{
  while (!input_avail())
    loop_delay();
  return 0;
}

int
cl_commander::proc_input(void)
{
  if (config_console)
    {
      if (!cons->index_of(config_console, NULL))
	config_console= 0;
    }
  
  for (int j = 0; j < cons->count; j++)
    {
      class cl_console *c = dynamic_cast<class cl_console*>((class cl_console_base*)(cons->at(j)));

      if (config_console &&
	  (config_console != c))
	continue;
      
      if (c->input_active())
        {
	  deb("check input on fid=%d\n", c->fin->file_id);
	  if (c->input_avail())
            {
              actual_console = c;
              int retval = c->proc_input(cmdset);
              if (retval)
                {
		  deb("closing console fin-fid=%d\n", c->fin->file_id);
                  del_console(c);
                  //delete c;
                }
              actual_console = 0;
	      int i= consoles_prevent_quit();
	      if (!i)
		deb("no more consoles left\n");
              return(i == 0);
            }
	  else
	    deb("no input on fid=%d\n", c->fin->file_id);
        }
    }
  return 0;
}


void
cl_commander::check(void)
{
  int i;

  for (i= 0; i < check_list->count; i++)
    {
      class cl_f *f= (class cl_f *)check_list->at(i);
      f->check();
    }
}

/* End of cmd.src/newcmdposix.cc */