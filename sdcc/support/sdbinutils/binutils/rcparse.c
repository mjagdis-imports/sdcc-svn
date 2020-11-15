/* A Bison parser, made by GNU Bison 3.7.3.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "rcparse.y"
 /* rcparse.y -- parser for Windows rc files
   Copyright (C) 1997-2014 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.
   Extended by Kai Tietz, Onevision.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */


/* This is a parser for Windows rc files.  It is based on the parser
   by Gunther Ebert <gunther.ebert@ixos-leipzig.de>.  */

#include "sysdep.h"
#include "bfd.h"
#include "bucomm.h"
#include "libiberty.h"
#include "windres.h"
#include "safe-ctype.h"

/* The current language.  */

static unsigned short language;

/* The resource information during a sub statement.  */

static rc_res_res_info sub_res_info;

/* Dialog information.  This is built by the nonterminals styles and
   controls.  */

static rc_dialog dialog;

/* This is used when building a style.  It is modified by the
   nonterminal styleexpr.  */

static unsigned long style;

/* These are used when building a control.  They are set before using
   control_params.  */

static rc_uint_type base_style;
static rc_uint_type default_style;
static rc_res_id class;
static rc_res_id res_text_field;
static unichar null_unichar;

/* This is used for COMBOBOX, LISTBOX and EDITTEXT which
   do not allow resource 'text' field in control definition. */
static const rc_res_id res_null_text = { 1, {{0, &null_unichar}}};


#line 137 "rcparse.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    BEG = 258,                     /* BEG  */
    END = 259,                     /* END  */
    ACCELERATORS = 260,            /* ACCELERATORS  */
    VIRTKEY = 261,                 /* VIRTKEY  */
    ASCII = 262,                   /* ASCII  */
    NOINVERT = 263,                /* NOINVERT  */
    SHIFT = 264,                   /* SHIFT  */
    CONTROL = 265,                 /* CONTROL  */
    ALT = 266,                     /* ALT  */
    BITMAP = 267,                  /* BITMAP  */
    CURSOR = 268,                  /* CURSOR  */
    DIALOG = 269,                  /* DIALOG  */
    DIALOGEX = 270,                /* DIALOGEX  */
    EXSTYLE = 271,                 /* EXSTYLE  */
    CAPTION = 272,                 /* CAPTION  */
    CLASS = 273,                   /* CLASS  */
    STYLE = 274,                   /* STYLE  */
    AUTO3STATE = 275,              /* AUTO3STATE  */
    AUTOCHECKBOX = 276,            /* AUTOCHECKBOX  */
    AUTORADIOBUTTON = 277,         /* AUTORADIOBUTTON  */
    CHECKBOX = 278,                /* CHECKBOX  */
    COMBOBOX = 279,                /* COMBOBOX  */
    CTEXT = 280,                   /* CTEXT  */
    DEFPUSHBUTTON = 281,           /* DEFPUSHBUTTON  */
    EDITTEXT = 282,                /* EDITTEXT  */
    GROUPBOX = 283,                /* GROUPBOX  */
    LISTBOX = 284,                 /* LISTBOX  */
    LTEXT = 285,                   /* LTEXT  */
    PUSHBOX = 286,                 /* PUSHBOX  */
    PUSHBUTTON = 287,              /* PUSHBUTTON  */
    RADIOBUTTON = 288,             /* RADIOBUTTON  */
    RTEXT = 289,                   /* RTEXT  */
    SCROLLBAR = 290,               /* SCROLLBAR  */
    STATE3 = 291,                  /* STATE3  */
    USERBUTTON = 292,              /* USERBUTTON  */
    BEDIT = 293,                   /* BEDIT  */
    HEDIT = 294,                   /* HEDIT  */
    IEDIT = 295,                   /* IEDIT  */
    FONT = 296,                    /* FONT  */
    ICON = 297,                    /* ICON  */
    ANICURSOR = 298,               /* ANICURSOR  */
    ANIICON = 299,                 /* ANIICON  */
    DLGINCLUDE = 300,              /* DLGINCLUDE  */
    DLGINIT = 301,                 /* DLGINIT  */
    FONTDIR = 302,                 /* FONTDIR  */
    HTML = 303,                    /* HTML  */
    MANIFEST = 304,                /* MANIFEST  */
    PLUGPLAY = 305,                /* PLUGPLAY  */
    VXD = 306,                     /* VXD  */
    TOOLBAR = 307,                 /* TOOLBAR  */
    BUTTON = 308,                  /* BUTTON  */
    LANGUAGE = 309,                /* LANGUAGE  */
    CHARACTERISTICS = 310,         /* CHARACTERISTICS  */
    VERSIONK = 311,                /* VERSIONK  */
    MENU = 312,                    /* MENU  */
    MENUEX = 313,                  /* MENUEX  */
    MENUITEM = 314,                /* MENUITEM  */
    SEPARATOR = 315,               /* SEPARATOR  */
    POPUP = 316,                   /* POPUP  */
    CHECKED = 317,                 /* CHECKED  */
    GRAYED = 318,                  /* GRAYED  */
    HELP = 319,                    /* HELP  */
    INACTIVE = 320,                /* INACTIVE  */
    MENUBARBREAK = 321,            /* MENUBARBREAK  */
    MENUBREAK = 322,               /* MENUBREAK  */
    MESSAGETABLE = 323,            /* MESSAGETABLE  */
    RCDATA = 324,                  /* RCDATA  */
    STRINGTABLE = 325,             /* STRINGTABLE  */
    VERSIONINFO = 326,             /* VERSIONINFO  */
    FILEVERSION = 327,             /* FILEVERSION  */
    PRODUCTVERSION = 328,          /* PRODUCTVERSION  */
    FILEFLAGSMASK = 329,           /* FILEFLAGSMASK  */
    FILEFLAGS = 330,               /* FILEFLAGS  */
    FILEOS = 331,                  /* FILEOS  */
    FILETYPE = 332,                /* FILETYPE  */
    FILESUBTYPE = 333,             /* FILESUBTYPE  */
    BLOCKSTRINGFILEINFO = 334,     /* BLOCKSTRINGFILEINFO  */
    BLOCKVARFILEINFO = 335,        /* BLOCKVARFILEINFO  */
    VALUE = 336,                   /* VALUE  */
    BLOCK = 337,                   /* BLOCK  */
    MOVEABLE = 338,                /* MOVEABLE  */
    FIXED = 339,                   /* FIXED  */
    PURE = 340,                    /* PURE  */
    IMPURE = 341,                  /* IMPURE  */
    PRELOAD = 342,                 /* PRELOAD  */
    LOADONCALL = 343,              /* LOADONCALL  */
    DISCARDABLE = 344,             /* DISCARDABLE  */
    NOT = 345,                     /* NOT  */
    QUOTEDUNISTRING = 346,         /* QUOTEDUNISTRING  */
    QUOTEDSTRING = 347,            /* QUOTEDSTRING  */
    STRING = 348,                  /* STRING  */
    NUMBER = 349,                  /* NUMBER  */
    SIZEDUNISTRING = 350,          /* SIZEDUNISTRING  */
    SIZEDSTRING = 351,             /* SIZEDSTRING  */
    IGNORED_TOKEN = 352,           /* IGNORED_TOKEN  */
    NEG = 353                      /* NEG  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define BEG 258
#define END 259
#define ACCELERATORS 260
#define VIRTKEY 261
#define ASCII 262
#define NOINVERT 263
#define SHIFT 264
#define CONTROL 265
#define ALT 266
#define BITMAP 267
#define CURSOR 268
#define DIALOG 269
#define DIALOGEX 270
#define EXSTYLE 271
#define CAPTION 272
#define CLASS 273
#define STYLE 274
#define AUTO3STATE 275
#define AUTOCHECKBOX 276
#define AUTORADIOBUTTON 277
#define CHECKBOX 278
#define COMBOBOX 279
#define CTEXT 280
#define DEFPUSHBUTTON 281
#define EDITTEXT 282
#define GROUPBOX 283
#define LISTBOX 284
#define LTEXT 285
#define PUSHBOX 286
#define PUSHBUTTON 287
#define RADIOBUTTON 288
#define RTEXT 289
#define SCROLLBAR 290
#define STATE3 291
#define USERBUTTON 292
#define BEDIT 293
#define HEDIT 294
#define IEDIT 295
#define FONT 296
#define ICON 297
#define ANICURSOR 298
#define ANIICON 299
#define DLGINCLUDE 300
#define DLGINIT 301
#define FONTDIR 302
#define HTML 303
#define MANIFEST 304
#define PLUGPLAY 305
#define VXD 306
#define TOOLBAR 307
#define BUTTON 308
#define LANGUAGE 309
#define CHARACTERISTICS 310
#define VERSIONK 311
#define MENU 312
#define MENUEX 313
#define MENUITEM 314
#define SEPARATOR 315
#define POPUP 316
#define CHECKED 317
#define GRAYED 318
#define HELP 319
#define INACTIVE 320
#define MENUBARBREAK 321
#define MENUBREAK 322
#define MESSAGETABLE 323
#define RCDATA 324
#define STRINGTABLE 325
#define VERSIONINFO 326
#define FILEVERSION 327
#define PRODUCTVERSION 328
#define FILEFLAGSMASK 329
#define FILEFLAGS 330
#define FILEOS 331
#define FILETYPE 332
#define FILESUBTYPE 333
#define BLOCKSTRINGFILEINFO 334
#define BLOCKVARFILEINFO 335
#define VALUE 336
#define BLOCK 337
#define MOVEABLE 338
#define FIXED 339
#define PURE 340
#define IMPURE 341
#define PRELOAD 342
#define LOADONCALL 343
#define DISCARDABLE 344
#define NOT 345
#define QUOTEDUNISTRING 346
#define QUOTEDSTRING 347
#define STRING 348
#define NUMBER 349
#define SIZEDUNISTRING 350
#define SIZEDSTRING 351
#define IGNORED_TOKEN 352
#define NEG 353

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 68 "rcparse.y"

  rc_accelerator acc;
  rc_accelerator *pacc;
  rc_dialog_control *dialog_control;
  rc_menuitem *menuitem;
  struct
  {
    rc_rcdata_item *first;
    rc_rcdata_item *last;
  } rcdata;
  rc_rcdata_item *rcdata_item;
  rc_fixed_versioninfo *fixver;
  rc_ver_info *verinfo;
  rc_ver_stringtable *verstringtable;
  rc_ver_stringinfo *verstring;
  rc_ver_varinfo *vervar;
  rc_toolbar_item *toobar_item;
  rc_res_id id;
  rc_res_res_info res_info;
  struct
  {
    rc_uint_type on;
    rc_uint_type off;
  } memflags;
  struct
  {
    rc_uint_type val;
    /* Nonzero if this number was explicitly specified as long.  */
    int dword;
  } i;
  rc_uint_type il;
  rc_uint_type is;
  const char *s;
  struct
  {
    rc_uint_type length;
    const char *s;
  } ss;
  unichar *uni;
  struct
  {
    rc_uint_type length;
    const unichar *s;
  } suni;

