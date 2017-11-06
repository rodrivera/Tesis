#include <stack>
#include <deque>

#include "isaac_interval_stabbing.h"

isaac_interval_stabbing::isaac_interval_stabbing(interval *intervals, uint size)
  : interval_stabbing(size) {
  coord_interval *coordInts = new coord_interval[2 * n];
  
  for(int i = 0; i < n; i++) {
	coord_interval ci = {intervals[i].getMin(), true, &intervals[i]};
	coord_interval ci1 = {intervals[i].getMax(), false, &intervals[i]};
	coordInts[2*i] = ci;
	coordInts[2*i+1] = ci1;
  }
  
  qsort(coordInts, 2*n, sizeof(coord_interval), cmpCoordInterval);
    
  for(int i = 0; i < 2*n; i++) {
	//cout << coordInts[i].coord << " (" << coordInts[i].intervalPtr->getId() << ") ";
	coords[i] = coordInts[i].coord;
	if(coordInts[i].min) {
	  coordInts[i].intervalPtr->setMinRank(i);
	} else {
	  coordInts[i].intervalPtr->setMaxRank(i);
	}
  }
  
  //create the tree and start
  start = new isaac_node*[4*n];
  start_2 = new isaac_node*[2*n];
  root = new isaac_node;
  root->parent = root->rightchild = root->leftsibling = NULL;
  root->coord = NULL;
  root->id = -1;
  stack<isaac_node*> nodesStack;
  stack<interval*> intervalsStack;
  isaac_node *start2Candidate;
  nodesStack.push(root);
  isaac_node *tmpNode, *tmpParent;
  for(int i = 0; i < 2*n; i++) {
	if(coordInts[i].min) {
	  tmpNode = new isaac_node;
	  tmpNode->id = coordInts[i].intervalPtr->getId();
	  tmpNode->coord_min = &coords[coordInts[i].intervalPtr->getMinRank()];
	  tmpNode->coord = &coords[coordInts[i].intervalPtr->getMaxRank()];
	  tmpNode->rightchild = NULL;
	  while(!intervalsStack.empty() && intervalsStack.top()->getMaxRank() < i) {
		intervalsStack.pop();
		nodesStack.pop();
	  }
	  if(nodesStack.empty()) {
		tmpParent = root;
	  } else {
		tmpParent = nodesStack.top();
		while(tmpParent->coord != NULL && *tmpParent->coord < coordInts[i].intervalPtr->getMax()) {
		  tmpParent = tmpParent->parent;
		}
	  }
	  tmpNode->leftsibling = tmpParent->rightchild;
	  tmpNode->parent = tmpParent;
	  tmpParent->rightchild = tmpNode;
	  nodesStack.push(tmpNode);
	  intervalsStack.push(coordInts[i].intervalPtr);
	  start[2*i] = tmpNode;
	  start[2*i+1] = tmpNode;
	  start2Candidate = tmpNode;
	  //cout << tmpNode->id << " ->" << tmpParent->id  << "\n";
	} else {
	  start[2*i] = nodesStack.top();
	  while(!intervalsStack.empty() && intervalsStack.top()->getMaxRank() <= i) {
		nodesStack.pop();
		intervalsStack.pop();
	  }
	  start[2*i+1] = nodesStack.top();
	}
	start_2[i] = start2Candidate;
  }
  
  delete []coordInts;  
}

void recursive_free(isaac_node *node) {
  if(node->rightchild != NULL) {
	recursive_free(node->rightchild);
  }
  if(node->leftsibling != NULL) {
	recursive_free(node->leftsibling);
  }
  delete node;
}

isaac_interval_stabbing::~isaac_interval_stabbing() {
  delete []start;
  delete []start_2;
  recursive_free(root);
}

int isaac_interval_stabbing::query(const double queryPoint, long *result, uint *resultSize) {
  *resultSize = 0;
  int pos;
  bool found;
  found = rightBinarySearch(queryPoint, coords, 2*n, &pos);
  //cout << queryPoint << " " << found << " " << pos << "\n";
  
  pos = 2*pos;
  if(!found) {
	if(pos < 0) {return 0;}
	pos++;
  }
  
  isaac_node *cNode;
  deque<isaac_node*> nodesDeque;
  for(cNode = start[pos]; cNode->parent != NULL; cNode = cNode->parent) {
	nodesDeque.push_front(cNode);
  }
  while(!nodesDeque.empty()) {
	cNode = nodesDeque.back();
	nodesDeque.pop_back();
	result[(*resultSize)++] = cNode->id;
	
	cNode = cNode->leftsibling;
	while(cNode != NULL && *cNode->coord >= queryPoint) {
	  nodesDeque.push_back(cNode);
	  cNode = cNode->rightchild;
	}
  }
  
  return 0;
}

int isaac_interval_stabbing::queryMark(interval *queryInterval, long *result, uint *resultSize, uint *intersector) {
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  if(pos < 0) {return 0;}
  
  isaac_node *cNode = start_2[pos];
  if(*cNode->coord < qiMin) {
	found = rightBinarySearch(qiMin, coords, 2*n, &pos);
	pos *= 2;
	if(!found) {pos++;}
	cNode = start[pos];
  }

  if(cNode->id == -1) {return 0;}
  
  isaac_node *tmpNode = cNode->rightchild;
  while(tmpNode != NULL && *tmpNode->coord >= queryInterval->getMin() && *tmpNode->coord <= queryInterval->getMax()) {
	cNode = tmpNode;
	tmpNode = tmpNode->rightchild;
  }

  while(cNode->parent != NULL) {
	nodesDeque[ndHead++] = cNode;
	cNode = cNode->parent;
  }
  while(ndHead > 0) {
	cNode = nodesDeque[--ndHead];
	bitset(intersector, cNode->id);
	
	cNode = cNode->leftsibling;
	while(cNode != NULL && *cNode->coord >= qiMin) {
	  nodesDeque[ndHead++] = cNode;
	  cNode = cNode->rightchild;
	}
  }
  
  return 0;
}

