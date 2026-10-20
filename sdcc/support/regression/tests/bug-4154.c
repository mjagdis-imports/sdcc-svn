/*
   bug-4154.c
 */

#include <testfwk.h>

#include <stdint.h>

struct bridge {
  uint8_t mac;
};
__xdata uint8_t buf[16];
__xdata struct bridge dbridge[4];
__xdata uint16_t dpid[4];
__xdata uint8_t rxage[4];
__xdata uint8_t rxfwd[4];
__xdata uint16_t info_while[4];
uint8_t rxbody;
struct pkt {
  struct bridge bridge;
  uint16_t fwd_delay;
};
int8_t cmp(__xdata uint8_t *, __xdata uint8_t *, uint8_t) __reentrant;
uint8_t clamp(uint8_t, uint8_t, uint8_t, uint8_t);
void rcv(uint8_t port) __reentrant {
  if (info_while[port] &&
          cmp((__xdata uint8_t *)((__xdata struct pkt *)buf)->bridge.mac,
                   &dbridge[port].mac, 1) ||
      dpid[port])
    return;
  rxage[port] = rxfwd[port] = clamp(
      ((__xdata struct pkt *)&buf[rxbody])->fwd_delay, 4, 30, 15);
  info_while[port] = port;
}

int8_t cmp(__xdata uint8_t *a, __xdata uint8_t *b, uint8_t n) __reentrant
{
  while (n--)
    if (*a++ != *b++)
      return 1;
  return 0;
}

uint8_t clamp(uint8_t v, uint8_t lo, uint8_t hi, uint8_t d)
{
  return (v < lo || v > hi) ? d : v;
}

void testBug(void)
{
  ((__xdata struct pkt *)buf)->fwd_delay = 9;
  rxbody = 0;
  rcv(2);
  ASSERT(rxfwd[2] == 9);
  ASSERT(rxage[2] == 9);
  ASSERT(info_while[2] == 2);
  ASSERT(info_while[0] == 0 && info_while[1] == 0 && info_while[3] == 0);
}
