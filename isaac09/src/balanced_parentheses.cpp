#include "balanced_parentheses.h"

balanced_parentheses * balanced_parentheses::load(FILE * fp) {
	uint r;
	fread(&r,sizeof(uint),1,fp);
	cout << "Loading "<< r << endl;
	switch(r) {
		case RP: return bp_rp::load(fp);
		case HB: return bp_hb::load(fp);
		case RMM: return bp_rmm::load(fp);
	}				
	return NULL;
}




