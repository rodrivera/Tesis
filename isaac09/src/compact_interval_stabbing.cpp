#include <stack>

#include "compact_interval_stabbing.h"

compact_interval_stabbing::compact_interval_stabbing(interval *intervals, uint size)
  : interval_stabbing(size+1) {
  //+1 because we use a fictitious interval to generalize the transformation of the
  //queries to the rank space

  ids = new long[size];
  maxCoordPos = new uint[size];
  
  coord_interval *coordInts = new coord_interval[2 * size];
  
  for(int i = 0; i < size; i++) {
	coord_interval ci = {intervals[i].getMin(), true, &intervals[i]};
	coord_interval ci1 = {intervals[i].getMax(), false, &intervals[i]};
	coordInts[2*i] = ci;
	coordInts[2*i+1] = ci1;
  }
  
  qsort(coordInts, 2*size, sizeof(coord_interval), cmpCoordInterval);
  
  uint mmc_size = (2*n)/W+1;
  uint *minMaxCoordsBitmap = new uint[mmc_size];
  for(int i = 0; i < mmc_size; i++) {
	minMaxCoordsBitmap[i] = 0;
  }
  
  uint idsSize = 0;
  uint auxTreeBitmapSize = 2*n/W+1;
  uint auxTreeBitmap[auxTreeBitmapSize];
  uint auxTreeBitmapCPos = 0;
  for(int i = 0; i < auxTreeBitmapSize; i++) {
	auxTreeBitmap[i] = 0;
  }
  bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 0);//tree root
  stack<double> auxStack;
  uint minCoordsRank = 0;
  for(int i = 0; i < 2*size; i++) {
	//cout << coordInts[i].coord << " (" << coordInts[i].intervalPtr->getId() << ") ";
	coords[i] = coordInts[i].coord;
	if(coordInts[i].min) {
	  bitset(minMaxCoordsBitmap, i);
  	  ids[idsSize++] = coordInts[i].intervalPtr->getId();
	  while(!auxStack.empty() && auxStack.top() < coordInts[i].intervalPtr->getMax()) {
		bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 1);
		auxStack.pop();
	  }
	  bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 0);
	  auxStack.push(coordInts[i].intervalPtr->getMax());
	  coordInts[i].intervalPtr->setMinRank(minCoordsRank++);
	  if(coordInts[i].intervalPtr->getMin() == coordInts[i].intervalPtr->getMax()) {
		maxCoordPos[coordInts[i].intervalPtr->getMinRank()] = i;
	  } 
	} else {
	  if(coordInts[i].intervalPtr->getMin() != coordInts[i].intervalPtr->getMax()) {
		maxCoordPos[coordInts[i].intervalPtr->getMinRank()] = i;
	  }
	  while(!auxStack.empty() && auxStack.top() <= coordInts[i].intervalPtr->getMax()) {
		bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 1);
		auxStack.pop();
	  }
	}
  }
  //fictitious interval begin
  coords[2*size] = coords[2*size-1] + 1;
  coords[2*size+1] = coords[2*size] + 1;
  bitset(minMaxCoordsBitmap, 2*size);
  bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 0);
  bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 1);
  //fictitious interval end
  bitput(auxTreeBitmap, auxTreeBitmapCPos++, 1, 1);//close tree root

  /*
  for(int i = 0; i < 2*(n+1); i++) {
	if(bitget1(auxTreeBitmap,i)) {cout << "1";}
	else {cout << "0";}
  }
  cout << "\n";
  */
  
  delete []coordInts;
  
  tree = new tree_ff(auxTreeBitmap, 2*(n+1), OPT_FAST_PREORDER_SELECT);
  
  mmc_rank = new static_bitsequence_brw32(minMaxCoordsBitmap, 2*n, 20);
  delete []minMaxCoordsBitmap;
  
  /* Borrar!!! Entrenando con árboles
  bitput(auxTreeBitmap,0,1,0);bitput(auxTreeBitmap,1,1,0);bitput(auxTreeBitmap,2,1,0);bitput(auxTreeBitmap,3,1,1);
  bitput(auxTreeBitmap,4,1,0);bitput(auxTreeBitmap,5,1,1);bitput(auxTreeBitmap,6,1,1);bitput(auxTreeBitmap,7,1,0);
  bitput(auxTreeBitmap,8,1,1);bitput(auxTreeBitmap,9,1,1);
  tree = new tree_ff(auxTreeBitmap, 10, OPT_FAST_PREORDER_SELECT);
  cout << "P_R(7) = " << tree->Preorder_Rank(7) << "\n";cout << "P_S(3) = " << tree->Preorden_Select(3) << "\n";
  cout << "IL(1) = " << tree->Isleaf(1) << "\n";cout << "IL(2) = " << tree->Isleaf(2) << "\n";
  cout << "Parent(7) = " << tree->Parent(7) << "\n";cout << "Parent(2) = " << tree->Parent(2) << "\n";
  cout << "N_S(2) = " << tree->Next_Sibling(2) << "\n";cout << "N_S(7) = " << (int)tree->Next_Sibling(7) << "\n";
  */
}