#line 431 "rcparse.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_BEG = 3,                        /* BEG  */
  YYSYMBOL_END = 4,                        /* END  */
  YYSYMBOL_ACCELERATORS = 5,               /* ACCELERATORS  */
  YYSYMBOL_VIRTKEY = 6,                    /* VIRTKEY  */
  YYSYMBOL_ASCII = 7,                      /* ASCII  */
  YYSYMBOL_NOINVERT = 8,                   /* NOINVERT  */
  YYSYMBOL_SHIFT = 9,                      /* SHIFT  */
  YYSYMBOL_CONTROL = 10,                   /* CONTROL  */
  YYSYMBOL_ALT = 11,                       /* ALT  */
  YYSYMBOL_BITMAP = 12,                    /* BITMAP  */
  YYSYMBOL_CURSOR = 13,                    /* CURSOR  */
  YYSYMBOL_DIALOG = 14,                    /* DIALOG  */
  YYSYMBOL_DIALOGEX = 15,                  /* DIALOGEX  */
  YYSYMBOL_EXSTYLE = 16,                   /* EXSTYLE  */
  YYSYMBOL_CAPTION = 17,                   /* CAPTION  */
  YYSYMBOL_CLASS = 18,                     /* CLASS  */
  YYSYMBOL_STYLE = 19,                     /* STYLE  */
  YYSYMBOL_AUTO3STATE = 20,                /* AUTO3STATE  */
  YYSYMBOL_AUTOCHECKBOX = 21,              /* AUTOCHECKBOX  */
  YYSYMBOL_AUTORADIOBUTTON = 22,           /* AUTORADIOBUTTON  */
  YYSYMBOL_CHECKBOX = 23,                  /* CHECKBOX  */
  YYSYMBOL_COMBOBOX = 24,                  /* COMBOBOX  */
  YYSYMBOL_CTEXT = 25,                     /* CTEXT  */
  YYSYMBOL_DEFPUSHBUTTON = 26,             /* DEFPUSHBUTTON  */
  YYSYMBOL_EDITTEXT = 27,                  /* EDITTEXT  */
  YYSYMBOL_GROUPBOX = 28,                  /* GROUPBOX  */
  YYSYMBOL_LISTBOX = 29,                   /* LISTBOX  */
  YYSYMBOL_LTEXT = 30,                     /* LTEXT  */
  YYSYMBOL_PUSHBOX = 31,                   /* PUSHBOX  */
  YYSYMBOL_PUSHBUTTON = 32,                /* PUSHBUTTON  */
  YYSYMBOL_RADIOBUTTON = 33,               /* RADIOBUTTON  */
  YYSYMBOL_RTEXT = 34,                     /* RTEXT  */
  YYSYMBOL_SCROLLBAR = 35,                 /* SCROLLBAR  */
  YYSYMBOL_STATE3 = 36,                    /* STATE3  */
  YYSYMBOL_USERBUTTON = 37,                /* USERBUTTON  */
  YYSYMBOL_BEDIT = 38,                     /* BEDIT  */
  YYSYMBOL_HEDIT = 39,                     /* HEDIT  */
  YYSYMBOL_IEDIT = 40,                     /* IEDIT  */
  YYSYMBOL_FONT = 41,                      /* FONT  */
  YYSYMBOL_ICON = 42,                      /* ICON  */
  YYSYMBOL_ANICURSOR = 43,                 /* ANICURSOR  */
  YYSYMBOL_ANIICON = 44,                   /* ANIICON  */
  YYSYMBOL_DLGINCLUDE = 45,                /* DLGINCLUDE  */
  YYSYMBOL_DLGINIT = 46,                   /* DLGINIT  */
  YYSYMBOL_FONTDIR = 47,                   /* FONTDIR  */
  YYSYMBOL_HTML = 48,                      /* HTML  */
  YYSYMBOL_MANIFEST = 49,                  /* MANIFEST  */
  YYSYMBOL_PLUGPLAY = 50,                  /* PLUGPLAY  */
  YYSYMBOL_VXD = 51,                       /* VXD  */
  YYSYMBOL_TOOLBAR = 52,                   /* TOOLBAR  */
  YYSYMBOL_BUTTON = 53,                    /* BUTTON  */
  YYSYMBOL_LANGUAGE = 54,                  /* LANGUAGE  */
  YYSYMBOL_CHARACTERISTICS = 55,           /* CHARACTERISTICS  */
  YYSYMBOL_VERSIONK = 56,                  /* VERSIONK  */
  YYSYMBOL_MENU = 57,                      /* MENU  */
  YYSYMBOL_MENUEX = 58,                    /* MENUEX  */
  YYSYMBOL_MENUITEM = 59,                  /* MENUITEM  */
  YYSYMBOL_SEPARATOR = 60,                 /* SEPARATOR  */
  YYSYMBOL_POPUP = 61,                     /* POPUP  */
  YYSYMBOL_CHECKED = 62,                   /* CHECKED  */
  YYSYMBOL_GRAYED = 63,                    /* GRAYED  */
  YYSYMBOL_HELP = 64,                      /* HELP  */
  YYSYMBOL_INACTIVE = 65,                  /* INACTIVE  */
  YYSYMBOL_MENUBARBREAK = 66,              /* MENUBARBREAK  */
  YYSYMBOL_MENUBREAK = 67,                 /* MENUBREAK  */
  YYSYMBOL_MESSAGETABLE = 68,              /* MESSAGETABLE  */
  YYSYMBOL_RCDATA = 69,                    /* RCDATA  */
  YYSYMBOL_STRINGTABLE = 70,               /* STRINGTABLE  */
  YYSYMBOL_VERSIONINFO = 71,               /* VERSIONINFO  */
  YYSYMBOL_FILEVERSION = 72,               /* FILEVERSION  */
  YYSYMBOL_PRODUCTVERSION = 73,            /* PRODUCTVERSION  */
  YYSYMBOL_FILEFLAGSMASK = 74,             /* FILEFLAGSMASK  */
  YYSYMBOL_FILEFLAGS = 75,                 /* FILEFLAGS  */
  YYSYMBOL_FILEOS = 76,                    /* FILEOS  */
  YYSYMBOL_FILETYPE = 77,                  /* FILETYPE  */
  YYSYMBOL_FILESUBTYPE = 78,               /* FILESUBTYPE  */
  YYSYMBOL_BLOCKSTRINGFILEINFO = 79,       /* BLOCKSTRINGFILEINFO  */
  YYSYMBOL_BLOCKVARFILEINFO = 80,          /* BLOCKVARFILEINFO  */
  YYSYMBOL_VALUE = 81,                     /* VALUE  */
  YYSYMBOL_BLOCK = 82,                     /* BLOCK  */
  YYSYMBOL_MOVEABLE = 83,                  /* MOVEABLE  */
  YYSYMBOL_FIXED = 84,                     /* FIXED  */
  YYSYMBOL_PURE = 85,                      /* PURE  */
  YYSYMBOL_IMPURE = 86,                    /* IMPURE  */
  YYSYMBOL_PRELOAD = 87,                   /* PRELOAD  */
  YYSYMBOL_LOADONCALL = 88,                /* LOADONCALL  */
  YYSYMBOL_DISCARDABLE = 89,               /* DISCARDABLE  */
  YYSYMBOL_NOT = 90,                       /* NOT  */
  YYSYMBOL_QUOTEDUNISTRING = 91,           /* QUOTEDUNISTRING  */
  YYSYMBOL_QUOTEDSTRING = 92,              /* QUOTEDSTRING  */
  YYSYMBOL_STRING = 93,                    /* STRING  */
  YYSYMBOL_NUMBER = 94,                    /* NUMBER  */
  YYSYMBOL_SIZEDUNISTRING = 95,            /* SIZEDUNISTRING  */
  YYSYMBOL_SIZEDSTRING = 96,               /* SIZEDSTRING  */
  YYSYMBOL_IGNORED_TOKEN = 97,             /* IGNORED_TOKEN  */
  YYSYMBOL_98_ = 98,                       /* '|'  */
  YYSYMBOL_99_ = 99,                       /* '^'  */
  YYSYMBOL_100_ = 100,                     /* '&'  */
  YYSYMBOL_101_ = 101,                     /* '+'  */
  YYSYMBOL_102_ = 102,                     /* '-'  */
  YYSYMBOL_103_ = 103,                     /* '*'  */
  YYSYMBOL_104_ = 104,                     /* '/'  */
  YYSYMBOL_105_ = 105,                     /* '%'  */
  YYSYMBOL_106_ = 106,                     /* '~'  */
  YYSYMBOL_NEG = 107,                      /* NEG  */
  YYSYMBOL_108_ = 108,                     /* ','  */
  YYSYMBOL_109_ = 109,                     /* '='  */
  YYSYMBOL_110_ = 110,                     /* '('  */
  YYSYMBOL_111_ = 111,                     /* ')'  */
  YYSYMBOL_YYACCEPT = 112,                 /* $accept  */
  YYSYMBOL_input = 113,                    /* input  */
  YYSYMBOL_accelerator = 114,              /* accelerator  */
  YYSYMBOL_acc_entries = 115,              /* acc_entries  */
  YYSYMBOL_acc_entry = 116,                /* acc_entry  */
  YYSYMBOL_acc_event = 117,                /* acc_event  */
  YYSYMBOL_acc_options = 118,              /* acc_options  */
  YYSYMBOL_acc_option = 119,               /* acc_option  */
  YYSYMBOL_bitmap = 120,                   /* bitmap  */
  YYSYMBOL_cursor = 121,                   /* cursor  */
  YYSYMBOL_dialog = 122,                   /* dialog  */
  YYSYMBOL_123_1 = 123,                    /* $@1  */
  YYSYMBOL_124_2 = 124,                    /* $@2  */
  YYSYMBOL_125_3 = 125,                    /* $@3  */
  YYSYMBOL_exstyle = 126,                  /* exstyle  */
  YYSYMBOL_styles = 127,                   /* styles  */
  YYSYMBOL_controls = 128,                 /* controls  */
  YYSYMBOL_control = 129,                  /* control  */
  YYSYMBOL_130_4 = 130,                    /* $@4  */
  YYSYMBOL_131_5 = 131,                    /* $@5  */
  YYSYMBOL_132_6 = 132,                    /* $@6  */
  YYSYMBOL_133_7 = 133,                    /* $@7  */
  YYSYMBOL_134_8 = 134,                    /* $@8  */
  YYSYMBOL_135_9 = 135,                    /* $@9  */
  YYSYMBOL_136_10 = 136,                   /* $@10  */
  YYSYMBOL_137_11 = 137,                   /* $@11  */
  YYSYMBOL_138_12 = 138,                   /* $@12  */
  YYSYMBOL_139_13 = 139,                   /* $@13  */
  YYSYMBOL_140_14 = 140,                   /* $@14  */
  YYSYMBOL_141_15 = 141,                   /* $@15  */
  YYSYMBOL_142_16 = 142,                   /* $@16  */
  YYSYMBOL_143_17 = 143,                   /* $@17  */
  YYSYMBOL_144_18 = 144,                   /* $@18  */
  YYSYMBOL_145_19 = 145,                   /* $@19  */
  YYSYMBOL_146_20 = 146,                   /* $@20  */
  YYSYMBOL_147_21 = 147,                   /* $@21  */
  YYSYMBOL_148_22 = 148,                   /* $@22  */
  YYSYMBOL_149_23 = 149,                   /* $@23  */
  YYSYMBOL_150_24 = 150,                   /* $@24  */
  YYSYMBOL_control_params = 151,           /* control_params  */
  YYSYMBOL_cresid = 152,                   /* cresid  */
  YYSYMBOL_optresidc = 153,                /* optresidc  */
  YYSYMBOL_resid = 154,                    /* resid  */
  YYSYMBOL_opt_control_data = 155,         /* opt_control_data  */
  YYSYMBOL_control_styleexpr = 156,        /* control_styleexpr  */
  YYSYMBOL_157_25 = 157,                   /* $@25  */
  YYSYMBOL_icon_styleexpr = 158,           /* icon_styleexpr  */
  YYSYMBOL_159_26 = 159,                   /* $@26  */
  YYSYMBOL_control_params_styleexpr = 160, /* control_params_styleexpr  */
  YYSYMBOL_161_27 = 161,                   /* $@27  */
  YYSYMBOL_font = 162,                     /* font  */
  YYSYMBOL_icon = 163,                     /* icon  */
  YYSYMBOL_language = 164,                 /* language  */
  YYSYMBOL_menu = 165,                     /* menu  */
  YYSYMBOL_menuitems = 166,                /* menuitems  */
  YYSYMBOL_menuitem = 167,                 /* menuitem  */
  YYSYMBOL_menuitem_flags = 168,           /* menuitem_flags  */
  YYSYMBOL_menuitem_flag = 169,            /* menuitem_flag  */
  YYSYMBOL_menuex = 170,                   /* menuex  */
  YYSYMBOL_menuexitems = 171,              /* menuexitems  */
  YYSYMBOL_menuexitem = 172,               /* menuexitem  */
  YYSYMBOL_messagetable = 173,             /* messagetable  */
  YYSYMBOL_optrcdata_data = 174,           /* optrcdata_data  */
  YYSYMBOL_175_28 = 175,                   /* $@28  */
  YYSYMBOL_optrcdata_data_int = 176,       /* optrcdata_data_int  */
  YYSYMBOL_rcdata_data = 177,              /* rcdata_data  */
  YYSYMBOL_stringtable = 178,              /* stringtable  */
  YYSYMBOL_179_29 = 179,                   /* $@29  */
  YYSYMBOL_string_data = 180,              /* string_data  */
  YYSYMBOL_rcdata_id = 181,                /* rcdata_id  */
  YYSYMBOL_user = 182,                     /* user  */
  YYSYMBOL_toolbar = 183,                  /* toolbar  */
  YYSYMBOL_toolbar_data = 184,             /* toolbar_data  */
  YYSYMBOL_versioninfo = 185,              /* versioninfo  */
  YYSYMBOL_fixedverinfo = 186,             /* fixedverinfo  */
  YYSYMBOL_verblocks = 187,                /* verblocks  */
  YYSYMBOL_verstringtables = 188,          /* verstringtables  */
  YYSYMBOL_vervals = 189,                  /* vervals  */
  YYSYMBOL_vertrans = 190,                 /* vertrans  */
  YYSYMBOL_id = 191,                       /* id  */
  YYSYMBOL_resname = 192,                  /* resname  */
  YYSYMBOL_resref = 193,                   /* resref  */
  YYSYMBOL_suboptions = 194,               /* suboptions  */
  YYSYMBOL_memflags_move_discard = 195,    /* memflags_move_discard  */
  YYSYMBOL_memflags_move = 196,            /* memflags_move  */
  YYSYMBOL_memflag = 197,                  /* memflag  */
  YYSYMBOL_file_name = 198,                /* file_name  */
  YYSYMBOL_res_unicode_string_concat = 199, /* res_unicode_string_concat  */
  YYSYMBOL_res_unicode_string = 200,       /* res_unicode_string  */
  YYSYMBOL_res_unicode_sizedstring = 201,  /* res_unicode_sizedstring  */
  YYSYMBOL_res_unicode_sizedstring_concat = 202, /* res_unicode_sizedstring_concat  */
  YYSYMBOL_sizedstring = 203,              /* sizedstring  */
  YYSYMBOL_sizedunistring = 204,           /* sizedunistring  */
  YYSYMBOL_styleexpr = 205,                /* styleexpr  */
  YYSYMBOL_parennumber = 206,              /* parennumber  */
  YYSYMBOL_optcnumexpr = 207,              /* optcnumexpr  */
  YYSYMBOL_cnumexpr = 208,                 /* cnumexpr  */
  YYSYMBOL_numexpr = 209,                  /* numexpr  */
  YYSYMBOL_sizednumexpr = 210,             /* sizednumexpr  */
  YYSYMBOL_cposnumexpr = 211,              /* cposnumexpr  */
  YYSYMBOL_posnumexpr = 212,               /* posnumexpr  */
  YYSYMBOL_sizedposnumexpr = 213           /* sizedposnumexpr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   830

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  112
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  276
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  520

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   353


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   105,   100,     2,
     110,   111,   103,   101,   108,   102,     2,   104,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   109,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    99,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    98,     2,   106,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,   107
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   178,   178,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   200,   211,
     214,   235,   240,   252,   272,   282,   286,   291,   298,   302,
     307,   311,   315,   319,   328,   340,   354,   352,   379,   377,
     406,   404,   436,   439,   445,   447,   453,   457,   462,   466,
     470,   483,   498,   513,   528,   532,   536,   540,   546,   548,
     560,   559,   572,   571,   584,   583,   596,   595,   611,   610,
     623,   622,   636,   647,   657,   656,   669,   668,   681,   680,
     693,   692,   705,   704,   719,   724,   730,   736,   743,   742,
     758,   757,   770,   769,   782,   781,   793,   792,   805,   804,
     817,   816,   829,   828,   841,   840,   854,   852,   873,   884,
     895,   907,   918,   921,   925,   930,   940,   943,   953,   952,
     959,   958,   965,   964,   972,   984,   997,  1006,  1017,  1020,
    1037,  1041,  1045,  1053,  1056,  1060,  1067,  1071,  1075,  1079,
    1083,  1087,  1096,  1107,  1110,  1127,  1131,  1135,  1139,  1143,
    1147,  1151,  1155,  1165,  1178,  1178,  1190,  1194,  1201,  1209,
    1217,  1225,  1234,  1243,  1252,  1262,  1261,  1266,  1268,  1273,
    1278,  1286,  1290,  1295,  1300,  1305,  1310,  1315,  1320,  1325,
    1330,  1341,  1348,  1358,  1364,  1365,  1384,  1409,  1420,  1425,
    1432,  1439,  1444,  1449,  1454,  1459,  1474,  1477,  1481,  1489,
    1492,  1500,  1503,  1511,  1514,  1523,  1528,  1537,  1541,  1551,
    1556,  1560,  1571,  1577,  1583,  1588,  1593,  1604,  1609,  1621,
    1626,  1638,  1643,  1648,  1653,  1658,  1663,  1668,  1678,  1682,
    1690,  1695,  1710,  1714,  1723,  1727,  1739,  1744,  1760,  1764,
    1776,  1780,  1802,  1806,  1810,  1814,  1821,  1825,  1835,  1838,
    1847,  1856,  1865,  1869,  1873,  1878,  1883,  1888,  1893,  1898,
    1903,  1908,  1913,  1918,  1929,  1938,  1949,  1953,  1957,  1962,
    1967,  1972,  1977,  1982,  1987,  1992,  1997
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "BEG", "END",
  "ACCELERATORS", "VIRTKEY", "ASCII", "NOINVERT", "SHIFT", "CONTROL",
  "ALT", "BITMAP", "CURSOR", "DIALOG", "DIALOGEX", "EXSTYLE", "CAPTION",
  "CLASS", "STYLE", "AUTO3STATE", "AUTOCHECKBOX", "AUTORADIOBUTTON",
  "CHECKBOX", "COMBOBOX", "CTEXT", "DEFPUSHBUTTON", "EDITTEXT", "GROUPBOX",
  "LISTBOX", "LTEXT", "PUSHBOX", "PUSHBUTTON", "RADIOBUTTON", "RTEXT",
  "SCROLLBAR", "STATE3", "USERBUTTON", "BEDIT", "HEDIT", "IEDIT", "FONT",
  "ICON", "ANICURSOR", "ANIICON", "DLGINCLUDE", "DLGINIT", "FONTDIR",
  "HTML", "MANIFEST", "PLUGPLAY", "VXD", "TOOLBAR", "BUTTON", "LANGUAGE",
  "CHARACTERISTICS", "VERSIONK", "MENU", "MENUEX", "MENUITEM", "SEPARATOR",
  "POPUP", "CHECKED", "GRAYED", "HELP", "INACTIVE", "MENUBARBREAK",
  "MENUBREAK", "MESSAGETABLE", "RCDATA", "STRINGTABLE", "VERSIONINFO",
  "FILEVERSION", "PRODUCTVERSION", "FILEFLAGSMASK", "FILEFLAGS", "FILEOS",
  "FILETYPE", "FILESUBTYPE", "BLOCKSTRINGFILEINFO", "BLOCKVARFILEINFO",
  "VALUE", "BLOCK", "MOVEABLE", "FIXED", "PURE", "IMPURE", "PRELOAD",
  "LOADONCALL", "DISCARDABLE", "NOT", "QUOTEDUNISTRING", "QUOTEDSTRING",
  "STRING", "NUMBER", "SIZEDUNISTRING", "SIZEDSTRING", "IGNORED_TOKEN",
  "'|'", "'^'", "'&'", "'+'", "'-'", "'*'", "'/'", "'%'", "'~'", "NEG",
  "','", "'='", "'('", "')'", "$accept", "input", "accelerator",
  "acc_entries", "acc_entry", "acc_event", "acc_options", "acc_option",
  "bitmap", "cursor", "dialog", "$@1", "$@2", "$@3", "exstyle", "styles",
  "controls", "control", "$@4", "$@5", "$@6", "$@7", "$@8", "$@9", "$@10",
  "$@11", "$@12", "$@13", "$@14", "$@15", "$@16", "$@17", "$@18", "$@19",
  "$@20", "$@21", "$@22", "$@23", "$@24", "control_params", "cresid",
  "optresidc", "resid", "opt_control_data", "control_styleexpr", "$@25",
  "icon_styleexpr", "$@26", "control_params_styleexpr", "$@27", "font",
  "icon", "language", "menu", "menuitems", "menuitem", "menuitem_flags",
  "menuitem_flag", "menuex", "menuexitems", "menuexitem", "messagetable",
  "optrcdata_data", "$@28", "optrcdata_data_int", "rcdata_data",
  "stringtable", "$@29", "string_data", "rcdata_id", "user", "toolbar",
  "toolbar_data", "versioninfo", "fixedverinfo", "verblocks",
  "verstringtables", "vervals", "vertrans", "id", "resname", "resref",
  "suboptions", "memflags_move_discard", "memflags_move", "memflag",
  "file_name", "res_unicode_string_concat", "res_unicode_string",
  "res_unicode_sizedstring", "res_unicode_sizedstring_concat",
  "sizedstring", "sizedunistring", "styleexpr", "parennumber",
  "optcnumexpr", "cnumexpr", "numexpr", "sizednumexpr", "cposnumexpr",
  "posnumexpr", "sizedposnumexpr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   124,    94,
      38,    43,    45,    42,    47,    37,   126,   353,    44,    61,
      40,    41
};
#endif

#define YYPACT_NINF (-446)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-231)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -446,    75,  -446,   317,  -446,  -446,  -446,  -446,  -446,  -446,
     317,   317,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,   463,  -446,  -446,  -446,
     589,  -446,   317,   317,   317,   -93,   626,   209,  -446,   437,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,   317,   317,   317,   317,   317,   317,   317,
     317,  -446,  -446,   526,   317,  -446,   317,   317,   317,   317,
     317,   317,   317,   317,  -446,   317,   317,   317,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,   267,   675,   675,
     275,   275,   675,   675,   491,   404,   441,   675,   168,   256,
     719,   379,   397,   213,   213,  -446,  -446,  -446,  -446,  -446,
     719,   379,   397,   213,   213,  -446,  -446,  -446,  -446,   -93,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,   -65,
     144,   144,  -446,  -446,   -93,  -446,  -446,  -446,  -446,   317,
     317,   317,   317,   317,   317,   317,  -446,  -446,    18,  -446,
      21,   317,   -93,   -93,    31,   140,   155,   126,   -93,   -93,
    -446,  -446,  -446,  -446,  -446,    47,   177,  -446,  -446,   212,
    -446,  -446,  -446,   -34,  -446,  -446,   -93,   -93,  -446,  -446,
     -36,    -5,  -446,  -446,   -25,    -5,  -446,  -446,   119,   131,
     -93,  -446,   -93,  -446,  -446,  -446,  -446,    54,    68,    84,
     626,     2,  -446,     2,    68,    84,   144,    87,   -93,   -93,
      25,  -446,    95,  -446,    -5,  -446,    95,    62,  -446,   102,
     -93,   -93,   177,  -446,  -446,     2,  -446,  -446,   552,  -446,
     -93,  -446,   306,  -446,  -446,  -446,    76,   -93,  -446,     8,
       6,    -5,  -446,  -446,    68,    84,   626,  -446,  -446,  -446,
    -446,  -446,  -446,   167,  -446,  -446,  -446,  -446,  -446,   271,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,   763,  -446,   -93,
     161,  -446,    11,  -446,   197,    -5,   552,  -446,   374,   548,
    -446,   178,  -446,  -446,  -446,   190,  -446,   -93,  -446,     3,
    -446,  -446,   317,    -5,   306,   -47,   317,   317,   317,   317,
     306,  -446,   565,  -446,  -446,   194,   201,    -1,  -446,   -93,
     639,  -446,    -5,  -446,    -5,   143,   -33,  -446,   317,   110,
    -446,   105,   -93,  -446,  -446,  -446,   676,  -446,  -446,  -446,
    -446,    -5,  -446,  -446,   311,   311,   311,   311,   311,  -446,
     311,   311,  -446,   311,  -446,   311,   311,   311,   311,   311,
    -446,   311,   306,   311,   311,   311,   306,  -446,  -446,   104,
     -42,    -5,  -446,  -446,   713,   207,    99,   317,   113,    -5,
    -446,  -446,  -446,  -446,  -446,   317,  -446,  -446,   317,  -446,
     317,  -446,  -446,  -446,  -446,  -446,   317,  -446,   115,   317,
     120,  -446,  -446,  -446,   317,  -446,   -33,  -446,    95,  -446,
    -446,    -5,   152,  -446,   317,   317,   317,   317,  -446,   -93,
     317,   317,  -446,   317,  -446,   317,   317,   317,   317,   317,
    -446,   317,  -446,   153,  -446,   317,   317,   317,   -93,  -446,
     -93,    -5,   311,   159,  -446,  -446,  -446,  -446,   -93,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,   317,  -446,
    -446,  -446,   -93,   -93,  -446,  -446,   -93,   -93,   173,    15,
    -446,   -47,   -93,   -93,   317,  -446,  -446,   -93,   110,   -93,
      27,   180,   244,    29,   -93,  -446,  -446,   -93,   317,  -446,
    -446,  -446,   -93,   -93,   -47,   273,   -93,   192,   -47,   273,
     -93,   273,   -93,   110,  -446,   273,   317,   110,  -446,   273,
    -446,   273,  -446,   193,  -446,  -446,  -446,   -47,   -75,  -446
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,   212,   232,   233,   208,   266,    17,
       0,     0,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    15,    14,    16,     0,   206,   207,   205,
     265,   252,     0,     0,     0,     0,   251,     0,   268,     0,
     212,   219,   217,   219,   219,   217,   217,   179,   180,   177,
     178,   172,   174,   175,   176,   212,   212,   212,   219,   173,
     188,   212,   171,     0,     0,     0,     0,     0,     0,     0,
       0,   255,   254,     0,     0,   126,     0,     0,     0,     0,
       0,     0,     0,     0,   165,     0,     0,     0,   221,   222,
     223,   224,   225,   226,   227,   213,   267,     0,     0,     0,
      42,    42,     0,     0,     0,     0,     0,     0,     0,     0,
     276,   275,   274,   272,   273,   269,   270,   271,   253,   250,
     263,   262,   261,   259,   260,   256,   257,   258,   167,     0,
     214,   216,    19,   228,   229,   220,    34,   218,    35,     0,
       0,     0,   124,   125,     0,   128,   143,   153,   196,     0,
       0,     0,     0,     0,     0,     0,   154,   182,     0,   215,
       0,     0,     0,     0,     0,     0,     0,     0,   248,   248,
     191,   192,   193,   194,   195,     0,   156,   170,   166,     0,
      18,    23,    20,     0,    24,    43,     0,     0,   184,   127,
       0,     0,   129,   142,     0,     0,   144,   187,     0,     0,
     248,   249,   248,   181,   240,   238,   155,   157,   158,   159,
     160,     0,   236,   168,   235,   234,     0,    21,     0,     0,
       0,   131,     0,   230,   133,   148,   145,     0,   199,     0,
     248,   248,   164,   239,   241,   169,   237,   264,     0,    36,
      38,   183,     0,   186,   231,   133,     0,   146,   143,     0,
       0,     0,   189,   190,   161,   162,   163,    28,    29,    30,
      31,    32,    33,    22,    25,    44,    44,    40,   185,   130,
     128,   136,   137,   138,   139,   140,   141,     0,   135,   248,
       0,   143,     0,   197,     0,   203,     0,    27,     0,     0,
      44,     0,   134,   147,   149,     0,   143,   248,   201,     0,
      26,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    58,     0,   132,   150,     0,     0,     0,   198,     0,
       0,    48,    45,    46,    49,   207,     0,   246,     0,    47,
     242,     0,     0,    55,    57,    54,     0,    58,   151,   143,
     200,     0,   204,    37,   112,   112,   112,   112,   112,    70,
     112,   112,    78,   112,    90,   112,   112,   112,   112,   112,
     102,   112,     0,   112,   112,   112,     0,    59,   243,     0,
       0,     0,    56,    39,     0,     0,     0,     0,     0,   115,
     114,    60,    62,    64,    68,     0,    74,    76,     0,    80,
       0,    92,    94,    96,    98,   100,     0,   104,   210,     0,
       0,    66,    82,    88,     0,   247,     0,   244,    50,    41,
     152,     0,     0,   113,     0,     0,     0,     0,    71,     0,
       0,     0,    79,     0,    91,     0,     0,     0,     0,     0,
     103,     0,   211,     0,   209,     0,     0,     0,     0,   245,
      51,   202,     0,     0,    61,    63,    65,    69,     0,    75,
      77,    81,    93,    95,    97,    99,   101,   105,     0,    67,
      83,    89,     0,    52,   111,   118,     0,     0,     0,   116,
      53,     0,     0,     0,     0,   154,    84,     0,   119,     0,
     116,     0,     0,   116,     0,   122,   108,   248,     0,   117,
     120,    85,   248,   248,     0,   116,   249,     0,     0,   116,
     249,   116,   249,   123,   109,   116,     0,   121,    86,   116,
      72,   116,   110,     0,    87,    73,   106,     0,   248,   107
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -236,  -446,  -446,
    -446,  -446,  -446,  -446,   184,  -262,  -273,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,   219,
    -446,   442,  -123,   274,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,    77,  -446,   101,    88,  -446,  -239,
    -446,  -446,  -109,  -446,  -446,  -446,  -446,  -446,  -446,  -446,
    -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,  -446,   -24,
    -245,     4,   169,   211,   270,   710,   175,  -178,     5,  -173,
     157,  -156,  -122,  -445,  -325,  -161,   -30,    -3,    26,  -446,
      20,  -446
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    12,   160,   182,   183,   263,   264,    13,    14,
      15,   265,   266,   290,   140,   288,   320,   367,   414,   415,
     416,   435,   417,   385,   420,   421,   388,   423,   436,   437,
     390,   425,   426,   427,   428,   429,   396,   431,   517,   418,
     443,   377,   378,   476,   466,   471,   492,   498,   487,   494,
      16,    17,    18,    19,   165,   192,   246,   278,    20,   166,
     196,    21,   175,   176,   206,   207,    22,   128,   158,    61,
      23,    24,   220,    25,   108,   167,   250,   317,   299,    26,
      27,   399,    37,    99,    98,    95,   136,   379,   223,   212,
     213,   214,   215,   329,   330,   200,   201,   419,    36,   217,
     380,    30
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      35,   368,    62,   340,   289,    75,    28,   318,   202,   280,
     283,   281,   222,   224,   296,    74,   226,   227,   475,   177,
     208,    29,   178,   370,   221,   180,   478,   287,   312,   241,
     475,    28,   475,    74,   188,   225,    38,    39,   336,   230,
     236,   231,   295,   326,   161,   407,    29,   327,   406,   503,
     300,   203,   327,   507,   209,     5,     6,   315,    71,    72,
      73,   327,   236,   328,   374,   248,     5,     6,   328,   252,
     253,   119,   518,   285,   216,     2,   254,   328,   242,   270,
     341,   439,   129,   130,   131,   243,     5,     6,   284,   110,
     111,   112,   113,   114,   115,   116,   117,   204,   205,   159,
     375,   144,   120,   121,   122,   123,   124,   125,   126,   127,
     255,    74,    31,   181,   164,     8,    74,   398,   293,    74,
      32,   398,   228,    74,    33,   322,   324,    10,    34,     3,
     197,    11,   186,   187,   229,   485,   316,   490,   271,   272,
     273,   274,   275,   276,   189,     4,   168,   169,   170,   171,
     172,   173,   174,     5,     6,   179,   218,   219,   185,   193,
     162,   163,   232,   376,   233,   294,     5,     6,     7,     8,
      74,   148,     9,   257,   258,   259,   260,   261,   262,   234,
     184,    10,   313,   251,   277,    11,     5,     6,   239,   240,
       5,     6,   245,   408,   314,   238,   247,   249,   338,   190,
     298,   191,   210,    74,   339,   198,   199,   411,   370,    97,
     267,   410,    84,   371,   194,   405,   195,   279,   268,   282,
     194,   413,   195,   432,   104,   105,   106,   244,   434,   244,
     109,   244,   244,   441,  -230,  -230,   237,   190,     8,   191,
     149,   150,   151,   152,   153,   154,   155,    28,   489,   194,
      10,   195,   297,   194,    11,   195,   102,   103,   256,   156,
     442,   458,    29,    85,    86,    87,   194,   465,   195,   319,
     132,    31,   204,   205,   138,   286,   475,   142,   143,    32,
     323,   474,   147,    33,   157,   141,   335,    34,   488,   342,
     244,   139,    88,    89,    90,    91,    92,    93,    94,   321,
     506,   516,   372,   331,   332,   333,   334,   204,   205,   325,
      85,    86,    87,   100,   101,    28,    81,    82,    83,   464,
     211,    85,    86,    87,    29,   369,   495,   244,   107,   244,
      29,   499,   501,   271,   272,   273,   274,   275,   276,    88,
      89,    90,    91,    92,    93,    94,   269,   291,   133,   134,
      88,    89,    90,    91,    92,    93,    94,   519,    88,    89,
      90,    91,    92,    93,    94,   292,   482,    28,   235,     0,
     404,    28,     0,     0,   412,     0,     0,   301,   440,   277,
       0,   244,   400,     0,   244,     0,   400,     0,     0,   448,
     302,   303,   304,   305,     0,     0,   433,     5,     6,     7,
       8,   438,     5,     6,     0,     8,     0,   145,   462,     0,
     463,    31,    10,   244,     0,   306,    11,    10,   467,    32,
       0,    11,     0,    33,     0,     0,     0,    34,   307,   308,
     309,   310,   469,   470,     0,     0,   472,   473,     0,   477,
       0,     0,   479,   480,   146,     0,   244,   483,     0,   484,
       0,     0,     0,     0,   493,   468,     0,   496,    85,    86,
      87,     0,   500,   502,     0,     0,   505,     0,    40,     0,
     509,   481,   511,     0,     0,    41,    42,    43,    44,    78,
      79,    80,    81,    82,    83,   497,     0,    88,    89,    90,
      91,    92,    93,    94,     0,    85,    86,    87,    79,    80,
      81,    82,    83,   513,    45,    46,    47,    48,    49,    50,
       0,    51,    52,    53,    54,    55,     0,     0,     0,     0,
      56,    57,     0,     0,    88,    89,    90,    91,    92,    93,
      94,    58,    59,     0,    60,    76,    77,    78,    79,    80,
      81,    82,    83,     0,     0,    85,    86,    87,    96,     0,
       0,   311,     0,     0,     5,     6,     7,     8,   257,   258,
     259,   260,   261,   262,   302,   303,   304,   305,   337,    10,
       0,     0,     0,    11,    88,    89,    90,    91,    92,    93,
      94,   302,   303,   304,   305,    31,     0,     0,     0,   306,
       0,     0,     0,    32,     0,     0,     0,    33,     0,     0,
       0,    34,   307,   308,   309,   310,   306,   422,     0,   424,
       0,     0,     0,     0,     0,   430,     0,     0,     0,   307,
     308,   309,   310,     0,    76,    77,    78,    79,    80,    81,
      82,    83,     0,   444,   445,   446,   447,   118,     0,   449,
     450,     0,   451,   343,   452,   453,   454,   455,   456,   344,
     457,     0,     0,     0,   459,   460,   461,     0,     0,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     373,   366,     0,     0,     0,     0,   344,    63,    64,    65,
      66,    67,    68,    69,    70,     0,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   409,   366,     0,
       0,     0,     0,   344,    76,    77,    78,    79,    80,    81,
      82,    83,     0,   345,   346,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   486,   366,     0,   491,    88,    89,
      90,    91,    92,    93,    94,     0,     0,   133,   134,   504,
       0,     0,     0,   508,     0,   510,     0,     0,     0,   512,
       0,     0,     0,   514,     0,   515,     0,   381,   382,   383,
     384,     0,   386,   387,     0,   389,     0,   391,   392,   393,
     394,   395,     0,   397,     0,   401,   402,   403,   135,   137,
     135,   135,   137,   137,     0,     0,     0,   135,    77,    78,
      79,    80,    81,    82,    83,   271,   272,   273,   274,   275,
     276
};

static const yytype_int16 yycheck[] =
{
       3,   326,    26,     4,   266,    35,     1,     4,   169,   248,
       4,     3,   190,   191,     3,   108,   194,   195,     3,     1,
     176,     1,     4,    98,    60,     4,   471,   263,   290,     4,
       3,    26,     3,   108,     3,    60,    10,    11,   311,   200,
     213,   202,   281,    90,   109,   370,    26,    94,    90,   494,
     286,     4,    94,   498,   176,    91,    92,   296,    32,    33,
      34,    94,   235,   110,   337,     3,    91,    92,   110,   230,
     231,    74,   517,   251,   108,     0,   232,   110,    53,     3,
      81,   406,    85,    86,    87,    60,    91,    92,    82,    63,
      64,    65,    66,    67,    68,    69,    70,    95,    96,   129,
     339,   104,    76,    77,    78,    79,    80,    81,    82,    83,
     232,   108,    94,    92,   144,    94,   108,   362,   279,   108,
     102,   366,     3,   108,   106,   303,   304,   106,   110,    54,
       4,   110,   162,   163,     3,   108,   297,   108,    62,    63,
      64,    65,    66,    67,     4,    70,   149,   150,   151,   152,
     153,   154,   155,    91,    92,   158,   186,   187,   161,     4,
     140,   141,   108,   341,    96,     4,    91,    92,    93,    94,
     108,     3,    97,     6,     7,     8,     9,    10,    11,    95,
     160,   106,     4,    81,   108,   110,    91,    92,   218,   219,
      91,    92,   222,   371,     4,   108,   226,   227,     4,    59,
       3,    61,   176,   108,     3,    79,    80,   108,    98,    40,
     240,     4,     3,   108,    59,   111,    61,   247,   242,   249,
      59,   108,    61,   108,    55,    56,    57,   222,   108,   224,
      61,   226,   227,   411,    91,    92,   216,    59,    94,    61,
      72,    73,    74,    75,    76,    77,    78,   242,     4,    59,
     106,    61,   282,    59,   110,    61,    45,    46,   232,     3,
     108,   108,   242,    54,    55,    56,    59,   108,    61,   299,
       3,    94,    95,    96,    99,   108,     3,   102,   103,   102,
     304,   108,   107,   106,   109,   101,   310,   110,   108,   319,
     285,    16,    83,    84,    85,    86,    87,    88,    89,   302,
     108,   108,   332,   306,   307,   308,   309,    95,    96,   304,
      54,    55,    56,    43,    44,   310,   103,   104,   105,   442,
     108,    54,    55,    56,   304,   328,   487,   322,    58,   324,
     310,   492,   493,    62,    63,    64,    65,    66,    67,    83,
      84,    85,    86,    87,    88,    89,   245,   270,    92,    93,
      83,    84,    85,    86,    87,    88,    89,   518,    83,    84,
      85,    86,    87,    88,    89,   277,   475,   362,   211,    -1,
     366,   366,    -1,    -1,   377,    -1,    -1,     3,   408,   108,
      -1,   376,   362,    -1,   379,    -1,   366,    -1,    -1,   419,
      16,    17,    18,    19,    -1,    -1,   399,    91,    92,    93,
      94,   404,    91,    92,    -1,    94,    -1,     3,   438,    -1,
     440,    94,   106,   408,    -1,    41,   110,   106,   448,   102,
      -1,   110,    -1,   106,    -1,    -1,    -1,   110,    54,    55,
      56,    57,   462,   463,    -1,    -1,   466,   467,    -1,   469,
      -1,    -1,   472,   473,     3,    -1,   441,   477,    -1,   479,
      -1,    -1,    -1,    -1,   484,   458,    -1,   487,    54,    55,
      56,    -1,   492,   493,    -1,    -1,   496,    -1,     5,    -1,
     500,   474,   502,    -1,    -1,    12,    13,    14,    15,   100,
     101,   102,   103,   104,   105,   488,    -1,    83,    84,    85,
      86,    87,    88,    89,    -1,    54,    55,    56,   101,   102,
     103,   104,   105,   506,    41,    42,    43,    44,    45,    46,
      -1,    48,    49,    50,    51,    52,    -1,    -1,    -1,    -1,
      57,    58,    -1,    -1,    83,    84,    85,    86,    87,    88,
      89,    68,    69,    -1,    71,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,    -1,    54,    55,    56,   111,    -1,
      -1,     3,    -1,    -1,    91,    92,    93,    94,     6,     7,
       8,     9,    10,    11,    16,    17,    18,    19,     3,   106,
      -1,    -1,    -1,   110,    83,    84,    85,    86,    87,    88,
      89,    16,    17,    18,    19,    94,    -1,    -1,    -1,    41,
      -1,    -1,    -1,   102,    -1,    -1,    -1,   106,    -1,    -1,
      -1,   110,    54,    55,    56,    57,    41,   388,    -1,   390,
      -1,    -1,    -1,    -1,    -1,   396,    -1,    -1,    -1,    54,
      55,    56,    57,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,   414,   415,   416,   417,   111,    -1,   420,
     421,    -1,   423,     4,   425,   426,   427,   428,   429,    10,
     431,    -1,    -1,    -1,   435,   436,   437,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
       4,    42,    -1,    -1,    -1,    -1,    10,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,     4,    42,    -1,
      -1,    -1,    -1,    10,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,   480,    42,    -1,   483,    83,    84,
      85,    86,    87,    88,    89,    -1,    -1,    92,    93,   495,
      -1,    -1,    -1,   499,    -1,   501,    -1,    -1,    -1,   505,
      -1,    -1,    -1,   509,    -1,   511,    -1,   345,   346,   347,
     348,    -1,   350,   351,    -1,   353,    -1,   355,   356,   357,
     358,   359,    -1,   361,    -1,   363,   364,   365,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,   107,    99,   100,
     101,   102,   103,   104,   105,    62,    63,    64,    65,    66,
      67
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   113,     0,    54,    70,    91,    92,    93,    94,    97,
     106,   110,   114,   120,   121,   122,   162,   163,   164,   165,
     170,   173,   178,   182,   183,   185,   191,   192,   200,   212,
     213,    94,   102,   106,   110,   209,   210,   194,   210,   210,
       5,    12,    13,    14,    15,    41,    42,    43,    44,    45,
      46,    48,    49,    50,    51,    52,    57,    58,    68,    69,
      71,   181,   191,    98,    99,   100,   101,   102,   103,   104,
     105,   210,   210,   210,   108,   208,    98,    99,   100,   101,
     102,   103,   104,   105,     3,    54,    55,    56,    83,    84,
      85,    86,    87,    88,    89,   197,   111,   194,   196,   195,
     196,   196,   195,   195,   194,   194,   194,   196,   186,   194,
     210,   210,   210,   210,   210,   210,   210,   210,   111,   209,
     210,   210,   210,   210,   210,   210,   210,   210,   179,   209,
     209,   209,     3,    92,    93,   197,   198,   197,   198,    16,
     126,   126,   198,   198,   209,     3,     3,   198,     3,    72,
      73,    74,    75,    76,    77,    78,     3,   198,   180,   208,
     115,   109,   212,   212,   208,   166,   171,   187,   209,   209,
     209,   209,   209,   209,   209,   174,   175,     1,     4,   209,
       4,    92,   116,   117,   212,   209,   208,   208,     3,     4,
      59,    61,   167,     4,    59,    61,   172,     4,    79,    80,
     207,   208,   207,     4,    95,    96,   176,   177,   203,   204,
     210,   108,   201,   202,   203,   204,   108,   211,   208,   208,
     184,    60,   199,   200,   199,    60,   199,   199,     3,     3,
     207,   207,   108,    96,    95,   202,   201,   212,   108,   208,
     208,     4,    53,    60,   200,   208,   168,   208,     3,   208,
     188,    81,   207,   207,   203,   204,   210,     6,     7,     8,
       9,    10,    11,   118,   119,   123,   124,   208,   191,   168,
       3,    62,    63,    64,    65,    66,    67,   108,   169,   208,
     171,     3,   208,     4,    82,   199,   108,   119,   127,   127,
     125,   166,   169,   207,     4,   171,     3,   208,     3,   190,
     119,     3,    16,    17,    18,    19,    41,    54,    55,    56,
      57,     3,   127,     4,     4,   171,   207,   189,     4,   208,
     128,   209,   199,   191,   199,   200,    90,    94,   110,   205,
     206,   209,   209,   209,   209,   191,   128,     3,     4,     3,
       4,    81,   208,     4,    10,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    42,   129,   206,   209,
      98,   108,   208,     4,   128,   171,   199,   153,   154,   199,
     212,   153,   153,   153,   153,   135,   153,   153,   138,   153,
     142,   153,   153,   153,   153,   153,   148,   153,   192,   193,
     212,   153,   153,   153,   193,   111,    90,   206,   199,     4,
       4,   108,   209,   108,   130,   131,   132,   134,   151,   209,
     136,   137,   151,   139,   151,   143,   144,   145,   146,   147,
     151,   149,   108,   209,   108,   133,   140,   141,   209,   206,
     208,   199,   108,   152,   151,   151,   151,   151,   208,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   108,   151,
     151,   151,   208,   208,   154,   108,   156,   208,   209,   208,
     208,   157,   208,   208,   108,     3,   155,   208,   205,   208,
     208,   209,   174,   208,   208,   108,   155,   160,   108,     4,
     108,   155,   158,   208,   161,   207,   208,   209,   159,   207,
     208,   207,   208,   205,   155,   208,   108,   205,   155,   208,
     155,   208,   155,   209,   155,   155,   108,   150,   205,   207
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   112,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   114,   115,
     115,   116,   116,   117,   117,   118,   118,   118,   119,   119,
     119,   119,   119,   119,   120,   121,   123,   122,   124,   122,
     125,   122,   126,   126,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   128,   128,
     130,   129,   131,   129,   132,   129,   133,   129,   134,   129,
     135,   129,   129,   129,   136,   129,   137,   129,   138,   129,
     139,   129,   140,   129,   129,   129,   129,   129,   141,   129,
     142,   129,   143,   129,   144,   129,   145,   129,   146,   129,
     147,   129,   148,   129,   149,   129,   150,   129,   151,   151,
     151,   152,   153,   153,   154,   154,   155,   155,   157,   156,
     159,   158,   161,   160,   162,   163,   164,   165,   166,   166,
     167,   167,   167,   168,   168,   168,   169,   169,   169,   169,
     169,   169,   170,   171,   171,   172,   172,   172,   172,   172,
     172,   172,   172,   173,   175,   174,   176,   176,   177,   177,
     177,   177,   177,   177,   177,   179,   178,   180,   180,   180,
     180,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   182,   182,   183,   184,   184,   184,   185,   186,   186,
     186,   186,   186,   186,   186,   186,   187,   187,   187,   188,
     188,   189,   189,   190,   190,   191,   191,   192,   192,   193,
     193,   193,   194,   194,   194,   194,   194,   195,   195,   196,
     196,   197,   197,   197,   197,   197,   197,   197,   198,   198,
     199,   199,   200,   200,   201,   201,   202,   202,   203,   203,
     204,   204,   205,   205,   205,   205,   206,   206,   207,   207,
     208,   209,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   211,   212,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     6,     0,
       2,     2,     4,     1,     1,     1,     3,     2,     1,     1,
       1,     1,     1,     1,     4,     4,     0,    13,     0,    13,
       0,    14,     0,     3,     0,     3,     3,     3,     3,     3,
       5,     6,     7,     8,     3,     3,     4,     3,     0,     2,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     3,    11,    12,     0,     4,     0,     4,     0,     3,
       0,     4,     0,     4,     6,     8,    10,    11,     0,     4,
       0,     3,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     3,     0,     4,     0,    15,     6,     8,
       9,     2,     0,     2,     1,     1,     0,     3,     0,     3,
       0,     3,     0,     3,     4,     4,     3,     6,     0,     2,
       4,     2,     6,     0,     3,     2,     1,     1,     1,     1,
       1,     1,     6,     0,     2,     2,     3,     5,     2,     5,
       6,     7,     9,     4,     0,     2,     0,     1,     1,     1,
       1,     3,     3,     3,     2,     0,     6,     0,     3,     4,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     8,     0,     3,     2,     6,     0,     6,
       6,     3,     3,     3,     3,     3,     0,     5,     7,     0,
       5,     0,     5,     0,     3,     1,     1,     1,     1,     2,
       1,     2,     0,     2,     3,     4,     3,     0,     2,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     2,     1,     2,     3,     4,     1,     3,     0,     1,
       2,     1,     1,     3,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     1,     1,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 18: /* accelerator: id ACCELERATORS suboptions BEG acc_entries END  */
