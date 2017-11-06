#include "bp_hb.h"

bp_hb::bp_hb(){
	data = NULL;
	bdata = NULL;
	n = 0;                  
	sbits = 0;
	b = 0;
	sftable = NULL;
	sbtable = NULL;
	bftable = NULL;
	bbtable = NULL;
	open_sbtable = NULL;
	open_bbtable = NULL;
}


bp_hb::bp_hb(uint *string, uint length, uint block_size, bool bwd){
	uint nbits, s, near, far, pnear, pfar;
	data = string;
	n = length; 
	b = block_size;
	bdata = new static_bitsequence_brw32(string, n, 20); 
	nbits = bits(n-1);
	s = nbits*b;
	sbits = bits(s-1);
	cout << "near limit: " << (1<<sbits) << endl;
	s = 1 << sbits; // to take the most advantage of what we can represent
	near = far = pnear = pfar = 0;
	calcsizes(~0,0,&near,&far,&pnear,&pfar);
	/*create structures for findclose()*/
	sftable = createHash(far,nbits, 1.6);
	bftable = createHash(near,sbits, 1.6);
	if(bwd){
		/*create structures for findopen()*/
		open_sbtable = createHash(far,nbits, 1.6);
		open_bbtable = createHash(near,sbits, 1.6);
		/*create structures for enclose()*/
		sbtable = createHash(pfar,nbits, 1.8);
		bbtable = createHash(pnear,sbits, 1.8);
	}
	else{ 
		sbtable = bbtable = open_sbtable = open_bbtable = NULL;
	}
	filltables(~0,0,bwd);
	/*compute tables*/
	for (int i=0;i<256;i++) {
			fcompchar(i,FwdPos[i],Excess+i);
			bcompchar(i,BwdPos[i]);
	}
}


/* creates a parentheses structure from a bitstring, which is shared
 * n is the total number of parentheses, opening + closing
 * */
uint bp_hb::calcsizes(uint posparent, uint posopen, uint *near, uint *far, uint *pnear, uint *pfar){
	uint posclose,newpos;
	if ((posopen == n) || bitget1(data,posopen))
		return posopen; // no more trees
	newpos = posopen;
	do{ 
		posclose = newpos+1;
		newpos = calcsizes (posopen,posclose,near,far,pnear,pfar);
	}
	while (newpos != posclose);
	
	if ((posclose < n) && (posclose-posopen > b)){ // exists and not small
		if ((int)(posclose-posopen) < (1<<sbits)) 
			(*near)++; // near pointer
		else 
			(*far)++;
	}
	if ((posopen > 0) && (posopen-posparent > b)){ // exists and not small
		if ((int)(posopen-posparent) < (1<<sbits)) 
			(*pnear)++; // near pointer
		else 
			(*pfar)++;
	}
	return posclose;
}

uint bp_hb::filltables(uint posparent, uint posopen, bool bwd){
	uint posclose,newpos;
	if ((posopen == n) || bitget1(data,posopen))
		return posopen; // no more trees
	newpos = posopen;
	do{ 
		posclose = newpos+1;
		newpos = filltables(posopen,posclose,bwd);
	}
	while (newpos != posclose);
	if ((posclose < n) && (posclose-posopen > b)) { // exists and not small
		if ((int)(posclose-posopen) < (1<<sbits)) { // near pointers
			insertHash(bftable,posopen,posclose-posopen);
			if (open_bbtable)
				insertHash(open_bbtable,posclose,posclose-posopen);
		}
		else { // far pointers
			insertHash(sftable,posopen,posclose-posopen); 
			if (open_sbtable)
				insertHash(open_sbtable,posclose,posclose-posopen);
		}
	}
	if (bwd && (posopen > 0) && (posopen-posparent > b)) { //exists and not small
		if ((int)(posopen-posparent) < (1<<sbits)) // near pointer
			insertHash(bbtable,posopen,posopen-posparent);
		else // far pointers
			insertHash(sbtable,posopen,posopen-posparent);
	}
	return posclose;
}

void bp_hb::fcompchar(byte x, byte *pos, char *excess){
	int exc = 0;
	uint i;
	for(i=0;i<W/2;i++) 
		pos[i] = 0;
	for(i=0;i<8;i++){
		if (x & 1) { // closing
			exc--; 
			if ((exc < 0) && !pos[-exc-1]) 
				pos[-exc-1] = i+1;
		}
		else 
			exc++;
		x >>= 1;
	}
	*excess = exc;
}

