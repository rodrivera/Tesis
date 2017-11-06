#ifndef INTERVAL_STABBING_QUERY_FUNCTIONS_H
#define	INTERVAL_STABBING_QUERY_FUNCTIONS_H

#include "basics.h"

//query functions
void functionDirectReport(uint *bitmap, uint pos, long id, long *result, uint *resultSize) {
  result[(*resultSize)++] = id;
} 

void functionTurnBitOn(uint *bitmap, uint pos, long id, long *result, uint *resultSize) {
  bitset(bitmap,pos);
} 

void functionReportIfBitOn(uint *bitmap, uint pos, long id, long *result, uint *resultSize) {
  if(bitget(bitmap,pos)) {
	result[(*resultSize)++] = id;
  }
} 

#endif