#line 201 "rcparse.y"
          {
	    define_accelerator ((yyvsp[-5].id), &(yyvsp[-3].res_info), (yyvsp[-1].pacc));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2084 "rcparse.c"
    break;

  case 19: /* acc_entries: %empty  */
#line 211 "rcparse.y"
          {
	    (yyval.pacc) = NULL;
	  }
#line 2092 "rcparse.c"
    break;

  case 20: /* acc_entries: acc_entries acc_entry  */
#line 215 "rcparse.y"
          {
	    rc_accelerator *a;

	    a = (rc_accelerator *) res_alloc (sizeof *a);
	    *a = (yyvsp[0].acc);
	    if ((yyvsp[-1].pacc) == NULL)
	      (yyval.pacc) = a;
	    else
	      {
		rc_accelerator **pp;

		for (pp = &(yyvsp[-1].pacc)->next; *pp != NULL; pp = &(*pp)->next)
		  ;
		*pp = a;
		(yyval.pacc) = (yyvsp[-1].pacc);
	      }
	  }
#line 2114 "rcparse.c"
    break;

  case 21: /* acc_entry: acc_event cposnumexpr  */
#line 236 "rcparse.y"
          {
	    (yyval.acc) = (yyvsp[-1].acc);
	    (yyval.acc).id = (yyvsp[0].il);
	  }
#line 2123 "rcparse.c"
    break;

  case 22: /* acc_entry: acc_event cposnumexpr ',' acc_options  */
#line 241 "rcparse.y"
          {
	    (yyval.acc) = (yyvsp[-3].acc);
	    (yyval.acc).id = (yyvsp[-2].il);
	    (yyval.acc).flags |= (yyvsp[0].is);
	    if (((yyval.acc).flags & ACC_VIRTKEY) == 0
		&& ((yyval.acc).flags & (ACC_SHIFT | ACC_CONTROL)) != 0)
	      rcparse_warning (_("inappropriate modifiers for non-VIRTKEY"));
	  }
#line 2136 "rcparse.c"
    break;

  case 23: /* acc_event: QUOTEDSTRING  */
#line 253 "rcparse.y"
          {
	    const char *s = (yyvsp[0].s);
	    char ch;

	    (yyval.acc).next = NULL;
	    (yyval.acc).id = 0;
	    ch = *s;
	    if (ch != '^')
	      (yyval.acc).flags = 0;
	    else
	      {
		(yyval.acc).flags = ACC_CONTROL | ACC_VIRTKEY;
		++s;
		ch = TOUPPER (s[0]);
	      }
	    (yyval.acc).key = ch;
	    if (s[1] != '\0')
	      rcparse_warning (_("accelerator should only be one character"));
	  }
#line 2160 "rcparse.c"
    break;

  case 24: /* acc_event: posnumexpr  */
#line 273 "rcparse.y"
          {
	    (yyval.acc).next = NULL;
	    (yyval.acc).flags = 0;
	    (yyval.acc).id = 0;
	    (yyval.acc).key = (yyvsp[0].il);
	  }
#line 2171 "rcparse.c"
    break;

  case 25: /* acc_options: acc_option  */
#line 283 "rcparse.y"
          {
	    (yyval.is) = (yyvsp[0].is);
	  }
#line 2179 "rcparse.c"
    break;

  case 26: /* acc_options: acc_options ',' acc_option  */
#line 287 "rcparse.y"
          {
	    (yyval.is) = (yyvsp[-2].is) | (yyvsp[0].is);
	  }
#line 2187 "rcparse.c"
    break;

  case 27: /* acc_options: acc_options acc_option  */
#line 292 "rcparse.y"
          {
	    (yyval.is) = (yyvsp[-1].is) | (yyvsp[0].is);
	  }
#line 2195 "rcparse.c"
    break;

  case 28: /* acc_option: VIRTKEY  */
#line 299 "rcparse.y"
          {
	    (yyval.is) = ACC_VIRTKEY;
	  }
#line 2203 "rcparse.c"
    break;

  case 29: /* acc_option: ASCII  */
#line 303 "rcparse.y"
          {
	    /* This is just the absence of VIRTKEY.  */
	    (yyval.is) = 0;
	  }
#line 2212 "rcparse.c"
    break;

  case 30: /* acc_option: NOINVERT  */
#line 308 "rcparse.y"
          {
	    (yyval.is) = ACC_NOINVERT;
	  }
#line 2220 "rcparse.c"
    break;

  case 31: /* acc_option: SHIFT  */
#line 312 "rcparse.y"
          {
	    (yyval.is) = ACC_SHIFT;
	  }
#line 2228 "rcparse.c"
    break;

  case 32: /* acc_option: CONTROL  */
#line 316 "rcparse.y"
          {
	    (yyval.is) = ACC_CONTROL;
	  }
#line 2236 "rcparse.c"
    break;

  case 33: /* acc_option: ALT  */
#line 320 "rcparse.y"
          {
	    (yyval.is) = ACC_ALT;
	  }
#line 2244 "rcparse.c"
    break;

  case 34: /* bitmap: id BITMAP memflags_move file_name  */
#line 329 "rcparse.y"
          {
	    define_bitmap ((yyvsp[-3].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2255 "rcparse.c"
    break;

  case 35: /* cursor: id CURSOR memflags_move_discard file_name  */
#line 341 "rcparse.y"
          {
	    define_cursor ((yyvsp[-3].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2266 "rcparse.c"
    break;

  case 36: /* $@1: %empty  */
#line 354 "rcparse.y"
            {
	      memset (&dialog, 0, sizeof dialog);
	      dialog.x = (yyvsp[-3].il);
	      dialog.y = (yyvsp[-2].il);
	      dialog.width = (yyvsp[-1].il);
	      dialog.height = (yyvsp[0].il);
	      dialog.style = WS_POPUP | WS_BORDER | WS_SYSMENU;
	      dialog.exstyle = (yyvsp[-4].il);
	      dialog.menu.named = 1;
	      dialog.class.named = 1;
	      dialog.font = NULL;
	      dialog.ex = NULL;
	      dialog.controls = NULL;
	      sub_res_info = (yyvsp[-5].res_info);
	      style = 0;
	    }
#line 2287 "rcparse.c"
    break;

  case 37: /* dialog: id DIALOG memflags_move exstyle posnumexpr cnumexpr cnumexpr cnumexpr $@1 styles BEG controls END  */
#line 371 "rcparse.y"
          {
	    define_dialog ((yyvsp[-12].id), &sub_res_info, &dialog);
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2298 "rcparse.c"
    break;

  case 38: /* $@2: %empty  */
#line 379 "rcparse.y"
            {
	      memset (&dialog, 0, sizeof dialog);
	      dialog.x = (yyvsp[-3].il);
	      dialog.y = (yyvsp[-2].il);
	      dialog.width = (yyvsp[-1].il);
	      dialog.height = (yyvsp[0].il);
	      dialog.style = WS_POPUP | WS_BORDER | WS_SYSMENU;
	      dialog.exstyle = (yyvsp[-4].il);
	      dialog.menu.named = 1;
	      dialog.class.named = 1;
	      dialog.font = NULL;
	      dialog.ex = ((rc_dialog_ex *)
			   res_alloc (sizeof (rc_dialog_ex)));
	      memset (dialog.ex, 0, sizeof (rc_dialog_ex));
	      dialog.controls = NULL;
	      sub_res_info = (yyvsp[-5].res_info);
	      style = 0;
	    }
#line 2321 "rcparse.c"
    break;

  case 39: /* dialog: id DIALOGEX memflags_move exstyle posnumexpr cnumexpr cnumexpr cnumexpr $@2 styles BEG controls END  */
#line 398 "rcparse.y"
          {
	    define_dialog ((yyvsp[-12].id), &sub_res_info, &dialog);
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2332 "rcparse.c"
    break;

  case 40: /* $@3: %empty  */
#line 406 "rcparse.y"
            {
	      memset (&dialog, 0, sizeof dialog);
	      dialog.x = (yyvsp[-4].il);
	      dialog.y = (yyvsp[-3].il);
	      dialog.width = (yyvsp[-2].il);
	      dialog.height = (yyvsp[-1].il);
	      dialog.style = WS_POPUP | WS_BORDER | WS_SYSMENU;
	      dialog.exstyle = (yyvsp[-5].il);
	      dialog.menu.named = 1;
	      dialog.class.named = 1;
	      dialog.font = NULL;
	      dialog.ex = ((rc_dialog_ex *)
			   res_alloc (sizeof (rc_dialog_ex)));
	      memset (dialog.ex, 0, sizeof (rc_dialog_ex));
	      dialog.ex->help = (yyvsp[0].il);
	      dialog.controls = NULL;
	      sub_res_info = (yyvsp[-6].res_info);
	      style = 0;
	    }
#line 2356 "rcparse.c"
    break;

  case 41: /* dialog: id DIALOGEX memflags_move exstyle posnumexpr cnumexpr cnumexpr cnumexpr cnumexpr $@3 styles BEG controls END  */
#line 426 "rcparse.y"
          {
	    define_dialog ((yyvsp[-13].id), &sub_res_info, &dialog);
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 2367 "rcparse.c"
    break;

  case 42: /* exstyle: %empty  */
#line 436 "rcparse.y"
          {
	    (yyval.il) = 0;
	  }
#line 2375 "rcparse.c"
    break;

  case 43: /* exstyle: EXSTYLE '=' numexpr  */
#line 440 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].il);
	  }
#line 2383 "rcparse.c"
    break;

  case 45: /* styles: styles CAPTION res_unicode_string_concat  */
#line 448 "rcparse.y"
          {
	    dialog.style |= WS_CAPTION;
	    style |= WS_CAPTION;
	    dialog.caption = (yyvsp[0].uni);
	  }
#line 2393 "rcparse.c"
    break;

  case 46: /* styles: styles CLASS id  */
#line 454 "rcparse.y"
          {
	    dialog.class = (yyvsp[0].id);
	  }
#line 2401 "rcparse.c"
    break;

  case 47: /* styles: styles STYLE styleexpr  */
#line 459 "rcparse.y"
          {
	    dialog.style = style;
	  }
#line 2409 "rcparse.c"
    break;

  case 48: /* styles: styles EXSTYLE numexpr  */
#line 463 "rcparse.y"
          {
	    dialog.exstyle = (yyvsp[0].il);
	  }
#line 2417 "rcparse.c"
    break;

  case 49: /* styles: styles CLASS res_unicode_string_concat  */
#line 467 "rcparse.y"
          {
	    res_unistring_to_id (& dialog.class, (yyvsp[0].uni));
	  }
#line 2425 "rcparse.c"
    break;

  case 50: /* styles: styles FONT numexpr ',' res_unicode_string_concat  */
#line 471 "rcparse.y"
          {
	    dialog.style |= DS_SETFONT;
	    style |= DS_SETFONT;
	    dialog.pointsize = (yyvsp[-2].il);
	    dialog.font = (yyvsp[0].uni);
	    if (dialog.ex != NULL)
	      {
		dialog.ex->weight = 0;
		dialog.ex->italic = 0;
		dialog.ex->charset = 1;
	      }
	  }
#line 2442 "rcparse.c"
    break;

  case 51: /* styles: styles FONT numexpr ',' res_unicode_string_concat cnumexpr  */
#line 484 "rcparse.y"
          {
	    dialog.style |= DS_SETFONT;
	    style |= DS_SETFONT;
	    dialog.pointsize = (yyvsp[-3].il);
	    dialog.font = (yyvsp[-1].uni);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("extended FONT requires DIALOGEX"));
	    else
	      {
		dialog.ex->weight = (yyvsp[0].il);
		dialog.ex->italic = 0;
		dialog.ex->charset = 1;
	      }
	  }
#line 2461 "rcparse.c"
    break;

  case 52: /* styles: styles FONT numexpr ',' res_unicode_string_concat cnumexpr cnumexpr  */
#line 499 "rcparse.y"
          {
	    dialog.style |= DS_SETFONT;
	    style |= DS_SETFONT;
	    dialog.pointsize = (yyvsp[-4].il);
	    dialog.font = (yyvsp[-2].uni);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("extended FONT requires DIALOGEX"));
	    else
	      {
		dialog.ex->weight = (yyvsp[-1].il);
		dialog.ex->italic = (yyvsp[0].il);
		dialog.ex->charset = 1;
	      }
	  }
#line 2480 "rcparse.c"
    break;

  case 53: /* styles: styles FONT numexpr ',' res_unicode_string_concat cnumexpr cnumexpr cnumexpr  */
#line 514 "rcparse.y"
          {
	    dialog.style |= DS_SETFONT;
	    style |= DS_SETFONT;
	    dialog.pointsize = (yyvsp[-5].il);
	    dialog.font = (yyvsp[-3].uni);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("extended FONT requires DIALOGEX"));
	    else
	      {
		dialog.ex->weight = (yyvsp[-2].il);
		dialog.ex->italic = (yyvsp[-1].il);
		dialog.ex->charset = (yyvsp[0].il);
	      }
	  }
#line 2499 "rcparse.c"
    break;

  case 54: /* styles: styles MENU id  */
#line 529 "rcparse.y"
          {
	    dialog.menu = (yyvsp[0].id);
	  }
#line 2507 "rcparse.c"
    break;

  case 55: /* styles: styles CHARACTERISTICS numexpr  */
#line 533 "rcparse.y"
          {
	    sub_res_info.characteristics = (yyvsp[0].il);
	  }
#line 2515 "rcparse.c"
    break;

  case 56: /* styles: styles LANGUAGE numexpr cnumexpr  */
#line 537 "rcparse.y"
          {
	    sub_res_info.language = (yyvsp[-1].il) | ((yyvsp[0].il) << SUBLANG_SHIFT);
	  }
#line 2523 "rcparse.c"
    break;

  case 57: /* styles: styles VERSIONK numexpr  */
#line 541 "rcparse.y"
          {
	    sub_res_info.version = (yyvsp[0].il);
	  }
#line 2531 "rcparse.c"
    break;

  case 59: /* controls: controls control  */
#line 549 "rcparse.y"
          {
	    rc_dialog_control **pp;

	    for (pp = &dialog.controls; *pp != NULL; pp = &(*pp)->next)
	      ;
	    *pp = (yyvsp[0].dialog_control);
	  }
#line 2543 "rcparse.c"
    break;

  case 60: /* $@4: %empty  */
#line 560 "rcparse.y"
            {
	      default_style = BS_AUTO3STATE | WS_TABSTOP;
	      base_style = BS_AUTO3STATE;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2555 "rcparse.c"
    break;

  case 61: /* control: AUTO3STATE optresidc $@4 control_params  */
#line 568 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2563 "rcparse.c"
    break;

  case 62: /* $@5: %empty  */
#line 572 "rcparse.y"
            {
	      default_style = BS_AUTOCHECKBOX | WS_TABSTOP;
	      base_style = BS_AUTOCHECKBOX;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2575 "rcparse.c"
    break;

  case 63: /* control: AUTOCHECKBOX optresidc $@5 control_params  */
#line 580 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2583 "rcparse.c"
    break;

  case 64: /* $@6: %empty  */
#line 584 "rcparse.y"
            {
	      default_style = BS_AUTORADIOBUTTON | WS_TABSTOP;
	      base_style = BS_AUTORADIOBUTTON;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2595 "rcparse.c"
    break;

  case 65: /* control: AUTORADIOBUTTON optresidc $@6 control_params  */
#line 592 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2603 "rcparse.c"
    break;

  case 66: /* $@7: %empty  */
#line 596 "rcparse.y"
            {
	      default_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      base_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_EDIT;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2615 "rcparse.c"
    break;

  case 67: /* control: BEDIT optresidc $@7 control_params  */
#line 604 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("BEDIT requires DIALOGEX"));
	    res_string_to_id (&(yyval.dialog_control)->class, "BEDIT");
	  }
#line 2626 "rcparse.c"
    break;

  case 68: /* $@8: %empty  */
#line 611 "rcparse.y"
            {
	      default_style = BS_CHECKBOX | WS_TABSTOP;
	      base_style = BS_CHECKBOX | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2638 "rcparse.c"
    break;

  case 69: /* control: CHECKBOX optresidc $@8 control_params  */
#line 619 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2646 "rcparse.c"
    break;

  case 70: /* $@9: %empty  */
#line 623 "rcparse.y"
            {
	      /* This is as per MSDN documentation.  With some (???)
		 versions of MS rc.exe their is no default style.  */
	      default_style = CBS_SIMPLE | WS_TABSTOP;
	      base_style = 0;
	      class.named = 0;
	      class.u.id = CTL_COMBOBOX;
	      res_text_field = res_null_text;	
	    }
#line 2660 "rcparse.c"
    break;

  case 71: /* control: COMBOBOX $@9 control_params  */
#line 633 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2668 "rcparse.c"
    break;

  case 72: /* control: CONTROL optresidc numexpr cresid control_styleexpr cnumexpr cnumexpr cnumexpr cnumexpr optcnumexpr opt_control_data  */
#line 638 "rcparse.y"
          {
	    (yyval.dialog_control) = define_control ((yyvsp[-9].id), (yyvsp[-8].il), (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), (yyvsp[-2].il), (yyvsp[-7].id), style, (yyvsp[-1].il));
	    if ((yyvsp[0].rcdata_item) != NULL)
	      {
		if (dialog.ex == NULL)
		  rcparse_warning (_("control data requires DIALOGEX"));
		(yyval.dialog_control)->data = (yyvsp[0].rcdata_item);
	      }
	  }
#line 2682 "rcparse.c"
    break;

  case 73: /* control: CONTROL optresidc numexpr cresid control_styleexpr cnumexpr cnumexpr cnumexpr cnumexpr cnumexpr cnumexpr opt_control_data  */
#line 649 "rcparse.y"
          {
	    (yyval.dialog_control) = define_control ((yyvsp[-10].id), (yyvsp[-9].il), (yyvsp[-6].il), (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), (yyvsp[-8].id), style, (yyvsp[-2].il));
	    if (dialog.ex == NULL)
	      rcparse_warning (_("help ID requires DIALOGEX"));
	    (yyval.dialog_control)->help = (yyvsp[-1].il);
	    (yyval.dialog_control)->data = (yyvsp[0].rcdata_item);
	  }
#line 2694 "rcparse.c"
    break;

  case 74: /* $@10: %empty  */
#line 657 "rcparse.y"
            {
	      default_style = SS_CENTER | WS_GROUP;
	      base_style = SS_CENTER;
	      class.named = 0;
	      class.u.id = CTL_STATIC;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2706 "rcparse.c"
    break;

  case 75: /* control: CTEXT optresidc $@10 control_params  */
#line 665 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2714 "rcparse.c"
    break;

  case 76: /* $@11: %empty  */
#line 669 "rcparse.y"
            {
	      default_style = BS_DEFPUSHBUTTON | WS_TABSTOP;
	      base_style = BS_DEFPUSHBUTTON | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2726 "rcparse.c"
    break;

  case 77: /* control: DEFPUSHBUTTON optresidc $@11 control_params  */
#line 677 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2734 "rcparse.c"
    break;

  case 78: /* $@12: %empty  */
#line 681 "rcparse.y"
            {
	      default_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      base_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_EDIT;
	      res_text_field = res_null_text;	
	    }
#line 2746 "rcparse.c"
    break;

  case 79: /* control: EDITTEXT $@12 control_params  */
#line 689 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2754 "rcparse.c"
    break;

  case 80: /* $@13: %empty  */
#line 693 "rcparse.y"
            {
	      default_style = BS_GROUPBOX;
	      base_style = BS_GROUPBOX;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2766 "rcparse.c"
    break;

  case 81: /* control: GROUPBOX optresidc $@13 control_params  */
#line 701 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2774 "rcparse.c"
    break;

  case 82: /* $@14: %empty  */
#line 705 "rcparse.y"
            {
	      default_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      base_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_EDIT;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2786 "rcparse.c"
    break;

  case 83: /* control: HEDIT optresidc $@14 control_params  */
#line 713 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("IEDIT requires DIALOGEX"));
	    res_string_to_id (&(yyval.dialog_control)->class, "HEDIT");
	  }
#line 2797 "rcparse.c"
    break;

  case 84: /* control: ICON resref numexpr cnumexpr cnumexpr opt_control_data  */
#line 720 "rcparse.y"
          {
	    (yyval.dialog_control) = define_icon_control ((yyvsp[-4].id), (yyvsp[-3].il), (yyvsp[-2].il), (yyvsp[-1].il), 0, 0, 0, (yyvsp[0].rcdata_item),
				      dialog.ex);
          }
#line 2806 "rcparse.c"
    break;

  case 85: /* control: ICON resref numexpr cnumexpr cnumexpr cnumexpr cnumexpr opt_control_data  */
#line 726 "rcparse.y"
          {
	    (yyval.dialog_control) = define_icon_control ((yyvsp[-6].id), (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), 0, 0, 0, (yyvsp[0].rcdata_item),
				      dialog.ex);
          }
#line 2815 "rcparse.c"
    break;

  case 86: /* control: ICON resref numexpr cnumexpr cnumexpr cnumexpr cnumexpr icon_styleexpr optcnumexpr opt_control_data  */
#line 732 "rcparse.y"
          {
	    (yyval.dialog_control) = define_icon_control ((yyvsp[-8].id), (yyvsp[-7].il), (yyvsp[-6].il), (yyvsp[-5].il), style, (yyvsp[-1].il), 0, (yyvsp[0].rcdata_item),
				      dialog.ex);
          }
#line 2824 "rcparse.c"
    break;

  case 87: /* control: ICON resref numexpr cnumexpr cnumexpr cnumexpr cnumexpr icon_styleexpr cnumexpr cnumexpr opt_control_data  */
#line 738 "rcparse.y"
          {
	    (yyval.dialog_control) = define_icon_control ((yyvsp[-9].id), (yyvsp[-8].il), (yyvsp[-7].il), (yyvsp[-6].il), style, (yyvsp[-2].il), (yyvsp[-1].il), (yyvsp[0].rcdata_item),
				      dialog.ex);
          }
#line 2833 "rcparse.c"
    break;

  case 88: /* $@15: %empty  */
#line 743 "rcparse.y"
            {
	      default_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      base_style = ES_LEFT | WS_BORDER | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_EDIT;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2845 "rcparse.c"
    break;

  case 89: /* control: IEDIT optresidc $@15 control_params  */
#line 751 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	    if (dialog.ex == NULL)
	      rcparse_warning (_("IEDIT requires DIALOGEX"));
	    res_string_to_id (&(yyval.dialog_control)->class, "IEDIT");
	  }
#line 2856 "rcparse.c"
    break;

  case 90: /* $@16: %empty  */
#line 758 "rcparse.y"
            {
	      default_style = LBS_NOTIFY | WS_BORDER;
	      base_style = LBS_NOTIFY | WS_BORDER;
	      class.named = 0;
	      class.u.id = CTL_LISTBOX;
	      res_text_field = res_null_text;	
	    }
#line 2868 "rcparse.c"
    break;

  case 91: /* control: LISTBOX $@16 control_params  */
#line 766 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2876 "rcparse.c"
    break;

  case 92: /* $@17: %empty  */
#line 770 "rcparse.y"
            {
	      default_style = SS_LEFT | WS_GROUP;
	      base_style = SS_LEFT;
	      class.named = 0;
	      class.u.id = CTL_STATIC;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2888 "rcparse.c"
    break;

  case 93: /* control: LTEXT optresidc $@17 control_params  */
#line 778 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2896 "rcparse.c"
    break;

  case 94: /* $@18: %empty  */
#line 782 "rcparse.y"
            {
	      default_style = BS_PUSHBOX | WS_TABSTOP;
	      base_style = BS_PUSHBOX;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	    }
#line 2907 "rcparse.c"
    break;

  case 95: /* control: PUSHBOX optresidc $@18 control_params  */
#line 789 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2915 "rcparse.c"
    break;

  case 96: /* $@19: %empty  */
#line 793 "rcparse.y"
            {
	      default_style = BS_PUSHBUTTON | WS_TABSTOP;
	      base_style = BS_PUSHBUTTON | WS_TABSTOP;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2927 "rcparse.c"
    break;

  case 97: /* control: PUSHBUTTON optresidc $@19 control_params  */
#line 801 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2935 "rcparse.c"
    break;

  case 98: /* $@20: %empty  */
#line 805 "rcparse.y"
            {
	      default_style = BS_RADIOBUTTON | WS_TABSTOP;
	      base_style = BS_RADIOBUTTON;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2947 "rcparse.c"
    break;

  case 99: /* control: RADIOBUTTON optresidc $@20 control_params  */
#line 813 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2955 "rcparse.c"
    break;

  case 100: /* $@21: %empty  */
#line 817 "rcparse.y"
            {
	      default_style = SS_RIGHT | WS_GROUP;
	      base_style = SS_RIGHT;
	      class.named = 0;
	      class.u.id = CTL_STATIC;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 2967 "rcparse.c"
    break;

  case 101: /* control: RTEXT optresidc $@21 control_params  */
#line 825 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2975 "rcparse.c"
    break;

  case 102: /* $@22: %empty  */
#line 829 "rcparse.y"
            {
	      default_style = SBS_HORZ;
	      base_style = 0;
	      class.named = 0;
	      class.u.id = CTL_SCROLLBAR;
	      res_text_field = res_null_text;	
	    }
#line 2987 "rcparse.c"
    break;

  case 103: /* control: SCROLLBAR $@22 control_params  */
#line 837 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 2995 "rcparse.c"
    break;

  case 104: /* $@23: %empty  */
#line 841 "rcparse.y"
            {
	      default_style = BS_3STATE | WS_TABSTOP;
	      base_style = BS_3STATE;
	      class.named = 0;
	      class.u.id = CTL_BUTTON;
	      res_text_field = (yyvsp[0].id);	
	    }
#line 3007 "rcparse.c"
    break;

  case 105: /* control: STATE3 optresidc $@23 control_params  */
#line 849 "rcparse.y"
          {
	    (yyval.dialog_control) = (yyvsp[0].dialog_control);
	  }
#line 3015 "rcparse.c"
    break;

  case 106: /* $@24: %empty  */
#line 854 "rcparse.y"
            { style = WS_CHILD | WS_VISIBLE; }
#line 3021 "rcparse.c"
    break;

  case 107: /* control: USERBUTTON resref numexpr ',' numexpr ',' numexpr ',' numexpr ',' numexpr ',' $@24 styleexpr optcnumexpr  */
#line 856 "rcparse.y"
          {
	    rc_res_id cid;
	    cid.named = 0;
	    cid.u.id = CTL_BUTTON;
	    (yyval.dialog_control) = define_control ((yyvsp[-13].id), (yyvsp[-12].il), (yyvsp[-10].il), (yyvsp[-8].il), (yyvsp[-6].il), (yyvsp[-4].il), cid,
				 style, (yyvsp[0].il));
	  }
#line 3033 "rcparse.c"
    break;

  case 108: /* control_params: numexpr cnumexpr cnumexpr cnumexpr cnumexpr opt_control_data  */
#line 874 "rcparse.y"
          {
	    (yyval.dialog_control) = define_control (res_text_field, (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), (yyvsp[-2].il), (yyvsp[-1].il), class,
				 default_style | WS_CHILD | WS_VISIBLE, 0);
	    if ((yyvsp[0].rcdata_item) != NULL)
	      {
		if (dialog.ex == NULL)
		  rcparse_warning (_("control data requires DIALOGEX"));
		(yyval.dialog_control)->data = (yyvsp[0].rcdata_item);
	      }
	  }
#line 3048 "rcparse.c"
    break;

  case 109: /* control_params: numexpr cnumexpr cnumexpr cnumexpr cnumexpr control_params_styleexpr optcnumexpr opt_control_data  */
#line 886 "rcparse.y"
          {
	    (yyval.dialog_control) = define_control (res_text_field, (yyvsp[-7].il), (yyvsp[-6].il), (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), class, style, (yyvsp[-1].il));
	    if ((yyvsp[0].rcdata_item) != NULL)
	      {
		if (dialog.ex == NULL)
		  rcparse_warning (_("control data requires DIALOGEX"));
		(yyval.dialog_control)->data = (yyvsp[0].rcdata_item);
	      }
	  }
#line 3062 "rcparse.c"
    break;

  case 110: /* control_params: numexpr cnumexpr cnumexpr cnumexpr cnumexpr control_params_styleexpr cnumexpr cnumexpr opt_control_data  */
#line 897 "rcparse.y"
          {
	    (yyval.dialog_control) = define_control (res_text_field, (yyvsp[-8].il), (yyvsp[-7].il), (yyvsp[-6].il), (yyvsp[-5].il), (yyvsp[-4].il), class, style, (yyvsp[-2].il));
	    if (dialog.ex == NULL)
	      rcparse_warning (_("help ID requires DIALOGEX"));
	    (yyval.dialog_control)->help = (yyvsp[-1].il);
	    (yyval.dialog_control)->data = (yyvsp[0].rcdata_item);
	  }
#line 3074 "rcparse.c"
    break;

  case 111: /* cresid: ',' resid  */
#line 908 "rcparse.y"
          {
	    if ((yyvsp[0].id).named)
	      res_unistring_to_id (&(yyval.id), (yyvsp[0].id).u.n.name);
	    else
	      (yyval.id)=(yyvsp[0].id);
	  }
#line 3085 "rcparse.c"
    break;

  case 112: /* optresidc: %empty  */
#line 918 "rcparse.y"
          {
	    res_string_to_id (&(yyval.id), "");
	  }
#line 3093 "rcparse.c"
    break;

  case 113: /* optresidc: resid ','  */
#line 921 "rcparse.y"
                    { (yyval.id)=(yyvsp[-1].id); }
#line 3099 "rcparse.c"
    break;

  case 114: /* resid: posnumexpr  */
#line 926 "rcparse.y"
          {
	    (yyval.id).named = 0;
	    (yyval.id).u.id = (yyvsp[0].il);
	  }
#line 3108 "rcparse.c"
    break;

  case 115: /* resid: res_unicode_string_concat  */
#line 931 "rcparse.y"
          {
	    (yyval.id).named = 1;
	    (yyval.id).u.n.name = (yyvsp[0].uni);
	    (yyval.id).u.n.length = unichar_len ((yyvsp[0].uni));
	  }
#line 3118 "rcparse.c"
    break;

  case 116: /* opt_control_data: %empty  */
#line 940 "rcparse.y"
          {
	    (yyval.rcdata_item) = NULL;
	  }
#line 3126 "rcparse.c"
    break;

  case 117: /* opt_control_data: BEG optrcdata_data END  */
#line 944 "rcparse.y"
          {
	    (yyval.rcdata_item) = (yyvsp[-1].rcdata).first;
	  }
#line 3134 "rcparse.c"
    break;

  case 118: /* $@25: %empty  */
#line 953 "rcparse.y"
          { style = WS_CHILD | WS_VISIBLE; }
#line 3140 "rcparse.c"
    break;

  case 120: /* $@26: %empty  */
#line 959 "rcparse.y"
          { style = SS_ICON | WS_CHILD | WS_VISIBLE; }
#line 3146 "rcparse.c"
    break;

  case 122: /* $@27: %empty  */
#line 965 "rcparse.y"
          { style = base_style | WS_CHILD | WS_VISIBLE; }
#line 3152 "rcparse.c"
    break;

  case 124: /* font: id FONT memflags_move_discard file_name  */
#line 973 "rcparse.y"
          {
	    define_font ((yyvsp[-3].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3163 "rcparse.c"
    break;

  case 125: /* icon: id ICON memflags_move_discard file_name  */
#line 985 "rcparse.y"
          {
	    define_icon ((yyvsp[-3].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3174 "rcparse.c"
    break;

  case 126: /* language: LANGUAGE numexpr cnumexpr  */
#line 998 "rcparse.y"
          {
	    language = (yyvsp[-1].il) | ((yyvsp[0].il) << SUBLANG_SHIFT);
	  }
#line 3182 "rcparse.c"
    break;

  case 127: /* menu: id MENU suboptions BEG menuitems END  */
#line 1007 "rcparse.y"
          {
	    define_menu ((yyvsp[-5].id), &(yyvsp[-3].res_info), (yyvsp[-1].menuitem));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3193 "rcparse.c"
    break;

  case 128: /* menuitems: %empty  */
#line 1017 "rcparse.y"
          {
	    (yyval.menuitem) = NULL;
	  }
#line 3201 "rcparse.c"
    break;

  case 129: /* menuitems: menuitems menuitem  */
#line 1021 "rcparse.y"
          {
	    if ((yyvsp[-1].menuitem) == NULL)
	      (yyval.menuitem) = (yyvsp[0].menuitem);
	    else
	      {
		rc_menuitem **pp;

		for (pp = &(yyvsp[-1].menuitem)->next; *pp != NULL; pp = &(*pp)->next)
		  ;
		*pp = (yyvsp[0].menuitem);
		(yyval.menuitem) = (yyvsp[-1].menuitem);
	      }
	  }
#line 3219 "rcparse.c"
    break;

  case 130: /* menuitem: MENUITEM res_unicode_string_concat cnumexpr menuitem_flags  */
#line 1038 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-2].uni), (yyvsp[-1].il), (yyvsp[0].is), 0, 0, NULL);
	  }
#line 3227 "rcparse.c"
    break;

  case 131: /* menuitem: MENUITEM SEPARATOR  */
#line 1042 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem (NULL, 0, 0, 0, 0, NULL);
	  }
#line 3235 "rcparse.c"
    break;

  case 132: /* menuitem: POPUP res_unicode_string_concat menuitem_flags BEG menuitems END  */
#line 1046 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-4].uni), 0, (yyvsp[-3].is), 0, 0, (yyvsp[-1].menuitem));
	  }
#line 3243 "rcparse.c"
    break;

  case 133: /* menuitem_flags: %empty  */
#line 1053 "rcparse.y"
          {
	    (yyval.is) = 0;
	  }
#line 3251 "rcparse.c"
    break;

  case 134: /* menuitem_flags: menuitem_flags ',' menuitem_flag  */
#line 1057 "rcparse.y"
          {
	    (yyval.is) = (yyvsp[-2].is) | (yyvsp[0].is);
	  }
#line 3259 "rcparse.c"
    break;

  case 135: /* menuitem_flags: menuitem_flags menuitem_flag  */
#line 1061 "rcparse.y"
          {
	    (yyval.is) = (yyvsp[-1].is) | (yyvsp[0].is);
	  }
#line 3267 "rcparse.c"
    break;

  case 136: /* menuitem_flag: CHECKED  */
#line 1068 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_CHECKED;
	  }
#line 3275 "rcparse.c"
    break;

  case 137: /* menuitem_flag: GRAYED  */
#line 1072 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_GRAYED;
	  }
#line 3283 "rcparse.c"
    break;

  case 138: /* menuitem_flag: HELP  */
#line 1076 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_HELP;
	  }
#line 3291 "rcparse.c"
    break;

  case 139: /* menuitem_flag: INACTIVE  */
#line 1080 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_INACTIVE;
	  }
#line 3299 "rcparse.c"
    break;

  case 140: /* menuitem_flag: MENUBARBREAK  */
#line 1084 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_MENUBARBREAK;
	  }
#line 3307 "rcparse.c"
    break;

  case 141: /* menuitem_flag: MENUBREAK  */
#line 1088 "rcparse.y"
          {
	    (yyval.is) = MENUITEM_MENUBREAK;
	  }
#line 3315 "rcparse.c"
    break;

  case 142: /* menuex: id MENUEX suboptions BEG menuexitems END  */
#line 1097 "rcparse.y"
          {
	    define_menu ((yyvsp[-5].id), &(yyvsp[-3].res_info), (yyvsp[-1].menuitem));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3326 "rcparse.c"
    break;

  case 143: /* menuexitems: %empty  */
#line 1107 "rcparse.y"
          {
	    (yyval.menuitem) = NULL;
	  }
#line 3334 "rcparse.c"
    break;

  case 144: /* menuexitems: menuexitems menuexitem  */
#line 1111 "rcparse.y"
          {
	    if ((yyvsp[-1].menuitem) == NULL)
	      (yyval.menuitem) = (yyvsp[0].menuitem);
	    else
	      {
		rc_menuitem **pp;

		for (pp = &(yyvsp[-1].menuitem)->next; *pp != NULL; pp = &(*pp)->next)
		  ;
		*pp = (yyvsp[0].menuitem);
		(yyval.menuitem) = (yyvsp[-1].menuitem);
	      }
	  }
#line 3352 "rcparse.c"
    break;

  case 145: /* menuexitem: MENUITEM res_unicode_string_concat  */
#line 1128 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[0].uni), 0, 0, 0, 0, NULL);
	  }
#line 3360 "rcparse.c"
    break;

  case 146: /* menuexitem: MENUITEM res_unicode_string_concat cnumexpr  */
#line 1132 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-1].uni), (yyvsp[0].il), 0, 0, 0, NULL);
	  }
#line 3368 "rcparse.c"
    break;

  case 147: /* menuexitem: MENUITEM res_unicode_string_concat cnumexpr cnumexpr optcnumexpr  */
#line 1136 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-3].uni), (yyvsp[-2].il), (yyvsp[-1].il), (yyvsp[0].il), 0, NULL);
	  }
#line 3376 "rcparse.c"
    break;

  case 148: /* menuexitem: MENUITEM SEPARATOR  */
#line 1140 "rcparse.y"
          {
 	    (yyval.menuitem) = define_menuitem (NULL, 0, 0, 0, 0, NULL);
 	  }
#line 3384 "rcparse.c"
    break;

  case 149: /* menuexitem: POPUP res_unicode_string_concat BEG menuexitems END  */
#line 1144 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-3].uni), 0, 0, 0, 0, (yyvsp[-1].menuitem));
	  }
#line 3392 "rcparse.c"
    break;

  case 150: /* menuexitem: POPUP res_unicode_string_concat cnumexpr BEG menuexitems END  */
#line 1148 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-4].uni), (yyvsp[-3].il), 0, 0, 0, (yyvsp[-1].menuitem));
	  }
#line 3400 "rcparse.c"
    break;

  case 151: /* menuexitem: POPUP res_unicode_string_concat cnumexpr cnumexpr BEG menuexitems END  */
#line 1152 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-5].uni), (yyvsp[-4].il), (yyvsp[-3].il), 0, 0, (yyvsp[-1].menuitem));
	  }
#line 3408 "rcparse.c"
    break;

  case 152: /* menuexitem: POPUP res_unicode_string_concat cnumexpr cnumexpr cnumexpr optcnumexpr BEG menuexitems END  */
#line 1157 "rcparse.y"
          {
	    (yyval.menuitem) = define_menuitem ((yyvsp[-7].uni), (yyvsp[-6].il), (yyvsp[-5].il), (yyvsp[-4].il), (yyvsp[-3].il), (yyvsp[-1].menuitem));
	  }
#line 3416 "rcparse.c"
    break;

  case 153: /* messagetable: id MESSAGETABLE memflags_move file_name  */
#line 1166 "rcparse.y"
          {
	    define_messagetable ((yyvsp[-3].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3427 "rcparse.c"
    break;

  case 154: /* $@28: %empty  */
#line 1178 "rcparse.y"
          {
	    rcparse_rcdata ();
	  }
#line 3435 "rcparse.c"
    break;

  case 155: /* optrcdata_data: $@28 optrcdata_data_int  */
#line 1182 "rcparse.y"
          {
	    rcparse_normal ();
	    (yyval.rcdata) = (yyvsp[0].rcdata);
	  }
#line 3444 "rcparse.c"
    break;

  case 156: /* optrcdata_data_int: %empty  */
#line 1190 "rcparse.y"
          {
	    (yyval.rcdata).first = NULL;
	    (yyval.rcdata).last = NULL;
	  }
#line 3453 "rcparse.c"
    break;

  case 157: /* optrcdata_data_int: rcdata_data  */
#line 1195 "rcparse.y"
          {
	    (yyval.rcdata) = (yyvsp[0].rcdata);
	  }
#line 3461 "rcparse.c"
    break;

  case 158: /* rcdata_data: sizedstring  */
#line 1202 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_string ((yyvsp[0].ss).s, (yyvsp[0].ss).length);
	    (yyval.rcdata).first = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3473 "rcparse.c"
    break;

  case 159: /* rcdata_data: sizedunistring  */
#line 1210 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_unistring ((yyvsp[0].suni).s, (yyvsp[0].suni).length);
	    (yyval.rcdata).first = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3485 "rcparse.c"
    break;

  case 160: /* rcdata_data: sizednumexpr  */
#line 1218 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_number ((yyvsp[0].i).val, (yyvsp[0].i).dword);
	    (yyval.rcdata).first = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3497 "rcparse.c"
    break;

  case 161: /* rcdata_data: rcdata_data ',' sizedstring  */
#line 1226 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_string ((yyvsp[0].ss).s, (yyvsp[0].ss).length);
	    (yyval.rcdata).first = (yyvsp[-2].rcdata).first;
	    (yyvsp[-2].rcdata).last->next = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3510 "rcparse.c"
    break;

  case 162: /* rcdata_data: rcdata_data ',' sizedunistring  */
#line 1235 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_unistring ((yyvsp[0].suni).s, (yyvsp[0].suni).length);
	    (yyval.rcdata).first = (yyvsp[-2].rcdata).first;
	    (yyvsp[-2].rcdata).last->next = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3523 "rcparse.c"
    break;

  case 163: /* rcdata_data: rcdata_data ',' sizednumexpr  */
#line 1244 "rcparse.y"
          {
	    rc_rcdata_item *ri;

	    ri = define_rcdata_number ((yyvsp[0].i).val, (yyvsp[0].i).dword);
	    (yyval.rcdata).first = (yyvsp[-2].rcdata).first;
	    (yyvsp[-2].rcdata).last->next = ri;
	    (yyval.rcdata).last = ri;
	  }
#line 3536 "rcparse.c"
    break;

  case 164: /* rcdata_data: rcdata_data ','  */
#line 1253 "rcparse.y"
          {
	    (yyval.rcdata)=(yyvsp[-1].rcdata);
	  }
#line 3544 "rcparse.c"
    break;

  case 165: /* $@29: %empty  */
#line 1262 "rcparse.y"
            { sub_res_info = (yyvsp[-1].res_info); rcparse_rcdata (); }
#line 3550 "rcparse.c"
    break;

  case 166: /* stringtable: STRINGTABLE suboptions BEG $@29 string_data END  */
#line 1263 "rcparse.y"
                            { rcparse_normal (); }
#line 3556 "rcparse.c"
    break;

  case 168: /* string_data: string_data numexpr res_unicode_sizedstring_concat  */
#line 1269 "rcparse.y"
          {
	    define_stringtable (&sub_res_info, (yyvsp[-1].il), (yyvsp[0].suni).s, (yyvsp[0].suni).length);
	    rcparse_discard_strings ();
	  }
#line 3565 "rcparse.c"
    break;

  case 169: /* string_data: string_data numexpr ',' res_unicode_sizedstring_concat  */
#line 1274 "rcparse.y"
          {
	    define_stringtable (&sub_res_info, (yyvsp[-2].il), (yyvsp[0].suni).s, (yyvsp[0].suni).length);
	    rcparse_discard_strings ();
	  }
#line 3574 "rcparse.c"
    break;

  case 170: /* string_data: string_data error  */
#line 1279 "rcparse.y"
          {
	    rcparse_warning (_("invalid stringtable resource."));
	    abort ();
	  }
#line 3583 "rcparse.c"
    break;

  case 171: /* rcdata_id: id  */
#line 1287 "rcparse.y"
          {
	    (yyval.id)=(yyvsp[0].id);
	  }
#line 3591 "rcparse.c"
    break;

  case 172: /* rcdata_id: HTML  */
#line 1291 "rcparse.y"
        {
	  (yyval.id).named = 0;
	  (yyval.id).u.id = 23;
	}
#line 3600 "rcparse.c"
    break;

  case 173: /* rcdata_id: RCDATA  */
#line 1296 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_RCDATA;
        }
#line 3609 "rcparse.c"
    break;

  case 174: /* rcdata_id: MANIFEST  */
#line 1301 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_MANIFEST;
        }
#line 3618 "rcparse.c"
    break;

  case 175: /* rcdata_id: PLUGPLAY  */
#line 1306 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_PLUGPLAY;
        }
#line 3627 "rcparse.c"
    break;

  case 176: /* rcdata_id: VXD  */
#line 1311 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_VXD;
        }
#line 3636 "rcparse.c"
    break;

  case 177: /* rcdata_id: DLGINCLUDE  */
#line 1316 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_DLGINCLUDE;
        }
#line 3645 "rcparse.c"
    break;

  case 178: /* rcdata_id: DLGINIT  */
#line 1321 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_DLGINIT;
        }
#line 3654 "rcparse.c"
    break;

  case 179: /* rcdata_id: ANICURSOR  */
#line 1326 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_ANICURSOR;
        }
#line 3663 "rcparse.c"
    break;

  case 180: /* rcdata_id: ANIICON  */
#line 1331 "rcparse.y"
        {
          (yyval.id).named = 0;
          (yyval.id).u.id = RT_ANIICON;
        }
#line 3672 "rcparse.c"
    break;

  case 181: /* user: id rcdata_id suboptions BEG optrcdata_data END  */
#line 1342 "rcparse.y"
          {
	    define_user_data ((yyvsp[-5].id), (yyvsp[-4].id), &(yyvsp[-3].res_info), (yyvsp[-1].rcdata).first);
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3683 "rcparse.c"
    break;

  case 182: /* user: id rcdata_id suboptions file_name  */
#line 1349 "rcparse.y"
          {
	    define_user_file ((yyvsp[-3].id), (yyvsp[-2].id), &(yyvsp[-1].res_info), (yyvsp[0].s));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3694 "rcparse.c"
    break;

  case 183: /* toolbar: id TOOLBAR suboptions numexpr cnumexpr BEG toolbar_data END  */
#line 1359 "rcparse.y"
        {
	  define_toolbar ((yyvsp[-7].id), &(yyvsp[-5].res_info), (yyvsp[-4].il), (yyvsp[-3].il), (yyvsp[-1].toobar_item));
	}
#line 3702 "rcparse.c"
    break;

  case 184: /* toolbar_data: %empty  */
#line 1364 "rcparse.y"
                          { (yyval.toobar_item)= NULL; }
#line 3708 "rcparse.c"
    break;

  case 185: /* toolbar_data: toolbar_data BUTTON id  */
#line 1366 "rcparse.y"
        {
	  rc_toolbar_item *c,*n;
	  c = (yyvsp[-2].toobar_item);
	  n= (rc_toolbar_item *)
	      res_alloc (sizeof (rc_toolbar_item));
	  if (c != NULL)
	    while (c->next != NULL)
	      c = c->next;
	  n->prev = c;
	  n->next = NULL;
	  if (c != NULL)
	    c->next = n;
	  n->id = (yyvsp[0].id);
	  if ((yyvsp[-2].toobar_item) == NULL)
	    (yyval.toobar_item) = n;
	  else
	    (yyval.toobar_item) = (yyvsp[-2].toobar_item);
	}
#line 3731 "rcparse.c"
    break;

  case 186: /* toolbar_data: toolbar_data SEPARATOR  */
#line 1385 "rcparse.y"
        {
	  rc_toolbar_item *c,*n;
	  c = (yyvsp[-1].toobar_item);
	  n= (rc_toolbar_item *)
	      res_alloc (sizeof (rc_toolbar_item));
	  if (c != NULL)
	    while (c->next != NULL)
	      c = c->next;
	  n->prev = c;
	  n->next = NULL;
	  if (c != NULL)
	    c->next = n;
	  n->id.named = 0;
	  n->id.u.id = 0;
	  if ((yyvsp[-1].toobar_item) == NULL)
	    (yyval.toobar_item) = n;
	  else
	    (yyval.toobar_item) = (yyvsp[-1].toobar_item);
	}
#line 3755 "rcparse.c"
    break;

  case 187: /* versioninfo: id VERSIONINFO fixedverinfo BEG verblocks END  */
#line 1410 "rcparse.y"
          {
	    define_versioninfo ((yyvsp[-5].id), language, (yyvsp[-3].fixver), (yyvsp[-1].verinfo));
	    if (yychar != YYEMPTY)
	      YYERROR;
	    rcparse_discard_strings ();
	  }
#line 3766 "rcparse.c"
    break;

  case 188: /* fixedverinfo: %empty  */
#line 1420 "rcparse.y"
          {
	    (yyval.fixver) = ((rc_fixed_versioninfo *)
		  res_alloc (sizeof (rc_fixed_versioninfo)));
	    memset ((yyval.fixver), 0, sizeof (rc_fixed_versioninfo));
	  }
#line 3776 "rcparse.c"
    break;

  case 189: /* fixedverinfo: fixedverinfo FILEVERSION numexpr optcnumexpr optcnumexpr optcnumexpr  */
#line 1427 "rcparse.y"
          {
	    (yyvsp[-5].fixver)->file_version_ms = ((yyvsp[-3].il) << 16) | ((yyvsp[-2].il) & 0xffff);
	    (yyvsp[-5].fixver)->file_version_ls = ((yyvsp[-1].il) << 16) | ((yyvsp[0].il) & 0xffff);
	    (yyval.fixver) = (yyvsp[-5].fixver);
	  }
#line 3786 "rcparse.c"
    break;

  case 190: /* fixedverinfo: fixedverinfo PRODUCTVERSION numexpr optcnumexpr optcnumexpr optcnumexpr  */
#line 1434 "rcparse.y"
          {
	    (yyvsp[-5].fixver)->product_version_ms = ((yyvsp[-3].il) << 16) | ((yyvsp[-2].il) & 0xffff);
	    (yyvsp[-5].fixver)->product_version_ls = ((yyvsp[-1].il) << 16) | ((yyvsp[0].il) & 0xffff);
	    (yyval.fixver) = (yyvsp[-5].fixver);
	  }
#line 3796 "rcparse.c"
    break;

  case 191: /* fixedverinfo: fixedverinfo FILEFLAGSMASK numexpr  */
#line 1440 "rcparse.y"
          {
	    (yyvsp[-2].fixver)->file_flags_mask = (yyvsp[0].il);
	    (yyval.fixver) = (yyvsp[-2].fixver);
	  }
#line 3805 "rcparse.c"
    break;

  case 192: /* fixedverinfo: fixedverinfo FILEFLAGS numexpr  */
#line 1445 "rcparse.y"
          {
	    (yyvsp[-2].fixver)->file_flags = (yyvsp[0].il);
	    (yyval.fixver) = (yyvsp[-2].fixver);
	  }
#line 3814 "rcparse.c"
    break;

  case 193: /* fixedverinfo: fixedverinfo FILEOS numexpr  */
#line 1450 "rcparse.y"
          {
	    (yyvsp[-2].fixver)->file_os = (yyvsp[0].il);
	    (yyval.fixver) = (yyvsp[-2].fixver);
	  }
#line 3823 "rcparse.c"
    break;

  case 194: /* fixedverinfo: fixedverinfo FILETYPE numexpr  */
#line 1455 "rcparse.y"
          {
	    (yyvsp[-2].fixver)->file_type = (yyvsp[0].il);
	    (yyval.fixver) = (yyvsp[-2].fixver);
	  }
#line 3832 "rcparse.c"
    break;

  case 195: /* fixedverinfo: fixedverinfo FILESUBTYPE numexpr  */
#line 1460 "rcparse.y"
          {
	    (yyvsp[-2].fixver)->file_subtype = (yyvsp[0].il);
	    (yyval.fixver) = (yyvsp[-2].fixver);
	  }
#line 3841 "rcparse.c"
    break;

  case 196: /* verblocks: %empty  */
#line 1474 "rcparse.y"
          {
	    (yyval.verinfo) = NULL;
	  }
#line 3849 "rcparse.c"
    break;

  case 197: /* verblocks: verblocks BLOCKSTRINGFILEINFO BEG verstringtables END  */
#line 1478 "rcparse.y"
          {
	    (yyval.verinfo) = append_ver_stringfileinfo ((yyvsp[-4].verinfo), (yyvsp[-1].verstringtable));
	  }
#line 3857 "rcparse.c"
    break;

  case 198: /* verblocks: verblocks BLOCKVARFILEINFO BEG VALUE res_unicode_string_concat vertrans END  */
#line 1482 "rcparse.y"
          {
	    (yyval.verinfo) = append_ver_varfileinfo ((yyvsp[-6].verinfo), (yyvsp[-2].uni), (yyvsp[-1].vervar));
	  }
#line 3865 "rcparse.c"
    break;

  case 199: /* verstringtables: %empty  */
#line 1489 "rcparse.y"
          {
	    (yyval.verstringtable) = NULL;
	  }
#line 3873 "rcparse.c"
    break;

  case 200: /* verstringtables: verstringtables BLOCK BEG vervals END  */
#line 1493 "rcparse.y"
          {
	    (yyval.verstringtable) = append_ver_stringtable ((yyvsp[-4].verstringtable), (yyvsp[-3].s), (yyvsp[-1].verstring));
	  }
#line 3881 "rcparse.c"
    break;

  case 201: /* vervals: %empty  */
#line 1500 "rcparse.y"
          {
	    (yyval.verstring) = NULL;
	  }
#line 3889 "rcparse.c"
    break;

  case 202: /* vervals: vervals VALUE res_unicode_string_concat ',' res_unicode_string_concat  */
#line 1504 "rcparse.y"
          {
	    (yyval.verstring) = append_verval ((yyvsp[-4].verstring), (yyvsp[-2].uni), (yyvsp[0].uni));
	  }
#line 3897 "rcparse.c"
    break;

  case 203: /* vertrans: %empty  */
#line 1511 "rcparse.y"
          {
	    (yyval.vervar) = NULL;
	  }
#line 3905 "rcparse.c"
    break;

  case 204: /* vertrans: vertrans cnumexpr cnumexpr  */
#line 1515 "rcparse.y"
          {
	    (yyval.vervar) = append_vertrans ((yyvsp[-2].vervar), (yyvsp[-1].il), (yyvsp[0].il));
	  }
#line 3913 "rcparse.c"
    break;

  case 205: /* id: posnumexpr  */
#line 1524 "rcparse.y"
          {
	    (yyval.id).named = 0;
	    (yyval.id).u.id = (yyvsp[0].il);
	  }
#line 3922 "rcparse.c"
    break;

  case 206: /* id: resname  */
#line 1529 "rcparse.y"
          {
	    res_unistring_to_id (&(yyval.id), (yyvsp[0].uni));
	  }
#line 3930 "rcparse.c"
    break;

  case 207: /* resname: res_unicode_string  */
#line 1538 "rcparse.y"
          {
	    (yyval.uni) = (yyvsp[0].uni);
	  }
#line 3938 "rcparse.c"
    break;

  case 208: /* resname: STRING  */
#line 1542 "rcparse.y"
          {
	    unichar *h = NULL;
	    unicode_from_ascii ((rc_uint_type *) NULL, &h, (yyvsp[0].s));
	    (yyval.uni) = h;
	  }
#line 3948 "rcparse.c"
    break;

  case 209: /* resref: posnumexpr ','  */
#line 1552 "rcparse.y"
          {
	    (yyval.id).named = 0;
	    (yyval.id).u.id = (yyvsp[-1].il);
	  }
#line 3957 "rcparse.c"
    break;

  case 210: /* resref: resname  */
#line 1557 "rcparse.y"
          {
	    res_unistring_to_id (&(yyval.id), (yyvsp[0].uni));
	  }
#line 3965 "rcparse.c"
    break;

  case 211: /* resref: resname ','  */
#line 1561 "rcparse.y"
          {
	    res_unistring_to_id (&(yyval.id), (yyvsp[-1].uni));
	  }
#line 3973 "rcparse.c"
    break;

  case 212: /* suboptions: %empty  */
#line 1571 "rcparse.y"
          {
	    memset (&(yyval.res_info), 0, sizeof (rc_res_res_info));
	    (yyval.res_info).language = language;
	    /* FIXME: Is this the right default?  */
	    (yyval.res_info).memflags = MEMFLAG_MOVEABLE | MEMFLAG_PURE | MEMFLAG_DISCARDABLE;
	  }
#line 3984 "rcparse.c"
    break;

  case 213: /* suboptions: suboptions memflag  */
#line 1578 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-1].res_info);
	    (yyval.res_info).memflags |= (yyvsp[0].memflags).on;
	    (yyval.res_info).memflags &=~ (yyvsp[0].memflags).off;
	  }
#line 3994 "rcparse.c"
    break;

  case 214: /* suboptions: suboptions CHARACTERISTICS numexpr  */
#line 1584 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-2].res_info);
	    (yyval.res_info).characteristics = (yyvsp[0].il);
	  }
#line 4003 "rcparse.c"
    break;

  case 215: /* suboptions: suboptions LANGUAGE numexpr cnumexpr  */
#line 1589 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-3].res_info);
	    (yyval.res_info).language = (yyvsp[-1].il) | ((yyvsp[0].il) << SUBLANG_SHIFT);
	  }
#line 4012 "rcparse.c"
    break;

  case 216: /* suboptions: suboptions VERSIONK numexpr  */
#line 1594 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-2].res_info);
	    (yyval.res_info).version = (yyvsp[0].il);
	  }
#line 4021 "rcparse.c"
    break;

  case 217: /* memflags_move_discard: %empty  */
#line 1604 "rcparse.y"
          {
	    memset (&(yyval.res_info), 0, sizeof (rc_res_res_info));
	    (yyval.res_info).language = language;
	    (yyval.res_info).memflags = MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE;
	  }
#line 4031 "rcparse.c"
    break;

  case 218: /* memflags_move_discard: memflags_move_discard memflag  */
#line 1610 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-1].res_info);
	    (yyval.res_info).memflags |= (yyvsp[0].memflags).on;
	    (yyval.res_info).memflags &=~ (yyvsp[0].memflags).off;
	  }
#line 4041 "rcparse.c"
    break;

  case 219: /* memflags_move: %empty  */
#line 1621 "rcparse.y"
          {
	    memset (&(yyval.res_info), 0, sizeof (rc_res_res_info));
	    (yyval.res_info).language = language;
	    (yyval.res_info).memflags = MEMFLAG_MOVEABLE | MEMFLAG_PURE | MEMFLAG_DISCARDABLE;
	  }
#line 4051 "rcparse.c"
    break;

  case 220: /* memflags_move: memflags_move memflag  */
#line 1627 "rcparse.y"
          {
	    (yyval.res_info) = (yyvsp[-1].res_info);
	    (yyval.res_info).memflags |= (yyvsp[0].memflags).on;
	    (yyval.res_info).memflags &=~ (yyvsp[0].memflags).off;
	  }
#line 4061 "rcparse.c"
    break;

  case 221: /* memflag: MOVEABLE  */
#line 1639 "rcparse.y"
          {
	    (yyval.memflags).on = MEMFLAG_MOVEABLE;
	    (yyval.memflags).off = 0;
	  }
#line 4070 "rcparse.c"
    break;

  case 222: /* memflag: FIXED  */
#line 1644 "rcparse.y"
          {
	    (yyval.memflags).on = 0;
	    (yyval.memflags).off = MEMFLAG_MOVEABLE;
	  }
#line 4079 "rcparse.c"
    break;

  case 223: /* memflag: PURE  */
#line 1649 "rcparse.y"
          {
	    (yyval.memflags).on = MEMFLAG_PURE;
	    (yyval.memflags).off = 0;
	  }
#line 4088 "rcparse.c"
    break;

  case 224: /* memflag: IMPURE  */
#line 1654 "rcparse.y"
          {
	    (yyval.memflags).on = 0;
	    (yyval.memflags).off = MEMFLAG_PURE;
	  }
#line 4097 "rcparse.c"
    break;

  case 225: /* memflag: PRELOAD  */
#line 1659 "rcparse.y"
          {
	    (yyval.memflags).on = MEMFLAG_PRELOAD;
	    (yyval.memflags).off = 0;
	  }
#line 4106 "rcparse.c"
    break;

  case 226: /* memflag: LOADONCALL  */
#line 1664 "rcparse.y"
          {
	    (yyval.memflags).on = 0;
	    (yyval.memflags).off = MEMFLAG_PRELOAD;
	  }
#line 4115 "rcparse.c"
    break;

  case 227: /* memflag: DISCARDABLE  */
#line 1669 "rcparse.y"
          {
	    (yyval.memflags).on = MEMFLAG_DISCARDABLE;
	    (yyval.memflags).off = 0;
	  }
#line 4124 "rcparse.c"
    break;

  case 228: /* file_name: QUOTEDSTRING  */
#line 1679 "rcparse.y"
          {
	    (yyval.s) = (yyvsp[0].s);
	  }
#line 4132 "rcparse.c"
    break;

  case 229: /* file_name: STRING  */
#line 1683 "rcparse.y"
          {
	    (yyval.s) = (yyvsp[0].s);
	  }
#line 4140 "rcparse.c"
    break;

  case 230: /* res_unicode_string_concat: res_unicode_string  */
#line 1691 "rcparse.y"
          {
	    (yyval.uni) = (yyvsp[0].uni);
	  }
#line 4148 "rcparse.c"
    break;

  case 231: /* res_unicode_string_concat: res_unicode_string_concat res_unicode_string  */
#line 1696 "rcparse.y"
          {
	    rc_uint_type l1 = unichar_len ((yyvsp[-1].uni));
	    rc_uint_type l2 = unichar_len ((yyvsp[0].uni));
	    unichar *h = (unichar *) res_alloc ((l1 + l2 + 1) * sizeof (unichar));
	    if (l1 != 0)
	      memcpy (h, (yyvsp[-1].uni), l1 * sizeof (unichar));
	    if (l2 != 0)
	      memcpy (h + l1, (yyvsp[0].uni), l2  * sizeof (unichar));
	    h[l1 + l2] = 0;
	    (yyval.uni) = h;
	  }
#line 4164 "rcparse.c"
    break;

  case 232: /* res_unicode_string: QUOTEDUNISTRING  */
#line 1711 "rcparse.y"
          {
	    (yyval.uni) = unichar_dup ((yyvsp[0].uni));
	  }
#line 4172 "rcparse.c"
    break;

  case 233: /* res_unicode_string: QUOTEDSTRING  */
#line 1715 "rcparse.y"
          {
	    unichar *h = NULL;
	    unicode_from_ascii ((rc_uint_type *) NULL, &h, (yyvsp[0].s));
	    (yyval.uni) = h;
	  }
#line 4182 "rcparse.c"
    break;

  case 234: /* res_unicode_sizedstring: sizedunistring  */
#line 1724 "rcparse.y"
          {
	    (yyval.suni) = (yyvsp[0].suni);
	  }
#line 4190 "rcparse.c"
    break;

  case 235: /* res_unicode_sizedstring: sizedstring  */
#line 1728 "rcparse.y"
          {
	    unichar *h = NULL;
	    rc_uint_type l = 0;
	    unicode_from_ascii_len (&l, &h, (yyvsp[0].ss).s, (yyvsp[0].ss).length);
	    (yyval.suni).s = h;
	    (yyval.suni).length = l;
	  }
#line 4202 "rcparse.c"
    break;

  case 236: /* res_unicode_sizedstring_concat: res_unicode_sizedstring  */
#line 1740 "rcparse.y"
          {
	    (yyval.suni) = (yyvsp[0].suni);
	  }
#line 4210 "rcparse.c"
    break;

  case 237: /* res_unicode_sizedstring_concat: res_unicode_sizedstring_concat res_unicode_sizedstring  */
#line 1745 "rcparse.y"
          {
	    rc_uint_type l1 = (yyvsp[-1].suni).length;
	    rc_uint_type l2 = (yyvsp[0].suni).length;
	    unichar *h = (unichar *) res_alloc ((l1 + l2 + 1) * sizeof (unichar));
	    if (l1 != 0)
	      memcpy (h, (yyvsp[-1].suni).s, l1 * sizeof (unichar));
	    if (l2 != 0)
	      memcpy (h + l1, (yyvsp[0].suni).s, l2  * sizeof (unichar));
	    h[l1 + l2] = 0;
	    (yyval.suni).length = l1 + l2;
	    (yyval.suni).s = h;
	  }
#line 4227 "rcparse.c"
    break;

  case 238: /* sizedstring: SIZEDSTRING  */
#line 1761 "rcparse.y"
          {
	    (yyval.ss) = (yyvsp[0].ss);
	  }
#line 4235 "rcparse.c"
    break;

  case 239: /* sizedstring: sizedstring SIZEDSTRING  */
#line 1765 "rcparse.y"
          {
	    rc_uint_type l = (yyvsp[-1].ss).length + (yyvsp[0].ss).length;
	    char *h = (char *) res_alloc (l);
	    memcpy (h, (yyvsp[-1].ss).s, (yyvsp[-1].ss).length);
	    memcpy (h + (yyvsp[-1].ss).length, (yyvsp[0].ss).s, (yyvsp[0].ss).length);
	    (yyval.ss).s = h;
	    (yyval.ss).length = l;
	  }
#line 4248 "rcparse.c"
    break;

  case 240: /* sizedunistring: SIZEDUNISTRING  */
#line 1777 "rcparse.y"
          {
	    (yyval.suni) = (yyvsp[0].suni);
	  }
#line 4256 "rcparse.c"
    break;

  case 241: /* sizedunistring: sizedunistring SIZEDUNISTRING  */
#line 1781 "rcparse.y"
          {
	    rc_uint_type l = (yyvsp[-1].suni).length + (yyvsp[0].suni).length;
	    unichar *h = (unichar *) res_alloc (l * sizeof (unichar));
	    memcpy (h, (yyvsp[-1].suni).s, (yyvsp[-1].suni).length * sizeof (unichar));
	    memcpy (h + (yyvsp[-1].suni).length, (yyvsp[0].suni).s, (yyvsp[0].suni).length  * sizeof (unichar));
	    (yyval.suni).s = h;
	    (yyval.suni).length = l;
	  }
#line 4269 "rcparse.c"
    break;

  case 242: /* styleexpr: parennumber  */
#line 1803 "rcparse.y"
          {
	    style |= (yyvsp[0].il);
	  }
#line 4277 "rcparse.c"
    break;

  case 243: /* styleexpr: NOT parennumber  */
#line 1807 "rcparse.y"
          {
	    style &=~ (yyvsp[0].il);
	  }
#line 4285 "rcparse.c"
    break;

  case 244: /* styleexpr: styleexpr '|' parennumber  */
#line 1811 "rcparse.y"
          {
	    style |= (yyvsp[0].il);
	  }
#line 4293 "rcparse.c"
    break;

  case 245: /* styleexpr: styleexpr '|' NOT parennumber  */
#line 1815 "rcparse.y"
          {
	    style &=~ (yyvsp[0].il);
	  }
#line 4301 "rcparse.c"
    break;

  case 246: /* parennumber: NUMBER  */
#line 1822 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].i).val;
	  }
#line 4309 "rcparse.c"
    break;

  case 247: /* parennumber: '(' numexpr ')'  */
#line 1826 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[-1].il);
	  }
#line 4317 "rcparse.c"
    break;

  case 248: /* optcnumexpr: %empty  */
#line 1835 "rcparse.y"
          {
	    (yyval.il) = 0;
	  }
#line 4325 "rcparse.c"
    break;

  case 249: /* optcnumexpr: cnumexpr  */
#line 1839 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].il);
	  }
#line 4333 "rcparse.c"
    break;

  case 250: /* cnumexpr: ',' numexpr  */
#line 1848 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].il);
	  }