void bp_hb::bcompchar (byte x, byte *pos){
	int exc = 0;
	uint i;
	for (i=0;i<W/2;i++) 
		pos[i] = 0;
	for (i=0;i<8;i++){
		if (x & 128) { // opening, will be used on complemented masks
			exc++; 
			if ((exc > 0) && !pos[exc-1]) 
				pos[exc-1] = i+1;
		}
		else 
			exc--;
		x <<= 1;
	}
}

bp_hb::~bp_hb(){
	delete bdata;
	destroyHash(sftable);
	if (sbtable) 
		destroyHash(sbtable);
	destroyHash(bftable);
	if (bbtable) 
		destroyHash(bbtable);
	destroyHash(open_sbtable);
	destroyHash(open_bbtable);
}

/* # open - # close at position i, not included*/
uint bp_hb::excess(int v){
	return v - 2*bdata->rank1(v)+1; 
}

/* the position of the closing parenthesis corresponding to (opening)
 * parenthesis at position i
 */
uint bp_hb::close(int v){
	if(v==0)
		return n-1;
	uint bitW;
	uint len,res,minres,exc;    
	uint len_aux;
	uint pos = v;
	int cont;
	byte _W1;
	handle h;
	uint myexcess;
	/*first see if it is at small distance*/
	len = b; 
	if (v+len >= n) 
		len = n-v-1;
	len_aux = len;
	exc = 0; 
	len = 0;
	while((int)len_aux > 0){
		if((int)len_aux < 32)
			bitW = bitget_go(data, pos+1,len_aux);
		else
			bitW = bitget_go(data, pos+1, 32);
		cont = 0 ;
		while(bitW && (exc < b/2)) {
			/*either we shift it all or it only opens parentheses or too
			 open parentheses*/
			_W1 = bitW & 255;
			if(exc < W/2){
				if ((int)(res = FwdPos[_W1][exc])) 
					return v+len+res;
			}
			bitW >>= 8; 
			exc += Excess[_W1];
			len += 8;
			cont ++;
		}
		if(cont != 4){
			_W1 = 0;
			while(cont!=4){
				exc += Excess[_W1];
				len +=8;
				cont++;
			}
		}
		pos +=32;
		len_aux = len_aux-32;
	}
	/* ok, it's not a small distance, try with hashing btable*/
	minres = 0;
	myexcess = excess(v-1);
	res = searchHash(bftable,v,&h);
	while (res) {
		if (!minres || (res < minres)) 
			if ((v+res+1 < n) && (excess(v+res) == myexcess)) 
				minres = res;
		res = nextHash(bftable,&h);
	}
	if (minres) 
		return v+minres;
	/* finally, it has to be a far pointer*/
	res = searchHash(sftable,v,&h);
	while (res) {
		if (!minres || (res < minres)){
			if ((v+res+1 < n) && (excess(v+res) == myexcess))
				minres = res;
		}
		res = nextHash(sftable,&h);
	}

	return v+minres; /*there should be one if the sequence is balanced!*/
}

uint bp_hb::open(int v){
	uint bitW;
	uint len,res,minres,exc;
	uint len_aux;
	uint pos = v;
	int cont;
	byte _W1;
	handle h;
	uint myexcess;
	/*first see if it is at small distance*/
	len = b; 
	if ((uint)v < len) 
		len = v-1;
	len_aux = len;
	exc = 0; 
	len = 0;
	while((int)len_aux > 0){
		if((int)len_aux < 32)
			bitW = ~bitget_go(data,pos-len_aux,len_aux) << (W-len_aux);
		else
			bitW = ~bitget_go(data,pos-W,W);
		cont = 0;
		while (bitW && (exc < b/2)) {
			/*either we shift it all or it only closes parentheses or to
			 * many closed parentheses*/
			_W1 = (bitW >> (W-8));
			if(exc < W/2){
				if ((res = BwdPos[_W1][exc])) 
					return v-len-res;
			}
			bitW <<= 8; 
			exc += Excess[_W1];  /*note _W1 is complemented!*/
			len += 8;
			cont++;
		}		
		if(cont != 4){
			_W1=0;
			while(cont!=4){
				exc += Excess[_W1];
				len +=8;
				cont++;
			}
		}
		pos-=32;
		len_aux -=32;
	}			
	/* ok, it's not a small distance, try with hashing btable*/
	minres = 0;
	myexcess = excess(v-1)-1;
	res = searchHash(open_bbtable,v,&h);
	while (res) {
		if (!minres || (res < minres)) 
			if (((int)(v-res) >= 0) && (excess(v-res-1) == myexcess)) 
				minres = res;
		res = nextHash(open_bbtable,&h);
	}
	if (minres) 
		return v-minres;
	/* finally, it has to be a far pointer*/
	res = searchHash(open_sbtable,v,&h);
	while (res) {
		if (!minres || (res < minres))
			if (((int)(v-res) >= 0) && (excess(v-res-1) == myexcess))
				minres = res;
		res = nextHash(open_sbtable,&h);
	}
	return v-minres; /*there should be one if the sequence is balanced!*/
}

