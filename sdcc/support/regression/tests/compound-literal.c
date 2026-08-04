/** Test compound literals (C99)
 */

#include <testfwk.h>
#include <string.h>

#pragma disable_warning 196
#pragma disable_warning 360

char *str1 = "aaa";
char *str2 = (char[]){'b', 'b', 'b', '\0'};

struct s {
  int i;
};

struct t {
  char a, b, c;
};

union u {
  char c;
  struct { char x, y; };
};

volatile char seed = 5;

// struct s S1 = (struct s){0x24};

void testCompoundLiterals(void)
{
  const char *str1_local = "AAA";
  const char *str2_local = "BBB";
  struct s s1 = {0x42};
  struct s s2;
  static struct s s3 = {0x84};
  s2 = (struct s){0x21};
  str2_local = (char[]){'C', 'C', 'C', '\0'};

  ASSERT(strcmp(str1, "aaa") == 0);
  ASSERT(strcmp(str2, "bbb") == 0);
  ASSERT(strcmp(str1_local, "AAA") == 0);
  ASSERT(strcmp(str2_local, "CCC") == 0);
  // ASSERT(S1.i == 0x24);
  ASSERT(s1.i == 0x42);
  ASSERT(s2.i == 0x21);
  ASSERT(s3.i == 0x84);
}

/* A compound literal in the initializer of a declaration, rather than on the
   right of an assignment. The temporary the literal lives in is created while
   parsing the initializer list, which is turned into a tree only after the
   pass that puts such temporaries into their block has run, so it used to be
   left with neither storage nor a name. */
void testCompoundLiteralInDeclaration(void)
{
  struct t v = (struct t){1, 2, 3};
  struct t *p = &(struct t){4, 5, 6};
  char *a = (char[]){7, 8, 9};
  union u w = (union u){.y = 8};

  ASSERT(v.a == 1);
  ASSERT(v.b == 2);
  ASSERT(v.c == 3);
  ASSERT(p->a == 4);
  ASSERT(p->b == 5);
  ASSERT(p->c == 6);
  ASSERT(a[0] == 7);
  ASSERT(a[1] == 8);
  ASSERT(a[2] == 9);
  ASSERT(w.y == 8);
  // ASSERT(w.x == 0);  // fails for unrelated reasons -- see e.g. patch #508
}

/* The literal's own initializer may name a variable declared earlier in the
   same block, so it has to run after that variable's initializer rather than
   at the top of the block. seed is volatile so that nothing folds. */
void testCompoundLiteralOrder(void)
{
  char x = (char)(seed + 1);
  struct t v = (struct t){x, 2, 3};
  char y = (char)(x + 1);
  struct t w = (struct t){y, x, 4};

  ASSERT(x == 6);
  ASSERT(v.a == 6);
  ASSERT(v.b == 2);
  ASSERT(v.c == 3);
  ASSERT(y == 7);
  ASSERT(w.a == 7);
  ASSERT(w.b == 6);
  ASSERT(w.c == 4);
}
