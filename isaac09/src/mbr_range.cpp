#include "mbr_range.h"

mbr_range::mbr_range(mbr *mbrs, uint size, uint type) {
  interval *tmpXInts = (interval*)malloc(size*sizeof(interval));
  interval *tmpYInts = (interval*)malloc(size*sizeof(interval));
  
  for(int i = 0; i < size; i++) {
	mbr cMBR = mbrs[i];
	tmpXInts[i] = interval(cMBR.getId(), cMBR.getMinX(), cMBR.getMaxX());
	tmpYInts[i] = interval(cMBR.getId(), cMBR.getMinY(), cMBR.getMaxY());
  }

  switch(type) {
	case NO_COMPACT:
	  x_intervals = new isaac_interval_stabbing(tmpXInts, size);
	  y_intervals = new isaac_interval_stabbing(tmpYInts, size);
	  break;
	case COMPACT:
	  x_intervals = new compact_interval_stabbing(tmpXInts, size);
	  y_intervals = new compact_interval_stabbing(tmpYInts, size);
  }
  
  free(tmpXInts);
  free(tmpYInts);
  
  intersectorSize = size/W+1;
  intersector = new uint[intersectorSize];
}

mbr_range::~mbr_range() {
  delete []intersector;
  delete x_intervals;
  delete y_intervals;
}

inline void mbr_range::cleanIntersector() {
  for(int i = 0; i < intersectorSize; i++) {
	intersector[i] = 0;
  }
}
int mbr_range::query(mbr *queryMBR, long *result, uint *resultSize) {
  cleanIntersector();
  long id = queryMBR->getId();
  double minx = queryMBR->getMinX();
  double maxx = queryMBR->getMaxX();
  double miny = queryMBR->getMinY();
  double maxy = queryMBR->getMaxY();
  interval x_interval = interval(id, minx, maxx);
  interval y_interval = interval(id, miny, maxy);
  
  //x_intervals->query(&x_interval, result, resultSize, intersector, QUERY_TYPE_MARK);
  //y_intervals->query(&y_interval, result, resultSize, intersector, QUERY_TYPE_CHECK_REPORT);
  x_intervals->queryMark(&x_interval, result, resultSize, intersector);
  y_intervals->queryReport(&y_interval, result, resultSize, intersector);
}
