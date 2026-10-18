/*
   bug-4153.c
 */

#include <testfwk.h>

#include <stdint.h>
#include <string.h>

struct entry {
  uint8_t prio;
  uint8_t mac[6];
};

struct pkt {
  uint8_t id;
};

__xdata struct entry ent_a[4], ent_b[4];
__xdata uint16_t id[4];
__xdata uint32_t cost[4];
__xdata uint16_t timer[4];
__xdata uint32_t scratch;
uint16_t flags;
uint8_t *pkt;
uint8_t buf[8];

uint8_t check(uint8_t port) __reentrant;

void update(uint8_t port) __reentrant
{
  if (timer[port] && check(port))
    if (timer[port])
      flags &= 0;
  memcpy(ent_a[port].mac, pkt, 6);
  ent_b[port].prio = id[port] = ((struct pkt *)pkt)->id;
  cost[port] = scratch;
  timer[port] = port;
}

uint8_t check(uint8_t port) __reentrant
{
  return port == 0xff;
}

void testBug(void)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
    buf[i] = i + 0x10;
  pkt = buf;
  scratch = 0x12345678;
  timer[3] = 1;
  update(3);
  ASSERT(id[3] == 0x10);
  ASSERT(ent_b[3].prio == 0x10);
  ASSERT(ent_a[3].mac[5] == 0x15);
  ASSERT(cost[3] == 0x12345678);
  ASSERT(timer[3] == 3);
  ASSERT(id[0] == 0 && id[1] == 0 && id[2] == 0);
}
