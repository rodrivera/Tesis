#ifndef ISAAC_INTERVAL_STABBING_H
#define	ISAAC_INTERVAL_STABBING_H

#include "interval_stabbing.h"

#define STACK_SIZE 2000000

//auxiliary
struct isaac_node {
  long id;
  double *coord_min;
  double *coord;
  isaac_node *parent;
  isaac_node *leftsibling;
  isaac_node *rightchild;
};

class isaac_interval_stabbing : public interval_stabbing {
  public:
	isaac_interval_stabbing(interval *intervals, uint size);
	~isaac_interval_stabbing();
	virtual int query(const double queryPoint, long *result, uint *resultSize);
	virtual int queryMark(interval *queryInterval, long *result, uint *resultSize, uint *intersector);
	virtual int queryReport(interval *queryInterval, long *result, uint *resultSize, uint *intersector);
//	virtual int query(interval *queryInterval, long *result, uint *resultSize, uint *intersector, uint type);
//	virtual int query(interval *queryInterval, long *result, uint *resultSize, uint *intersector, 
//				void (*function) (uint *bitmap, uint pos, long id, long *result, uint *resultSize));

  private:
	isaac_node *root;
	isaac_node **start;
	isaac_node **start_2;

	isaac_node *nodesDeque[STACK_SIZE];
	uint ndHead;
};

#endif
