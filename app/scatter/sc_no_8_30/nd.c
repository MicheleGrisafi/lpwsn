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

#define CA 0
/*---------------------------------------------------------------------------*/
struct nd_callbacks app_cb = {
  .nd_new_nbr = NULL,
  .nd_epoch_end = NULL
  };
/*---------------------------------------------------------------------------*/

static signed long SLOT_DURATION; 
static signed long RX_DURATION; 
static signed long BEACON_INTERVAL;
 
/*---------------------------------------------------------------------------*/
static signed long remaining_tx_slot;
/*---------------------------------------------------------------------------*/
static uint8_t protocol_mode;
/*---------------------------------------------------------------------------*/
static struct rtimer timer_end_rx_slot;
static struct rtimer timer_end_tx_slot;
static struct rtimer timer_turn_off_radio;
static struct rtimer timer_next_beacon;
/*---------------------------------------------------------------------------*/
static uint8_t slots = TOTAL_SLOTS;
static uint16_t epoch_number =1;
static uint8_t tot_neigh = 0;
/*---------------------------------------------------------------------------*/
static uint8_t discovered_nodes[MAX_NBR] = {0};
/*---------------------------------------------------------------------------*/
static uint8_t payload;
/*---------------------------------------------------------------------------*/
static bool transmitting;
/*---------------------------------------------------------------------------*/
/******* COLLISION AVOIDANCE ********/
#if CA
static bcin send_info = {
  .mode = NULL,
  .random = 0
};
static long rnd_list[50]; //50 is an upper limit to the number of beacons
static uint8_t rnd_counter = 0;
#endif
/*---------------------------------------------------------------------------*/


void
nd_start(uint8_t mode, const struct nd_callbacks *cb){
  // Initialize all the variables and callbacks. A bug prevent the use of nd_new_nbr
  epoch_number = 1;
  slots=TOTAL_SLOTS;
  tot_neigh=0;
  protocol_mode=mode;
  /** BUG **
  printf("\nDEBUG: initial %d  %d\n",cb,cb->nd_new_nbr);
  app_cb.nd_new_nbr = cb->nd_new_nbr;
  *** END BUG **/
  app_cb.nd_epoch_end = cb->nd_epoch_end;
  
  //Set the static content of the beacon
  payload=(uint8_t)node_id;

  //Initialize the various durations
  SLOT_DURATION = EPOCH_INTERVAL_RT/TOTAL_SLOTS;
  BEACON_INTERVAL = SLOT_DURATION/30.0;
  RX_DURATION = BEACON_INTERVAL*3.0;
  PRINTF("DEBUG: the following duration will be used:\n  Beacon Interval: %ld\n  RX duration: %ld\n",BEACON_INTERVAL,RX_DURATION);
  #if CA
  /*** Collision avoidance **/
  if (mode == ND_BURST){
    random_init(node_id); //Initialize the random generator with the seed = node_id
    PRINTF("Random generated:");
    //Store 50 random number in an array for later use (faster TX)
    for (rnd_counter = 0; rnd_counter<50; rnd_counter++){
      rnd_list[rnd_counter] = (random_rand() % (BEACON_INTERVAL*2))-BEACON_INTERVAL;
      PRINTF("%d-",rnd_list[rnd_counter]);
    }
    rnd_counter = 0;
    send_info.mode = &protocol_mode;
  }
  
  /*** end Collision Avoidance****/
  #endif
  PRINTF("Starting in mode %d with bcin %d\n",mode,*(send_info.mode));
  //Start protocol by initiating transmission. No matter the mode
  start_tx_slot(NULL,&protocol_mode);
}

/**
 * \brief  A packet has been received by the node
 */
void
nd_recv(void){
  //Check wheter the node's receiving and the packetbuffer contains a valid payload
  if (!transmitting && packetbuf_datalen() == sizeof(payload)){
    uint8_t node;
    memcpy(&node, packetbuf_dataptr(), sizeof(node));
    //Check if it's a valid node not yet discovered
    if(node != 0 && !discovered_nodes[node]){
      new_discovery(epoch_number, node);
    }else{
      PRINTF("DEBUG: Neighbour already discovered or collision\n");
    }
  } 
}


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
  // Reset the neighbour discovery
  memset(discovered_nodes, 0, sizeof(discovered_nodes));
  
  app_cb.nd_epoch_end(epoch,num_nbr);
}

/**
 * \brief      Notify the application of a new discovery
 * \param epoch The current epoch number 
 * \param nbr_id The ID of the discovered node
 */
void new_discovery(uint16_t epoch, uint8_t nbr_id){
  //PRINTF("DEBUG: New node %d was discovered in epoch %d\n", nbr_id,epoch);
  printf("App: Epoch %u New NBR %u\n",epoch, nbr_id);
  discovered_nodes[nbr_id]=1;
  tot_neigh++;

  /** BUG **
  PRINTF("\nDEBUG: disco %d %d\n",&app_cb.nd_new_nbr, app_cb.nd_new_nbr);
  app_cb.nd_new_nbr(epoch,nbr_id);
  ** END BUG **/
}

/**
 * \brief       Callback function for the initialization of a TX slot
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode  The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_tx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: Starting the TX slot and scheduling end in %ld secondsa\n",SLOT_DURATION/RTIMER_SECOND);
  remaining_tx_slot = SLOT_DURATION;
  transmitting = true;
  PRINTF("Starting TX with mode %d and bcin %d\n",*mode,*(send_info.mode));
  #if CA
  /*** collision avoidance **/
  // Reset the random indicator used to keep track of the amount spent waiting
  send_info.random=0; 
  
  send_beacon_random(NULL,&send_info);
  #else
  send_beacon(NULL,mode);
  #endif
}

