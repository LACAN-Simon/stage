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
// #include "net/rpl/rpl-dag.h"
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

static struct simple_udp_connection broadcast_connection;

extern const struct sensors_sensor temperature_sensor;
// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
//

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_example_process, "UDP broadcast example process");
AUTOSTART_PROCESSES(&broadcast_example_process);
 float tab[3];
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  printf("Received;%s\n",
         data);
}

static int tabs(float a, float b) {
    float difference = a - b;
    if (difference < 0) {
        return -difference;
    } else {
        return difference;
    }
}

static void config_pressure()
{
  pressure_sensor.configure(PRESSURE_SENSOR_DATARATE, LPS331AP_P_12_5HZ_T_1HZ);
  SENSORS_ACTIVATE(pressure_sensor);
}

static void config_light()
{
  light_sensor.configure(LIGHT_SENSOR_SOURCE, ISL29020_LIGHT__AMBIENT);
  light_sensor.configure(LIGHT_SENSOR_RESOLUTION, ISL29020_RESOLUTION__16bit);
  light_sensor.configure(LIGHT_SENSOR_RANGE, ISL29020_RANGE__1000lux);
  SENSORS_ACTIVATE(light_sensor);
}
static float process_pressure()
{
  int pressure;
  pressure = pressure_sensor.value(0);
  return (float)pressure / PRESSURE_SENSOR_VALUE_SCALE;
}

static float process_light()
{
  int light_val = light_sensor.value(0);
  float light = ((float)light_val) / LIGHT_SENSOR_VALUE_SCALE;
  return light;
}

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
static uint32_t 
pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void 
pcg32_srandom_r(pcg32_random_t* rng, 
		uint64_t initstate, 
		uint64_t initseq)
{
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    pcg32_random_r(rng);
    rng->state += initstate;
    pcg32_random_r(rng);
}
//
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_example_process, ev, data)
{
  static unsigned long initstate, initseq;
  static struct etimer periodic_timer;
  char send_buffer[SEND_BUFFER_SIZE];
  static struct etimer begin_timer;
  static struct etimer send_timer;
  static pcg32_random_t rng;
  uip_ipaddr_t addr;
  uint32_t id;
  char *eptr;
  int i;
  int u = 1;
  PROCESS_BEGIN();
  simple_udp_register(&broadcast_connection, UDP_PORT,
                      NULL, UDP_PORT,
                      receiver);
  clock_init();
  uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0x5cc2, 0xf20f, 0xba77, 0xa63b);

  PROCESS_YIELD();
  if (ev == serial_line_event_message) {
    initstate = strtoul((char*)data, &eptr, 10);
  }

  PROCESS_YIELD();
  if (ev == serial_line_event_message) {
    initseq = strtoul((char*)data, &eptr, 10);
  }

  etimer_set(&begin_timer, BEGIN_INTERVAL);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&begin_timer));

  printf("Init state : %lu\n", initstate);
  printf("Init seq : %lu\n", initseq);
  pcg32_srandom_r(&rng, initstate, initseq);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
    etimer_set(&send_timer, SEND_TIME);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    void rpl_neighbor_print_list(void);
    //id = nid * clock_seconds();
    id = pcg32_random_r(&rng);    
    for (i=0; i<NB_PACKETS; i++) { 
	int16_t temp = 0 ;
	uint8_t res = lps331ap_read_temp(&temp);
        float t = 42.5 + temp / 480 ;
	config_pressure();
	config_light();
	float l = process_light();
	float p = process_pressure();
	    
	if (tabs(l,tab[0])>u && tabs(p,tab[1])>u && tabs(t,tab[2])>u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,L=%.2f;P=%.2f;T=%.1f",i+id,l,p,t);
		printf("Send=%s\n", send_buffer);  
		//uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[0]=l;
		tab[1]=p;
		tab[2]=t;
	}
	    
	else if (tabs(l,tab[0])<u && tabs(p,tab[1])>u && tabs(t,tab[2])>u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx;P=%.2f;T=%.1f",i+id,p,t);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[1]=p;
		tab[2]=t;		
	}
	   
	else if (tabs(l,tab[0])>u && tabs(p,tab[1])<u && tabs(t,tab[2])>u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,L=%.2f;T=%.1f",i+id,l,t);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[0]=l;
		tab[2]=t;
	}
	  
	else if (tabs(l,tab[0])>u && tabs(p,tab[1])>u && tabs(t,tab[2])<u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,L=%.2f;P=%.2f",i+id,l,p);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[0]=l;
		tab[1]=p;
	}
	    
	else if (tabs(l,tab[0])>u && tabs(p,tab[1])<u && tabs(t,tab[2])<u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,L=%.2f",i+id,l);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[0]=l;
	}
	    
	else if (tabs(l,tab[0])<u && tabs(p,tab[1])>u && tabs(t,tab[2])<u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,P=%.2f",i+id,p);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[1]=p;
	}
	    
	else if (tabs(l,tab[0])<u && tabs(p,tab[1])<u && tabs(t,tab[2])>u){
		snprintf(send_buffer, sizeof(uint32_t)*30, "ID:%lx,T=%.1f",i+id,t);
		printf("Send=%s\n", send_buffer);  
// 		uip_create_linklocal_allnodes_mcast(&addr);
		simple_udp_sendto(&broadcast_connection,send_buffer,sizeof(send_buffer), &addr) ;
		tab[2]=t;
	}
    } 
  }
	
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