#line 4341 "rcparse.c"
    break;

  case 251: /* numexpr: sizednumexpr  */
#line 1857 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].i).val;
	  }
#line 4349 "rcparse.c"
    break;

  case 252: /* sizednumexpr: NUMBER  */
#line 1866 "rcparse.y"
          {
	    (yyval.i) = (yyvsp[0].i);
	  }
#line 4357 "rcparse.c"
    break;

  case 253: /* sizednumexpr: '(' sizednumexpr ')'  */
#line 1870 "rcparse.y"
          {
	    (yyval.i) = (yyvsp[-1].i);
	  }
#line 4365 "rcparse.c"
    break;

  case 254: /* sizednumexpr: '~' sizednumexpr  */
#line 1874 "rcparse.y"
          {
	    (yyval.i).val = ~ (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[0].i).dword;
	  }
#line 4374 "rcparse.c"
    break;

  case 255: /* sizednumexpr: '-' sizednumexpr  */
#line 1879 "rcparse.y"
          {
	    (yyval.i).val = - (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[0].i).dword;
	  }
#line 4383 "rcparse.c"
    break;

  case 256: /* sizednumexpr: sizednumexpr '*' sizednumexpr  */
#line 1884 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val * (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4392 "rcparse.c"
    break;

  case 257: /* sizednumexpr: sizednumexpr '/' sizednumexpr  */
