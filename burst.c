#include "burst.h"
#include "node-id.h"

struct beacon tx_beacon = {
  .node_id = node_id;
};
  
void send_beacon(){
    
    int (* send)(const void *payload, unsigned short payload_len);
}