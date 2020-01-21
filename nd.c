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
static struct rtimer timer_turn_off_radio;
static struct rtimer timer_end_epoch;
/*---------------------------------------------------------------------------*/
uint8_t slots;
uint16_t epoch_number;
uint8_t tot_neigh;
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
    bursting=false;
    start_rx_slot();
  }
}
/*---------------------------------------------------------------------------*/

void end_epoch(uint16_t epoch, uint8_t num_nbr){
  printf("The epoch %d has ended with a total of %d neighbours discovered",epoch,num_nbr);
  // (Re)set the total number of slots to end the epoch at the right time
  slots=TOTAL_SLOTS;
  epoch_number++;
  tot_neigh = 0;
}

void new_discovery(uint16_t epoch, uint8_t nbr_id){
  if(!contains(&discovered_nodes,&nbr_id)){
    printf("New node %d was discovered\n", nbr_id);
  }
}

/**
 * \brief      Initialize a TX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_tx_slot(uint8_t mode){
  if(mode==ND_BURST){
    // Set the bursting boolean to true. As long as it is true the node will transmit repeatedly
    bursting=true;
    // Set a timer for the end of the TX slot
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_tx_slot,ND_BURST);
  }else{
    //Send a single beacon and schedule the end of the slot
    send_beacon();
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_tx_slot,ND_SCATTER);
  }
}

/**
 * \brief      Terminate a TX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_tx_slot(uint8_t mode){
  //Decrease the slot counter to keep track of the epoch
  slots--;
  if(mode==ND_BURST){
    // terminate the burst of beacons
    bursting=false;
    // Enter the first RX slot
    start_rx_slot(mode);
  }else{
    if(slots>0){
      // It's not the last slot, so a new one needs to be initiated
      start_tx_slot(mode);
    }else{
      // It is the last slot, the epoch needs to end
      end_epoch(epoch_number,tot_neigh);
      //Begin new epoch by entering RX again
      start_rx_slot(mode);
    }
  }
}

/**
 * \brief      Initiate a RX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_rx_slot(uint8_t mode){
  // Turn the radio on so that packets can be received
  NETSTACK_RADIO.on();
  if(mode==ND_BURST){
    //Since the reception is shorter than the actual RX slot, two different timers need to be initiated
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_rx_slot,mode);
    //Set the end of the reception
    rtimer_set(&timer_turn_off_radio,RTIMER_NOW()+____,NULL,stop_listen,NULL);  
  }else{
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_rx_slot,mode);
  }
}

/**
 * \brief      Terminate a RX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_rx_slot(uint8_t mode){
  //Decrease the slot counter to keep track of the epoch
  slots--;
  if(mode==ND_BURST){
    if(slots>0){
      //The epoch is not terminated yet, thus a new RX slot is initiated
      start_rx_slot(mode);
    }else{
      //The epoch has ended
      end_epoch(epoch_number,tot_neigh);
      //Start new epoch by entering TX again
      start_tx_slot(mode);
    }
  }else{
    //Turn off the radio and start the TX slots
    NETSTACK_RADIO.off();
    start_tx_slot(mode);
  }
}

/**
 * \brief      Switch off the radio
 */
void stop_listen(){
  NETSTACK_RADIO.off();
}

/**
 * \brief      Send a beacon for Neighbour Discovery with the node_id
 */
void send_beacon(){
  //Retrieve the node_id
  unsigned short payload = node_id;
  //Broadcast the packet
  NETSTACK_RADIO.send(&payload, sizeof(payload));
  if (bursting){
    //Since it's bursting then another beacon should be sent right away 
    send_beacon();
  }
}

/**
 * \brief       Check wheter the neighbour array has already the selected entry
 * \param array         Array in which the element should be looked for. It should contain all of the discovered neighbours
 * \param toBeSearched  Element to be searched
 * \return     Boolean value depending on the outcome of the search.
 */
bool contains(unsigned short *array,unsigned short * toBeSearched){
  bool found = false;
  for(i = 0; i < sizeof(*array) / sizeof(unsigned short); i++){
    if(*array[i] == *toBeSearched){
      found = true;
      break;
    }
  }
  return found;
}