#ifndef FF_DFUDS
#define FF_DFUDS

#include "bp.h"

#define NULLT -1
#define ROOT 1

#ifndef uint
typedef unsigned int uint;
#endif
typedef unsigned char symbol_T;
typedef unsigned long treeNode;


class ff_dfuds {
   symbol_T *label;
   uint Nnodes;
   treeNode *boost;
public:
	 bp topology;
    ff_dfuds(uint *par, uint n, symbol_T *_label, int opt);
    ff_dfuds() {;};
    ~ff_dfuds();
    uint getNnodes() {return Nnodes;};
    treeNode Root_Node();
    treeNode Parent_Fast(treeNode x);
    treeNode Parent_Sequential(treeNode x);
    treeNode Child(treeNode x, int i);
    treeNode Level_Ancestor(treeNode x, int delta);  // brute force
    uint     Depth(treeNode x); // brute force
    uint     Preorder_Rank(treeNode x);
    treeNode Preorder_Select(uint p);
    uint     Inspect(uint p);
    bool     Isleaf(treeNode x);
    uint     Subtree_Size(treeNode x);
    treeNode First_Child(treeNode x);
    treeNode Next_Sibling(treeNode x);
    treeNode Prev_Sibling(treeNode x);
    bool     Is_Ancestor(treeNode x, treeNode y);
    uint     Distance(treeNode x, treeNode y);  // brute force
    uint     Degree_Sequential(treeNode x);
    uint     Degree_Fast(treeNode x);
    uint     Child_Rank(treeNode x);
    symbol_T Get_Label(treeNode x);
    treeNode Labeled_Child(treeNode x, symbol_T s);
    uint size();
};

#endif
