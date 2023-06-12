#include "contiki.h"
#include "net/link-stats.h"
#include "net/rpl/rpl-private.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/nbr-table.h"
#include "net/ipv6/multicast/uip-mcast6.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "sys/ctimer.h"
#include <limits.h>
#include <string.h>
#include "net/ip/uip-debug.h"

void rpl_print_neighbor_list(void); 
uip_ds6_nbr_t *rpl_get_nbr(rpl_parent_t *parent);
static void
nbr_callback(void *ptr);

