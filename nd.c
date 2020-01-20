/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/random.h"
#include "sys/rtimer.h"
#include "dev/radio.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "node-id.h"
/*---------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
/*---------------------------------------------------------------------------*/
#include "nd.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
struct nd_callbacks app_cb = {
  .nd_new_nbr = NULL,
  .nd_epoch_end = NULL
  };
/*---------------------------------------------------------------------------*/
struct beacon tx_beacon = {
  .node_id = node_id;
};
/*---------------------------------------------------------------------------*/
bool bursting;
void
nd_recv(void)
{
  /* New packet received
   * 1. Read packet from packetbuf---packetbuf_dataptr()
   * 2. If a new neighbor is discovered within the epoch, notify the application
   */
}
/*---------------------------------------------------------------------------*/
void
nd_start(uint8_t mode, const struct nd_callbacks *cb)
{
  /* Start selected ND primitive and set nd_callbacks */
  radio_driver radio;
  if(mode==ND_BURST){

  }else{

  }
}
/*---------------------------------------------------------------------------*/

void start_rx(rtimer_clock_t duration){
  radio.on();
  rtimer rx_task = RTIMER_TASK();
  rtimer_set(&rx_task,RTIMER_NOW()+duration,NULL,stop_rx,NULL);
}

void start_burst(){
  bursting=true;
  rtimer burst_task = RTIMER_TASK();
  rtimer_set(&burst_task,RTIMER_NOW()+SLOT_DURATION,NULL,stop_burst,NULL);
}

void stop_burst(void *ptr){
  bursting=false;
  
}

void stop_rx(void *ptr){
  radio.off();
}

void send_beacon(){
  radio.send(&tx_beacon, sizeof(tx_beacon));
  if (bursting){ 
    send_beacon();
  }
}