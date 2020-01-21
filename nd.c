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
  .nd_new_nbr = new_discovery,
  .nd_epoch_end = end_epoch
  };
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
bool bursting;
/*---------------------------------------------------------------------------*/
static struct rtimer timer_end_rx_slot;
static struct rtimer timer_end_tx_slot;
static struct rtimer timer_end_epoch;
/*---------------------------------------------------------------------------*/
uint8_t slots;
uint16_t epoch_number;
uint8_t tot_neigh;
/*---------------------------------------------------------------------------*/
static struct radio_driver radio;
/*---------------------------------------------------------------------------*/
unsigned short discovered_nodes[MAX_NBR];
/*---------------------------------------------------------------------------*/
void
nd_recv(void)
{
  /* New packet received
   * 1. Read packet from packetbuf---packetbuf_dataptr()
   * 2. If a new neighbor is discovered within the epoch, notify the application
   */
  uint8_t* payload = packetbuf_dataptr();
  unsigned short neighbour = *payload;

  printf("found a new neighbour %d",neighbour);
}
/*---------------------------------------------------------------------------*/
void
nd_start(uint8_t mode, const struct nd_callbacks *cb)
{
  /* Start selected ND primitive and set nd_callbacks */
  epoch_number = 0;
  slots=TOTAL_SLOTS;
  tot_neigh=0;

  if(mode==ND_BURST){
    start_tx_slot();
  }else{
    
  }
}
/*---------------------------------------------------------------------------*/

void end_epoch(uint16_t epoch, uint8_t num_nbr){
  printf("The epoch %d has ended with a total of %d neighbours discovered",epoch,num_nbr);
  // (Re)set the total number of slots to end the epoch at the right time
  slots=TOTAL_SLOTS;
  epoch_number++;
  tot_neigh = 0;
  
  if(mode==ND_BURST){
    start_tx_slot(mode);
  }else{
    start_rx_slot(mode);
  }
}

void new_discovery(uint16_t epoch, uint8_t nbr_id){
  if(!contains(&discovered_nodes,&nbr_id)){
    printf("New node %d was discovered\n", nbr_id);
  }
}


void start_burst(){
  // Set the bursting boolean to true to let the app know that it should be 
  // transmitting.
  bursting=true;

  // Set a timer for the end of the TX slot
  rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_tx_slot,ND_BURST);
}

void start_tx_slot(uint8_t mode){
  if(mode==ND_BURST){
    start_burst();
  }else{
    //Send a single beacon and schedule the end of the slot
    send_beacon();
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_tx_slot,ND_SCATTER);
  }
}

void end_tx_slot(uint8_t mode){
  slots--;
  if(mode==ND_BURST){
    bursting=false;
    // Enter the RX slot
    start_rx_slot();
  }else{
    //Check whether it is the last TX slot and, if not, enter a new one
    if(slots>0){
      start_tx_slot(mode);
    }else{
      end_epoch(epoch_number,tot_neigh);
    }
  }
}

void start_rx_slot(uint8_t mode){
  radio.on();
  if(mode==ND_BURST){
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_rx_slot,mode);
    //Set the end of the reception
    rtimer_set(&timer_end_listen,RTIMER_NOW()+____,NULL,stop_listen,NULL);  
  }else{
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_rx_slot,mode);
  }
}

void end_rx_slot(uint8_t mode){
  slots--;
  if(mode==ND_BURST){
    if(slots>0){
      start_rx_slot(mode);
    }else{
      //The epoch has ended
      end_epoch(epoch_number,tot_neigh);
    }
  }else{
    //Turn off the radio and start the TX slots
    radio.off();
    start_tx_slot(mode);
  }
}

void stop_listen(){
  radio.off();
}

void send_beacon(){
  unsigned short payload = node_id;
  radio.send(&payload, sizeof(payload));
  if (bursting){ 
    send_beacon();
  }
}





int contains(unsigned short *array,unsigned short * toBeSearched){
  int found = 0;
  for(i = 0; i < sizeof(*array) / sizeof(unsigned short); i++){
    if(*array[i] == *toBeSearched){
      found = 1
      break;
    }
  }
  return found;
}