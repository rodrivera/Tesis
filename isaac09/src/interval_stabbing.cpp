#include "interval_stabbing.h"

interval_stabbing::~interval_stabbing() {
  delete []coords;
}

//auxiliary
int cmpCoordInterval(const void *int1, const void *int2) {
  coord_interval *ci1 = (coord_interval *) int1;
  coord_interval *ci2 = (coord_interval *) int2;
  double c1 = ci1->coord;
  double c2 = ci2->coord;
  double result = c1 - c2;
  if(result > 0) {
	return 1;
  } else if(result < 0) {
	return -1;
  } else {
	if(ci1->min) {
	  if(ci2->min) {
		if(ci1->intervalPtr->getMax() > ci2->intervalPtr->getMax()) {return -1;} 
		else {return 1;}
	  } else {return 1;}
	} else {
	  if(ci2->min) {return -1;} 
	  else {
		if(ci1->intervalPtr->getMin() > ci2->intervalPtr->getMin()) {return 1;} 
		else {return -1;}
	  }
	}
  }
}

int rightBinarySearch(const double target, const double *values, const uint size, int *pos) {
	register int lowerLimit = 0;
	register int upperLimit = size - 1;
	register int pivotPos;
	double pivotValue; 
	while(lowerLimit <= upperLimit) {
		pivotPos = (lowerLimit + upperLimit) / 2; 
		//cambiar esto por pivotPos = lowerLimit + (upperLimit - lowerLimit)/2 
		//si los arrays son muy grandes por problema de overflow en la suma
		pivotValue = values[pivotPos];	
		if(pivotValue > target) {
			upperLimit = pivotPos - 1;
		} else {
			lowerLimit = pivotPos + 1;  
		}
	}
	
	*pos = upperLimit;
	return (upperLimit >= 0) && (values[upperLimit] == target);
}
