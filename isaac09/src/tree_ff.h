#ifndef TREE_FF
#define TREE_FF

#include "balanced_parentheses.h"
class tree_ff{
	private:
	  uchar *label;
	  uint length;
	
	public:
		balanced_parentheses *topology;
		/*op== 0|
			* OPT_LEAF | 
			* OPT_DEGREE | 
			* OPT_INORDER | 
			* OPT_FAST_PREORDER_SELECT |
			* OPT_FAST_LEAF_SELECT |
			* OPT_FAST_INORDER_SELECT |
			* OPT_FAST_POSTORDER_SELECT
			*/
		tree_ff(uint *bitarray, uint ba_size, int opt);
		tree_ff(uint *bitarray, uint ba_size, int opt,  uchar *labels);
		
		~tree_ff();
		uint Root_Node();
		uint Parent(uint x);
		uint Child(uint x, int i);
		uint Level_Ancestor(uint x, int delta);
		uint Depth(uint x);
		uint Preorder_Rank(uint x);
		uint Preorden_Select(uint x);
		uint Postorder_Rank(uint x);
		uint Inspect(uint p);
		bool Isleaf(uint x);
		uint Subtree_Size(uint x);
		uint First_Child(uint x);
		uint Last_Child(uint x);
		uint Next_Sibling(uint x);
		uint Prev_Sibling(uint x);
		uint Deepest_Node(uint x);
		uint Subtree_Height(uint x);
		bool Is_Ancestor(uint x, uint y);
		uint Distance(uint x, uint y);
		uint Lca(uint x, uint y);
		uint Level_Lefthmost(uint d);
		uint Level_Rigthmost(uint d);
		uint Degree(uint x);
		uint Child_rank(uint x);
		uchar Get_Label(uint x);
		uint Labeled_Child(uint x, uchar s);
		uint Labeled_Child_seq(uint x, uchar s);
		uint Labeled_Child_bin(uint x, uchar s);
		uint node_rank(uint x);
		uint node_select(uint x);
		uint size();
};

#endif



