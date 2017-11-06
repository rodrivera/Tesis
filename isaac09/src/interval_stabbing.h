#ifndef INTERVAL_STABBING_H
#define	INTERVAL_STABBING_H

#include "basics.h"
#include "interval.h"

#define QUERY_TYPE_REPORT 1
#define QUERY_TYPE_MARK 2
#define QUERY_TYPE_CHECK_REPORT 3

class interval_stabbing {
  public:
	virtual int query(const double queryPoint, long *result, uint *resultSize) = 0;
	virtual int queryMark(interval *queryInterval, long *result, uint *resultSize, uint *intersector) = 0;
	virtual int queryReport(interval *queryInterval, long *result, uint *resultSize, uint *intersector) = 0;
//	virtual int query(interval *queryInterval, long *result, uint *resultSize, uint *intersector, uint type) = 0;
//	virtual int query(interval *queryInterval, long *result, uint *resultSize, uint *intersector, 
//				void (*function) (uint *bitmap, uint pos, long id, long *result, uint *resultSize)) = 0;
				
	virtual ~interval_stabbing();

  protected:
	uint n;
	double *coords;

	interval_stabbing(uint size):n(size){
	  coords = new double[2 * n];
	};
		
};

//sublclasses
#include "compact_interval_stabbing.h"
#include "isaac_interval_stabbing.h"

//auxiliary
struct coord_interval {
  double coord;
  bool min;
  interval *intervalPtr;
};
int cmpCoordInterval(const void *int1, const void *int2);

int rightBinarySearch(const double target, const double *values, const uint size, int *pos);
#endif
