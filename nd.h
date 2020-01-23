#include <stdbool.h>
#include "sys/rtimer.h"
/*---------------------------------------------------------------------------*/
#define ND_BURST 1
#define ND_SCATTER 2
/*---------------------------------------------------------------------------*/
#define EPOCH_INTERVAL_RT (RTIMER_SECOND)
/*---------------------------------------------------------------------------*/
#define MAX_NBR 64 /* Maximum number of neighbors */
/*---------------------------------------------------------------------------*/
void nd_recv(void); /* Called by lower layers when a message is received */
/*---------------------------------------------------------------------------*/
#define TOTAL_SLOTS 10
#define EPOCH_DURATION RTIMER_SECOND*20
#define SLOT_DURATION EPOCH_DURATION/TOTAL_SLOTS
#define RX_DURATION SLOT_DURATION/5


/* ND callbacks:
 * 	nd_new_nbr: inform the application when a new neighbor is discovered
 *	nd_epoch_end: report to the application the number of neighbors discovered
 *				  at the end of the epoch
 */
struct nd_callbacks {
  void (* nd_new_nbr)(uint16_t epoch, uint8_t nbr_id);
  void (* nd_epoch_end)(uint16_t epoch, uint8_t num_nbr);
};
/*---------------------------------------------------------------------------*/
/* Start selected ND primitive (ND_BURST or ND_SCATTER) */
void nd_start(uint8_t mode, const struct nd_callbacks *cb);
/*---------------------------------------------------------------------------*/
/* End the current epoch */
void end_epoch(uint16_t epoch, uint8_t num_nbr);
/*---------------------------------------------------------------------------*/
/* notify application of the new neighbour discovery */
void new_discovery(uint16_t epoch, uint8_t nbr_id);
/*---------------------------------------------------------------------------*/
/* Start the TX slot */
void start_tx_slot(struct rtimer *t, uint8_t *mode);
/*---------------------------------------------------------------------------*/
/* End the TX slot  */
void end_tx_slot(struct rtimer *t, uint8_t *mode);
/*---------------------------------------------------------------------------*/
/* Start the RX slot */
void start_rx_slot(struct rtimer *t, uint8_t *mode);
/*---------------------------------------------------------------------------*/
/* End the RX slot */
void end_rx_slot(struct rtimer *t, uint8_t *mode);
/*---------------------------------------------------------------------------*/
/* turn off the radio */
void stop_listen(struct rtimer *t, void *ptr);
/*---------------------------------------------------------------------------*/
/* Send a discovery beacon */
void send_beacon();
/*---------------------------------------------------------------------------*/
/* Check array for duplicates */
bool contains(unsigned short array[],uint8_t * toBeSearched);
/*---------------------------------------------------------------------------*/