#line 1889 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val / (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4401 "rcparse.c"
    break;

  case 258: /* sizednumexpr: sizednumexpr '%' sizednumexpr  */
#line 1894 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val % (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4410 "rcparse.c"
    break;

  case 259: /* sizednumexpr: sizednumexpr '+' sizednumexpr  */
#line 1899 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val + (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4419 "rcparse.c"
    break;

  case 260: /* sizednumexpr: sizednumexpr '-' sizednumexpr  */
#line 1904 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val - (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4428 "rcparse.c"
    break;

  case 261: /* sizednumexpr: sizednumexpr '&' sizednumexpr  */
#line 1909 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val & (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4437 "rcparse.c"
    break;

  case 262: /* sizednumexpr: sizednumexpr '^' sizednumexpr  */
#line 1914 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val ^ (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4446 "rcparse.c"
    break;

  case 263: /* sizednumexpr: sizednumexpr '|' sizednumexpr  */
#line 1919 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val | (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4455 "rcparse.c"
    break;

  case 264: /* cposnumexpr: ',' posnumexpr  */
#line 1930 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].il);
	  }
#line 4463 "rcparse.c"
    break;

  case 265: /* posnumexpr: sizedposnumexpr  */
#line 1939 "rcparse.y"
          {
	    (yyval.il) = (yyvsp[0].i).val;
	  }
