#ifndef MBR_RANGE_H
#define	MBR_RANGE_H

#include "interval_stabbing.h"
#include "mbr.h"

#define COMPACT 1
#define NO_COMPACT 2

class mbr_range {
  public:
	mbr_range(mbr *mbrs, uint size, uint type);
	~mbr_range();
	int query(mbr *queryMBR, long *result, uint *resultSize);
	
  private:
	interval_stabbing *x_intervals;
	interval_stabbing *y_intervals;
	uint *intersector;
	uint intersectorSize;
	
	void cleanIntersector();
};

#endif