int isaac_interval_stabbing::queryReport(interval *queryInterval, long *result, uint *resultSize, uint *intersector) {
  (*resultSize) = 0;
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  if(pos < 0) {return 0;}
  
  isaac_node *cNode = start_2[pos];
  if(*cNode->coord < qiMin) {
	found = rightBinarySearch(qiMin, coords, 2*n, &pos);
	pos *= 2;
	if(!found) {pos++;}
	cNode = start[pos];
  }
  if(cNode->id == -1) {return 0;}
  
  isaac_node *tmpNode = cNode->rightchild;
  while(tmpNode != NULL && *tmpNode->coord >= queryInterval->getMin() && *tmpNode->coord <= queryInterval->getMax()) {
	cNode = tmpNode;
	tmpNode = tmpNode->rightchild;
  }

  while(cNode->parent != NULL) {
	nodesDeque[ndHead++] = cNode;
	cNode = cNode->parent;
  }
  while(ndHead > 0) {
	cNode = nodesDeque[--ndHead];
	if(bitget(intersector, cNode->id)) {
	  result[(*resultSize)++] = cNode->id;
	}
	//function(intersector, cNode->id, cNode->id, result, resultSize); 
	
	cNode = cNode->leftsibling;
	while(cNode != NULL && *cNode->coord >= qiMin) {
	  nodesDeque[ndHead++] = cNode;
	  cNode = cNode->rightchild;
	}
  }
  
  return 0;
}

/*
int isaac_interval_stabbing::query(interval *queryInterval, long *result, uint *resultSize, uint *intersector, uint type) {
  *resultSize = 0;
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  pos = 2*pos;
  isaac_node *cNode = start[pos];

  if(pos > 4) {
	isaac_node *auxNode = start[pos-2];
	if(auxNode->parent == cNode && *auxNode->coord >= queryInterval->getMin()) {
	  cNode = auxNode;
	}
	auxNode = start[pos-4];
	if(auxNode->parent == cNode && *auxNode->coord >= queryInterval->getMin()) {
	  cNode = auxNode;
	}
  }
  
  if(!found) {
	if(pos < 0) {return 0;}
	if(*cNode->coord < qiMin) {cNode = start[pos+1];}
  }
  if(cNode->id == -1) {return 0;}
  
  isaac_node *tmpNode = cNode->rightchild;
  while(tmpNode != NULL && *tmpNode->coord >= queryInterval->getMin() && *tmpNode->coord <= queryInterval->getMax()) {
	cNode = tmpNode;
	tmpNode = tmpNode->rightchild;
  }

  while(cNode->parent != NULL) {
	nodesDeque[ndHead++] = cNode;
	cNode = cNode->parent;
  }
  while(ndHead > 0) {
	cNode = nodesDeque[--ndHead];
	switch(type) {
	  case QUERY_TYPE_REPORT:
		result[(*resultSize)++] = cNode->id;
		break;
	  case QUERY_TYPE_MARK:
		bitset(intersector, cNode->id);
		break;
	  case QUERY_TYPE_CHECK_REPORT:
		if(bitget(intersector, cNode->id))
		  result[(*resultSize)++] = cNode->id;
		break;
	}
	//function(intersector, cNode->id, cNode->id, result, resultSize); 
	
	cNode = cNode->leftsibling;
	while(cNode != NULL && *cNode->coord >= qiMin) {
	  nodesDeque[ndHead++] = cNode;
	  cNode = cNode->rightchild;
	}
  }
  
  return 0;
}
*/

/*
int isaac_interval_stabbing::query(interval *queryInterval, long *result, uint *resultSize, uint *intersector,
	  void (*function) (uint *bitmap, uint pos, long id, long *result, uint *resultSize)) {
  *resultSize = 0;
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  pos = 2*pos;
  isaac_node *cNode = start[pos];

  if(pos > 4) {
	isaac_node *auxNode = start[pos-2];
	if(auxNode->parent == cNode && *auxNode->coord >= queryInterval->getMin()) {
	  cNode = auxNode;
	}
	auxNode = start[pos-4];
	if(auxNode->parent == cNode && *auxNode->coord >= queryInterval->getMin()) {
	  cNode = auxNode;
	}
  }
  
  if(!found) {
	if(pos < 0) {return 0;}
	if(*cNode->coord < qiMin) {cNode = start[pos+1];}
  }
  if(cNode->id == -1) {return 0;}
  
  isaac_node *tmpNode = cNode->rightchild;
  while(tmpNode != NULL && *tmpNode->coord >= queryInterval->getMin() && *tmpNode->coord <= queryInterval->getMax()) {
	cNode = tmpNode;
	tmpNode = tmpNode->rightchild;
  }

  while(cNode->parent != NULL) {
	nodesDeque[ndHead++] = cNode;
	cNode = cNode->parent;
  }
  while(ndHead > 0) {
	cNode = nodesDeque[--ndHead];
	function(intersector, cNode->id, cNode->id, result, resultSize); 
	
	cNode = cNode->leftsibling;
	while(cNode != NULL && *cNode->coord >= qiMin) {
	  nodesDeque[ndHead++] = cNode;
	  cNode = cNode->rightchild;
	}
  }
  
  return 0;
}
*/