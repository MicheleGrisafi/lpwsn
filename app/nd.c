/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/random.h"
#include "sys/rtimer.h"
#include "dev/radio.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "node-id.h"
#include "utils.h"
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

static unsigned long SLOT_DURATION; 
static unsigned long RX_DURATION; 
static unsigned long BEACON_INTERVAL; 
/*---------------------------------------------------------------------------*/
unsigned long end_tx_window;
unsigned long remaining_tx_slot;
/*---------------------------------------------------------------------------*/
uint8_t protocol_mode;
/*---------------------------------------------------------------------------*/
static struct rtimer timer_end_rx_slot;
static struct rtimer timer_end_tx_slot;
static struct rtimer timer_turn_off_radio;
static struct rtimer timer_next_beacon;
/*---------------------------------------------------------------------------*/
uint8_t slots;
uint16_t epoch_number;
uint8_t tot_neigh;
/*---------------------------------------------------------------------------*/
uint8_t discovered_nodes[MAX_NBR];
/*---------------------------------------------------------------------------*/
uint8_t payload;
/*---------------------------------------------------------------------------*/


void
nd_recv(void){
  /* New packet received
   * 1. Read packet from packetbuf---packetbuf_dataptr()
   * 2. If a new neighbor is discovered within the epoch, notify the application
   */
  uint8_t* payload = packetbuf_dataptr();
  
  if(!contains(discovered_nodes,tot_neigh,*payload)){
    PRINTF("DEBUG: discobery in %d of %d\n",epoch_number,*payload);
    new_discovery(epoch_number, *payload);
  }else{
    PRINTF("DEBUG: Neighbour already discovered\n");
  }
}
/*---------------------------------------------------------------------------*/
void
nd_start(uint8_t mode, const struct nd_callbacks *cb){
  /* Start selected ND primitive and set nd_callbacks */
  epoch_number = 1;
  slots=TOTAL_SLOTS;
  tot_neigh=0;
  app_cb.nd_new_nbr = cb->nd_new_nbr;
  app_cb.nd_epoch_end = cb->nd_epoch_end;
  protocol_mode=mode;

  payload=(uint8_t)node_id;
  
  SLOT_DURATION = EPOCH_INTERVAL_RT/TOTAL_SLOTS;
  BEACON_INTERVAL = SLOT_DURATION/20.0;
  RX_DURATION = BEACON_INTERVAL*3.0;
  
  PRINTF("DEBUG: the following duration will be used:\n  Beacon Interval: %ld\n  RX duration: %ld\n",BEACON_INTERVAL,RX_DURATION);
  
  if(protocol_mode==ND_BURST){
    start_tx_slot(NULL,&protocol_mode);
  }else{
    start_rx_slot(NULL,&protocol_mode);
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      End the epoch by signaling the application of the discovered nodes
 * \param epoch The number of the epoch that jsut ended
 * \param num_nbr The number of neighbours discovered
 */
void end_epoch(uint16_t epoch, uint8_t num_nbr){
  // (Re)set the total number of slots to end the epoch at the right time
  slots=TOTAL_SLOTS;
  epoch_number++;
  tot_neigh = 0;
  app_cb.nd_epoch_end(epoch,num_nbr);
}

/**
 * \brief      Notify the application of a new discovery
 * \param epoch The current epoch number 
 * \param nbr_id The ID of the discovered node
 */
void new_discovery(uint16_t epoch, uint8_t nbr_id){
  //PRINTF("DEBUG: New node %d was discovered in epoch %d\n", nbr_id,epoch);
  discovered_nodes[tot_neigh]=nbr_id;
  tot_neigh++;
  app_cb.nd_new_nbr(epoch,nbr_id);
}

/**
 * \brief      Initialize a TX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_tx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: Starting the TX slot and scheduling end in %ld secondsa\n",SLOT_DURATION/RTIMER_SECOND);
  remaining_tx_slot = SLOT_DURATION;
  send_beacon(NULL,mode);
}

/**
 * \brief      Terminate a TX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_tx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: End TX slot\n");
  //Decrease the slot counter to keep track of the epoch
  slots--;
  //Notify the APIs that no further transmission should be taken care of
  if(*mode==ND_BURST){
    // Enter the first RX slot
    start_rx_slot(NULL,mode);
  }else{
    if(slots>0){
      // It's not the last slot, so a new one needs to be initiated
      start_tx_slot(NULL,mode);
    }else{
      // It is the last slot, the epoch needs to end
      end_epoch(epoch_number,tot_neigh);
      //Begin new epoch by entering RX again
      start_rx_slot(NULL,mode);
    }
  }
}

/**
 * \brief      Initiate a RX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_rx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: Starting the RX slot\n");
  if(*mode==ND_BURST){
    rtimer_set(&timer_turn_off_radio,RTIMER_NOW()+RX_DURATION,NULL,stop_listen,mode);  
  }else{
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,NULL,end_rx_slot,mode);
  }
  // Turn the radio on so that packets can be received
  NETSTACK_RADIO.on();
}

/**
 * \brief      Switch off the radio
 */
void stop_listen(struct rtimer *t, uint8_t *mode){
  //Schedule the end of the RX slot
  rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+(SLOT_DURATION-RX_DURATION),NULL,end_rx_slot,mode);
  //Turn off the radio
  NETSTACK_RADIO.off();
}

/**
 * \brief      Terminate a RX slot
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_rx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: Ending the RX slot\n");
  //Decrease the slot counter to keep track of the epoch
  slots--;
  if(*mode==ND_BURST){
    if(slots>0){
      //The epoch is not terminated yet, thus a new RX slot is initiated
      start_rx_slot(NULL,mode);
    }else{
      //The epoch has ended
      end_epoch(epoch_number,tot_neigh);
      //Start new epoch by entering TX again
      start_tx_slot(NULL,mode);
    }
  }else{
    //Turn off the radio and start the TX slots
    NETSTACK_RADIO.off();
    start_tx_slot(NULL,mode);
  }
}



/**
 * \brief      Send a beacon for Neighbour Discovery with the node_id
 */
void send_beacon(struct rtimer *t, uint8_t *mode){
  if (*mode==ND_BURST){
    if(t!=NULL){
      //It means that it is at least the second beacon call
      remaining_tx_slot-=BEACON_INTERVAL;
    }
    if(remaining_tx_slot>=BEACON_INTERVAL){
      //Still need to send beacon,
      rtimer_set(&timer_next_beacon,RTIMER_NOW()+BEACON_INTERVAL,NULL,send_beacon,mode);
    }else{
      //Schedule the end of the TX slot
      rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,NULL,end_tx_slot,mode);
    }
  }else{
    //Schedule the end of the TX slot
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,NULL,end_tx_slot,mode);
  }
  //Send the beacon
  NETSTACK_RADIO.send(&payload, sizeof(payload));
}


void test_beacon(struct rtimer *t, uint8_t *mode){
  //TODO: RANDOM interval for better collision avoidance!
  if (*mode==ND_BURST){
    if(t!=NULL){
      //It means that it is at least the second beacon call
      remaining_tx_slot-=BEACON_INTERVAL;
    }
    if(remaining_tx_slot>=BEACON_INTERVAL){
      //Still need to send beacon,
      rtimer_set(&timer_next_beacon,RTIMER_NOW()+BEACON_INTERVAL,NULL,send_beacon,mode);
    }else{
      //Schedule the end of the TX slot
      rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,NULL,end_tx_slot,mode);
    }
  }else{
    //Schedule the end of the TX slot
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,NULL,end_tx_slot,mode);
  }
  //Send the beacon
  NETSTACK_RADIO.send(&payload, sizeof(payload));
}