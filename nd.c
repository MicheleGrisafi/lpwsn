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
  .nd_epoch_end = end_epoch
  };
/*---------------------------------------------------------------------------*/
struct beacon tx_beacon = {
  .node_id = node_id
};
/*---------------------------------------------------------------------------*/
bool bursting;
/*---------------------------------------------------------------------------*/
rtimer timer_end_rx_slot;
rtimer timer_end_tx_slot;
rtimer timer_end_epoch;
/*---------------------------------------------------------------------------*/
uint8_t unsigned slots;
/*---------------------------------------------------------------------------*/
void
nd_recv(void)
{
  /* New packet received
   * 1. Read packet from packetbuf---packetbuf_dataptr()
   * 2. If a new neighbor is discovered within the epoch, notify the application
   */
  uint8_t* payload = packetbuf_dataptr();
  unsigned short neighbour = (beacon)payload->node_id;
  printf("found a new neighbour %d",neighbour);
}
/*---------------------------------------------------------------------------*/
void
nd_start(uint8_t mode, const struct nd_callbacks *cb)
{
  /* Start selected ND primitive and set nd_callbacks */
  radio_driver radio;
  timer_end_rx_slot = RTIMER_TASK();
  timer_end_tx_slot = RTIMER_TASK();
  timer_end_epoch = RTIMER_TASK();

  if(mode==ND_BURST){
    
    burst_start_tx_slot();
  }else{

  }
}
/*---------------------------------------------------------------------------*/

void end_epoch(uint16_t epoch, uint8_t num_nbr){
  printf("The epoch %d has ended with a total of %d neighbours discovered",epoch,num_nbr);
  if(mode==ND_BURST){
    burst_start_tx_slot();   
  }
}

/* BURST MODE */
void start_burst(){
  // Set the bursting boolean to true to let the app know that it should be 
  // transmitting.
  bursting=true;
  // (Re)set the total number of slots to end the epoch at the right time
  slots=TOTAL_SLOTS;
  // Set a timer for the end of the TX slot
  rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,burst_end_rx_slot,NULL);
}

void burst_start_tx_slot(){
  start_burst();
}

void burst_end_tx_slot(){
  bursting=false;
  slots--;
  // Enter the RX slot
  burst_start_rx_slot();
}

void burst_start_rx_slot(){
  radio.on();
  //Set the end of the RX window
  rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,burst_end_rx_slot,NULL);
  //Set the end of the reception
  rtimer_set(&timer_end_listen,RTIMER_NOW()+duration,NULL,burst_stop_listen,NULL);
}

void burst_end_rx_slot(){
  slots--;
  if(slots>0){
    start_rx_slot();
  }else{
    //The epoch has ended
    end_epoch();
  }
}

void burst_stop_listen(){
  radio.off();
}

void send_beacon(){
  radio.send(&tx_beacon, sizeof(tx_beacon));
  if (bursting){ 
    send_beacon();
  }
}