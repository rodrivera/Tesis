#ifndef COMPACT_INTERVAL_STABBING_H
#define	COMPACT_INTERVAL_STABBING_H

#include "interval_stabbing.h"
#include "static_bitsequence_brw32.h"
#include "tree_ff.h"

#define STACK_SIZE 2000000

class compact_interval_stabbing : public interval_stabbing {
  public:
	compact_interval_stabbing(interval *intervals, uint size);
	~compact_interval_stabbing();
	virtual int query(const double queryPoint, long *result, uint *resultSize);
	virtual int queryMark(interval *queryInterval, long *result, uint *resultSize, uint *intersector);
	virtual int queryReport(interval *queryInterval, long *result, uint *resultSize, uint *intersector);

  private:
	long *ids;
	static_bitsequence *mmc_rank;
	tree_ff *tree;
	uint *maxCoordPos;//position in coords of an interval (accessed in preorder)
	
	uint nodesDeque[STACK_SIZE];
	uint ndHead;

};

#endif
