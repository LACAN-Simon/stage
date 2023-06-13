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
#define UDP_PORT 1234
#define SEND_BUFFER_SIZE 1000
#define BEGIN_INTERVAL_SECONDS 10 
#define BEGIN_INTERVAL  (BEGIN_INTERVAL_SECONDS * CLOCK_SECOND)
#define NB_PACKETS 1
#define SEND_INTERVAL_SECONDS 6
#define SEND_INTERVAL		(SEND_INTERVAL_SECONDS * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_example_process, "UDP broadcast example process");
AUTOSTART_PROCESSES(&broadcast_example_process);
/*---------------------------------------------------------------------------*/

//
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_example_process, ev, data)
{

  rpl_init(); 
  uip_ipaddr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
  uip_ds6_defrt_add(&ipaddr, 0);
  rpl_dag_t *dag;
  dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
  rpl_set_prefix(dag, &ipaddr, 64);
  rpl_set_mode(RPL_MODE_MESH);
  if(dag != NULL) {
  printf("DAG created\n");
} else {
  printf("DAG creation failed\n");
}
  PROCESS_BEGIN();
  
  while(1) {

  }
	
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
