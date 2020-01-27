#include "utils.h"
#include <stdbool.h>

/**
 * \brief       Check wheter the neighbour array has already the selected entry
 * \param array         Array in which the element should be looked for. It should contain all of the discovered neighbours
 * \param toBeSearched  Element to be searched
 * \return     Boolean value depending on the outcome of the search.
 */
bool contains(uint8_t array[],uint8_t lenght, uint8_t toBeSearched){
  bool found = false;
  uint8_t i = 0;
  while(i < lenght){
    if(array[i] == toBeSearched){
      found = true;
      break;
    }
    i++;
  }
  return found;
}

