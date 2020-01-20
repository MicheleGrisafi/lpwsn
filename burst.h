/*---------------------------------------------------------------------------*/
/* Burst implementation */
#define SLOT_DURATION (50*(RTIMER_SECOND/10))
#define EPOCH_DURATION 50*RTIMER_SECOND*20

struct beacon {
  unsigned short node_id;
};

void send_beacon();

void enter_reception();