uint bp_hb::enclose(int v){
	if (v == 0) 
		return (uint)-1; // no parent!
	return parent(v);
}

uint bp_hb::parent(int v){
	uint bitW;
	uint len,res,minres,exc;
	uint len_aux;
	uint pos = v;
	int cont;
	byte _W1;
	handle h;
	uint myexcess;
	/* first see if it is at small distance*/
	len = b; 
	if ((uint)v < len) 
		len = v;
	len_aux = len;
	exc = 0; 
	len = 0; 
	do{
		if((int)len_aux < 32)
			bitW = ~bitget_go(data,pos-len_aux,len_aux) << (W-len_aux);
		else
			bitW = ~bitget_go(data,pos-W,W);
		cont = 0;
		while (bitW && (exc < b/2)) {
			/* either we shift it all or it only closes parentheses or too
			 * many closed parentheses*/
			_W1 = (bitW >> (W-8));
			if(exc < W/2){
				if ((res = BwdPos[_W1][exc]))
					return v-len-res;
			}
			bitW <<= 8; 
			exc += Excess[_W1]; // note _W1 is complemented!
			len += 8;
			cont ++;
		} 
		if(cont != 4){
			_W1=0;
			while(cont!=4){
				exc += Excess[_W1];
				len +=8;
				cont++;
			}
		}
		pos -= 32;
		len_aux -= 32;
	}	
	while((int)len_aux > 0);
	/* ok, it's not a small distance, try with hashing btable*/
	minres = 0;
	myexcess = excess(v-1) - 1;
	res = searchHash(bbtable,v,&h);
	while (res) {
		if (!minres || (res < minres)) 
			if (((int)v-(int)res >= 0) && (excess(v-res-1) == myexcess)) 
				minres = res;
		res = nextHash(bbtable,&h);
	}
	if (minres) 
		return v-minres;
	/*finally, it has to be a far pointer*/
	res = searchHash(sbtable,v,&h);
	while (res) {
		if (!minres || (res < minres)) 
			if (((int)v-(int)res >= 0) && (excess(v-res-1) == myexcess))
				minres = res;
		res = nextHash(sbtable,&h);
	}
	return v-minres;  /*there should be one if the sequence is balanced!*/
}


uint bp_hb::size(){
	uint mem = sizeof(bp_hb);
	mem += bdata->size();
	mem += sizeofHash(sftable);
	mem += sizeofHash(sbtable);
	mem += sizeofHash(bftable);
	mem += sizeofHash(bbtable);
	mem += sizeofHash(open_sbtable);
	mem += sizeofHash(open_bbtable); 
	mem += 2*sizeof(byte)*256*W/2;
	mem += sizeof(char)*256;
	return mem;
}

int bp_hb::save(FILE *fp){
	uint wr = HB;
	fwrite(&wr,sizeof(uint),1,fp);
	fwrite(&n,sizeof(uint),1,fp);
	fwrite(&sbits,sizeof(uint),1,fp);
	fwrite(&b,sizeof(uint),1,fp);
	if(bdata->save(fp) != 0){
		cout << "Error" << endl;
		return 1;
	}
	saveHash(fp, sftable);
	saveHash(fp, sbtable);
	saveHash(fp, bftable);	
	saveHash(fp, bbtable);	
	saveHash(fp, open_sbtable);
	saveHash(fp, open_bbtable);
	return 0;
}

bp_hb * bp_hb::load(FILE *fp){
	bp_hb *new_bp = new bp_hb();
	fread(&new_bp->n,sizeof(uint),1,fp);
	fread(&new_bp->sbits,sizeof(uint),1,fp);
	fread(&new_bp->b,sizeof(uint),1,fp);
	new_bp->bdata = static_bitsequence::load(fp);
	new_bp->data = ((static_bitsequence_brw32 *)new_bp->bdata)->data;
	new_bp->sftable = loadHash(fp);
	new_bp->sbtable = loadHash(fp);
	new_bp->bftable = loadHash(fp);
	new_bp->bbtable = loadHash(fp);
	new_bp->open_sbtable = loadHash(fp);
	new_bp->open_bbtable = loadHash(fp);
	/*compute tables*/
	for (int i=0;i<256;i++) {
		new_bp->fcompchar(i,new_bp->FwdPos[i],new_bp->Excess+i);
		new_bp->bcompchar(i,new_bp->BwdPos[i]);
	}
	return new_bp;
}

