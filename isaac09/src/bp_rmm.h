#ifndef BP_RMM_H
#define	BP_RMM_H

#include "balanced_parentheses.h"
#include "bp.h"

class bp_rmm: public balanced_parentheses{
	private:
		bp_rmm();
	public:
		bp b1;


		/*n:size of the bitmap
		 * pb == uint  -> B is the bitmap
		 * opt: options for the construction :  OPT_LEAF | 
		 * 																		OPT_DEGREE | 
		 * 																		OPT_INORDER | 
		 * 																		OPT_FAST_PREORDER_SELECT |
		 * 																		OPT_FAST_LEAF_SELECT |
		 * 																		OPT_FAST_INORDER_SELECT |
		 * 																		OPT_FAST_POSTORDER_SELECT
		 * */
		bp_rmm(int n, pb *B, int opt);
		virtual ~bp_rmm();
		virtual uint excess(int v);
		virtual uint close(int v);
		virtual uint open(int v);
		virtual uint enclose(int v);
		virtual uint size();
		virtual int save(FILE *fp);
		static bp_rmm * load(FILE *fp);
};

#endif

