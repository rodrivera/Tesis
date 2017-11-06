#ifndef BALANCED_PARENTHESES_H
#define	BALANCED_PARENTHESES_H

#include "basics.h"
#include "static_bitsequence.h"

#define RP 2
#define HB 3
#define RMM 4

class balanced_parentheses{
	public:
		virtual ~balanced_parentheses() {};
		virtual uint excess(int v) = 0;
		virtual uint close(int v) = 0;
		virtual uint open(int v) = 0;
		virtual uint enclose(int v) = 0;
		virtual uint size() = 0;
		virtual int save(FILE *fp) = 0;
		static balanced_parentheses * load(FILE *fp);
};

#include "bp_rp.h"
#include "bp_hb.h"
#include "bp_rmm.h"

#endif

