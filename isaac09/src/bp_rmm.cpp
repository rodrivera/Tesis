#include "bp_rmm.h"

bp_rmm::bp_rmm(){

}

bp_rmm::bp_rmm(int n, pb *B, int opt){
	pb *B_aux = new pb[(n+D-1)/D];
	for(int i=0; i<n;i++)
		setbit(B_aux,i,CP);
	for(int i=0; i<n;i++){
		if(!bitget1(B,i))
			setbit(B_aux,i,OP);
	}
	bp_construct(&b1, n, B_aux,opt);
}

bp_rmm::~bp_rmm(){
	/*destructor???*/
}

uint bp_rmm::excess(int v){
	return 2*rank_open(&b1,v) - (v+1);
}

uint bp_rmm::close(int v){
	return find_close(&b1, v);
}

uint bp_rmm::open(int v){
	return find_open(&b1, v);
}

uint bp_rmm::enclose(int v){
	return find_enclose(&b1,v);
}

uint bp_rmm::size(){
	uint mem = b1.idx_size;
	return mem;
}

int bp_rmm::save(FILE *fp){
	return 0;
}

bp_rmm * bp_rmm::load(FILE *fp){
	return NULL;
}