compact_interval_stabbing::~compact_interval_stabbing() {
  delete tree;
  delete mmc_rank;
  delete []ids;
  delete []maxCoordPos;
}

int compact_interval_stabbing::query(const double queryPoint, long *result, uint *resultSize) {
  return -1;
}

int compact_interval_stabbing::queryMark(interval *queryInterval, long *result, uint *resultSize, uint *intersector) {
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  if(qiMax < coords[0] || qiMin > coords[2*n-3]) {return 0;}//2n-1 and 2n-2 correspond with the fictitious interval
	
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  int cNodePreorder;
  int cNodePos;
    
  cNodePreorder = mmc_rank->rank1(pos);
  if(pos < 2*n-2) {
	if(mmc_rank->access(pos)) {
	  bitset(intersector, ids[cNodePreorder-1]);
	} else {
	  cNodePreorder++;
	}
  }
  cNodePos = tree->Preorden_Select(cNodePreorder+1);
  nodesDeque[ndHead++] = cNodePos;
  
  int tmpCNodePos = tree->Parent(cNodePos);
  while(tmpCNodePos != 0) {
	cNodePreorder = tree->Preorder_Rank(tmpCNodePos);
	bitset(intersector, ids[cNodePreorder-2]);//-2 because the root is not represented
	nodesDeque[ndHead++] = tmpCNodePos;
	tmpCNodePos = tree->Parent(tmpCNodePos);
  }
  while(ndHead > 0) {
	cNodePos = tree->Prev_Sibling(nodesDeque[--ndHead]);
	while(cNodePos != -1) {
	  cNodePreorder = tree->Preorder_Rank(cNodePos);
	  if(coords[maxCoordPos[cNodePreorder-2]] < qiMin) {
		break;
	  }
	  bitset(intersector, ids[cNodePreorder-2]);
	  nodesDeque[ndHead++] = cNodePos;
	  cNodePos = tree->Last_Child(cNodePos);
	}
  }

  return 0;
}

int compact_interval_stabbing::queryReport(interval *queryInterval, long *result, uint *resultSize, uint *intersector) {
  *resultSize = 0;
  ndHead = 0;
  int pos;
  double qiMin = queryInterval->getMin();
  double qiMax = queryInterval->getMax();
  if(qiMax < coords[0] || qiMin > coords[2*n-3]) {return 0;}//2n-1 and 2n-2 correspond with the fictitious interval
	
  bool found = rightBinarySearch(qiMax, coords, 2*n, &pos);
  int cNodePreorder;
  int cNodePos;
  cNodePreorder = mmc_rank->rank1(pos);
  if(pos < 2*n-2) {
	if(mmc_rank->access(pos)) {
		if(bitget(intersector, ids[cNodePreorder-1])) {
		  result[(*resultSize)++] = ids[cNodePreorder - 1];
		}
	} else {
	  cNodePreorder++;
	}
  }
  cNodePos = tree->Preorden_Select(cNodePreorder+1);
  nodesDeque[ndHead++] = cNodePos;

  int tmpCNodePos = tree->Parent(cNodePos);
  while(tmpCNodePos != 0) {
	cNodePreorder = tree->Preorder_Rank(tmpCNodePos);
	if(bitget(intersector, ids[cNodePreorder-2])) {
	  result[(*resultSize)++] = ids[cNodePreorder - 2];
	}
	nodesDeque[ndHead++] = tmpCNodePos;
	tmpCNodePos = tree->Parent(tmpCNodePos);
  }
  while(ndHead > 0) {
	cNodePos = tree->Prev_Sibling(nodesDeque[--ndHead]);
	while(cNodePos != -1) {
	  cNodePreorder = tree->Preorder_Rank(cNodePos);
	  if(coords[maxCoordPos[cNodePreorder-2]] < qiMin) {
		break;
	  }
	  if(bitget(intersector, ids[cNodePreorder-2])) {
		result[(*resultSize)++] = ids[cNodePreorder - 2];
	  }
	  nodesDeque[ndHead++] = cNodePos;
	  cNodePos = tree->Last_Child(cNodePos);
	}
  }

  return 0;
}
