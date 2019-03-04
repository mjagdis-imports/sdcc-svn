void
_putchar(unsigned char c)
{
  c;
  __asm
    .db 0xFF, 0x00
  __endasm;
}

void
_initEmu(void)
{
}

void
_exitEmu(void)
{
  __asm
    stopsys
  __endasm;
}
