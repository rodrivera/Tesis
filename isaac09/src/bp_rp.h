#ifndef BP_RP_H
#define	BP_RP_H

/*Rodrigo Canovas implementation of Geary's
* balanced parentheses.
*****************************************************/


#include "balanced_parentheses.h"

class bp_rp: public balanced_parentheses{
	private:
		uint S1_size,   /*size of bitmap S*/
				 S2_size, /*size of bitmap S2*/
				 lvl3_size, /*size last level*/
				 b_lvl3,  /*bits used for each value*/
				 b;       /*tamaño de los bloques*/
						
		//static_bitsequence *S1;  /*first level*/
		static_bitsequence *S2; /*second level*/
		static_bitsequence *P1;
		static_bitsequence *P2;
		uint *Table_max_closers;
		uint *Table_max_opens;
		uint *Table_exceso;
		uint *enclose_lvl3;  /*respuestas directas en el 3er nivel*/
		uint *lvl3;   

		bp_rp();
		void create_P(uint **p, uint *s ,int s_size);
		void create_S2(uint **s); 
		void calculate_lvl3();
		void create_tables();
		uint  close_prima(int v);
		int open_prima(int v);
		int enclose_prima(int v);		
		int GetCloser(int v, int block, static_bitsequence *F, int cont_ini); 
		int GetOpen(int v, int block, static_bitsequence *F, int cont_ini);

	public:
		 static_bitsequence *S1;  /*first level*/

		bp_rp(uint *bitarray, int ba_size, int block_size);
		virtual ~bp_rp();
		virtual uint excess(int v);
		virtual uint close(int v);
		virtual uint open(int v);
		virtual uint enclose(int v);
		virtual uint size();
		virtual int save(FILE *fp);
		static bp_rp * load(FILE *fp);
};

#endif

