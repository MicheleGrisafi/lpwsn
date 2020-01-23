#include <stdbool.h>

void log_nd(unsigned short node_id, uint8_t epoch, r_time timestamp);
void log_epoch(uint8_t epoch, r_time timestamp);
bool contains(unsigned short array[],uint8_t * toBeSearched);