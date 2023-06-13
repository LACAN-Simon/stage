#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "dev/serial-line.h"
#include "net/ipv6/uip-ds6.h"
#include "lps331ap.h"
#include "simple-udp.h"
#include "dev/pressure-sensor.h"
#include "dev/light-sensor.h"
#include "net//rpl/rpl.h"
//#include "net/rpl/rpl-dag.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//
/*---------------------------------------------------------------------------*/
PROCESS(rpl_creation, "create an rpl network");
AUTOSTART_PROCESSES(&rpl_creation);
PROCESS_THREAD(rpl_creation,ev,data)
{
  printf("création du rpl");
  rpl_init(); 
  uip_ipaddr_t ipaddr;
	
  PROCESS_BEGIN();	
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
  uip_ds6_defrt_add(&ipaddr, 0);
  rpl_dag_t *dag;
  dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
  rpl_set_prefix(dag, &ipaddr, 64);
  rpl_set_mode(RPL_MODE_MESH);
  if(dag != NULL) {
	  printf("DAG created\n");   }
//   rpl_dag_t *dag = rpl_get_any_dag();
  if(dag != NULL && dag->preferred_parent != NULL) {
  printf("Preferred parent IP address: %u\n", dag->preferred_parent->flags);
} else {
  printf("No preferred parent\n");
}
// rpl_dag_t *dag = rpl_get_any_dag();
if(dag != NULL) {
  printf("Node rank: %u\n", dag->rank);
} else {
  printf("DAG not available\n");
}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