#line 4471 "rcparse.c"
    break;

  case 266: /* sizedposnumexpr: NUMBER  */
#line 1950 "rcparse.y"
          {
	    (yyval.i) = (yyvsp[0].i);
	  }
#line 4479 "rcparse.c"
    break;

  case 267: /* sizedposnumexpr: '(' sizednumexpr ')'  */
#line 1954 "rcparse.y"
          {
	    (yyval.i) = (yyvsp[-1].i);
	  }
#line 4487 "rcparse.c"
    break;

  case 268: /* sizedposnumexpr: '~' sizednumexpr  */
#line 1958 "rcparse.y"
          {
	    (yyval.i).val = ~ (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[0].i).dword;
	  }
#line 4496 "rcparse.c"
    break;

  case 269: /* sizedposnumexpr: sizedposnumexpr '*' sizednumexpr  */
#line 1963 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val * (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4505 "rcparse.c"
    break;

  case 270: /* sizedposnumexpr: sizedposnumexpr '/' sizednumexpr  */
#line 1968 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val / (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4514 "rcparse.c"
    break;

  case 271: /* sizedposnumexpr: sizedposnumexpr '%' sizednumexpr  */
#line 1973 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val % (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4523 "rcparse.c"
    break;

  case 272: /* sizedposnumexpr: sizedposnumexpr '+' sizednumexpr  */
#line 1978 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val + (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4532 "rcparse.c"
    break;

  case 273: /* sizedposnumexpr: sizedposnumexpr '-' sizednumexpr  */
#line 1983 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val - (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4541 "rcparse.c"
    break;

  case 274: /* sizedposnumexpr: sizedposnumexpr '&' sizednumexpr  */
#line 1988 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val & (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4550 "rcparse.c"
    break;

  case 275: /* sizedposnumexpr: sizedposnumexpr '^' sizednumexpr  */
#line 1993 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val ^ (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4559 "rcparse.c"
    break;

  case 276: /* sizedposnumexpr: sizedposnumexpr '|' sizednumexpr  */
#line 1998 "rcparse.y"
          {
	    (yyval.i).val = (yyvsp[-2].i).val | (yyvsp[0].i).val;
	    (yyval.i).dword = (yyvsp[-2].i).dword || (yyvsp[0].i).dword;
	  }
#line 4568 "rcparse.c"
    break;


#line 4572 "rcparse.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 2004 "rcparse.y"


/* Set the language from the command line.  */

void
rcparse_set_language (int lang)
{
  language = lang;
}