/**
 * \brief      Callback function for the termination of a TX slot
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_tx_slot(struct rtimer *t, uint8_t *mode){
  //PRINTF("DEBUG: End TX slot\n");
  //Decrease the slot counter to keep track of the epoch
  slots--;
  //Allow the reception
  transmitting=false;
  #if CA
  /** Collision avoidance **/
  rnd_counter = 0;
  /** **/
  #endif
  //Notify the APIs that no further transmission should be taken care of
  PRINTF("Ending TX with mode %d \n",*mode);
  if(*mode==ND_BURST){
    
    // Enter the first RX slot
    start_rx_slot(NULL,mode);
  }else{
    if(slots>0){// It's not the last slot, so a new one needs to be initiated
      start_tx_slot(NULL,mode);
    }else{// It is the last slot, the epoch needs to end
      end_epoch(epoch_number,tot_neigh);
      //Begin new epoch by entering RX again
      start_rx_slot(NULL,mode);
    }
  }
}

/**
 * \brief       Callback function for the initiation of a RX slot
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode  The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void start_rx_slot(struct rtimer *t, uint8_t *mode){
  PRINTF("Start RX with mode %d\n",*mode);
  if(*mode==ND_BURST){
    //Schedule the end of the reception
    rtimer_set(&timer_turn_off_radio,RTIMER_NOW()+RX_DURATION,0,(rtimer_callback_t)stop_listen,mode);  
  }else{
    //Set the end of the RX window
    rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+SLOT_DURATION,0,(rtimer_callback_t)end_rx_slot,mode);
  }
  // Turn the radio on so that packets can be received
  NETSTACK_RADIO.on();
}

/**
 * \brief      Callback function to switch off the radio
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode  The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void stop_listen(struct rtimer *t, uint8_t *mode){
  //Schedule the end of the RX slot
  rtimer_set(&timer_end_rx_slot,RTIMER_NOW()+(SLOT_DURATION-RX_DURATION),0,(rtimer_callback_t)end_rx_slot,mode);
  //Turn off the radio
  NETSTACK_RADIO.off();
}

/**
 * \brief      Callback function for the termination of a RX slot
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER
 */
void end_rx_slot(struct rtimer *t, uint8_t *mode){
  PRINTF("End rx with mode %d\n",*mode);
  //Decrease the slot counter to keep track of the epoch
  slots--;
  if(*mode==ND_BURST){
    if(slots>0){//The epoch is not terminated yet, thus a new RX slot is initiated
      start_rx_slot(NULL,mode);
    }else{ //The epoch has ended
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


#if CA
 /***** COLLISION AVOIDANCE ******/

/**
 * \brief      Callback function to send a beacon for Neighbour Discovery with the node_id
 * \param t     rTimer struct automatically passed when callback is called
 * \param data Struct containing both the mode and the random timing used for the TX
 */
void send_beacon_random(struct rtimer *t, bcin *data){
  if (*(data->mode)==ND_BURST){
    if(t!=NULL){ //It means that it is at least the second beacon call
      // Decrease the remaing slot by the BEACON INTERVAL + the random time used last
      PRINTF("Remaining slots: %d minus",remaining_tx_slot);
      remaining_tx_slot -= (BEACON_INTERVAL + data->random);
      PRINTF("%d",BEACON_INTERVAL + data->random);
    }
    //Pick the next random number
    data->random = rnd_list[rnd_counter++];
    if(remaining_tx_slot >= BEACON_INTERVAL+data->random){ //Still need to send beacon,
      PRINTF("Remaining time: %d\n",remaining_tx_slot);
      
      //PRogram the next beacon using also the random number to randomize.
      rtimer_set(&timer_next_beacon,RTIMER_NOW()+BEACON_INTERVAL+data->random,0,(rtimer_callback_t)send_beacon_random,data);
    }else{ //Not enough time for a new beacon
      //Schedule the end of the TX slot
      PRINTF("Not enough time: %d\n",remaining_tx_slot);
      rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,0,(rtimer_callback_t)end_tx_slot,data->mode);
    }
  }else{
    //Schedule the end of the TX slot
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,0,(rtimer_callback_t)end_tx_slot,data->mode);
  }
  //Send the beacon after having scheduled the rest of the work to improve efficiency
  NETSTACK_RADIO.send(&payload, sizeof(payload));
  //NETSTACK_RADIO.off();
}
#else
/**
 * \brief      Callback function to send a beacon for Neighbour Discovery with the node_id
 * \param t     rTimer struct automatically passed when callback is called
 * \param mode The mode in which the protocol is operating: either ND_BURST or ND_SCATTER 
 */
void send_beacon(struct rtimer *t, uint8_t *mode){
  PRINTF("Beacon Mode: %d",*mode);
  if (*mode==ND_BURST){
    if(t!=NULL){//It means that it is at least the second beacon call
      //Decrease the time to keep track of the transmission
      remaining_tx_slot -= BEACON_INTERVAL;
    }
    if(remaining_tx_slot >= BEACON_INTERVAL){//Still need to send beacon,
      rtimer_set(&timer_next_beacon,RTIMER_NOW()+BEACON_INTERVAL,0,(rtimer_callback_t)send_beacon,mode);
    }else{
      //Schedule the end of the TX slot
      rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,0,(rtimer_callback_t)end_tx_slot,mode);
    }
  }else{
    //Schedule the end of the TX slot
    rtimer_set(&timer_end_tx_slot,RTIMER_NOW()+remaining_tx_slot,0,(rtimer_callback_t)end_tx_slot,mode);
  }
  //Send the beacon
  NETSTACK_RADIO.send(&payload, sizeof(payload));
}
#endif


