#ifndef IS_IIS_H
#define IS_IIS_H

#define D(x)
//#define D(x) cout<<x<<endl

#include "sdsl/bit_vectors.hpp"
#include "sdsl/rank_support.hpp"
#include "sdsl/select_support.hpp"

#include <vector>

using namespace sdsl;
using namespace std;

struct interval {
		double l,r;
		long value;
};

bool IntervalCompare(const interval& x, const interval& y) {
	if (x.l < y.l) return true;
	if (x.l > y.l) return false;
	if (x.r < y.r) return true; // same starting point: ascending order
	if (x.r > y.r) return false;
	return false;
}

class Stabbing{
private:
	long discr_multiplier;
  long domain_size;

	sd_vector<>* sdv_start;
	sd_vector<>* sdv_stop;
	rank_support_sd<>* rs_start;
	rank_support_sd<>* rs_stop;
	select_support_sd<>* ss_start;
	select_support_sd<>* ss_stop;

  //int_vector<0> ids;
  vector<vector<long> > v_ids;

public:
	Stabbing(vector<interval>& intervals, long domainSize,long discrMultiplier = 100000){

		D("Construct BEGIN:");

		long num_intervals = intervals.size();
		domain_size = domainSize;
		discr_multiplier = discrMultiplier;

		D("    > structure vars set");

		sort(intervals.begin(), intervals.end(), IntervalCompare);
		//ids.width(ceil(log2(num_intervals)+1)); // ?!
		//ids.resize(num_intervals); 

		D("    > intervals sorted");

		v_ids.resize(num_intervals);

		v_ids[0].push_back(intervals[0].value);
		int cont = 0;
		for (int i = 1; i < intervals.size(); ++i) {
			if ( (intervals[i].l != intervals[i-1].l) || (intervals[i].r != intervals[i-1].r) ) {
				cont++;
			}
			v_ids[cont].push_back(intervals[i].value);
		}

		D("  > ids created");

		bit_vector start = bit_vector(domain_size*discr_multiplier, false);
	  bit_vector stop  = bit_vector(domain_size*discr_multiplier, false);

	  D("  > bvs created");

	  for (int i=0; i < intervals.size(); ++i) {
			start[ floor(intervals[i].l * discr_multiplier) ] = true;
			stop[ floor(intervals[i].r * discr_multiplier) ] = true;
		}

		D("  > start/stop created");

		sdv_start = new sd_vector<>(start);
    sdv_stop = new sd_vector<>(stop);

    D("  > sdv created");

		rs_start = new rank_support_sd<>(sdv_start);
	  rs_stop = new rank_support_sd<>(sdv_stop);

	  D("  > rs created");

	  ss_start = new select_support_sd<>(sdv_start);
	  ss_stop = new select_support_sd<>(sdv_stop);

	  D("  > ss created");

	  util::clear(start);
	  util::clear(stop);
	  D("  > clear");
	  D("Construct DONE");
	}

	void query(const double& ql, const double& qr, std::vector<long>& output){
		output.clear();

		if(! (ql >= 0 && ql <= domain_size && qr >= ql && qr <= domain_size) ){
			cout << "> q = [ " << ql << " , " << qr << " ]" << endl;
			cout << "> D = [ " << 1 << " , " << domain_size << " ]" << endl;
			cout << "Out of bounds query." << endl;
			return;
		}

		long q_l = ql * discr_multiplier; 
	  long q_r = qr * discr_multiplier;

	  long i_n = (*rs_start)( q_r + 1 ); // que query is exclusive
	  long i_r = (*ss_stop)( i_n );
	  
	  long i_l = (*ss_start)( i_n ); if ( q_r < i_l ) i_n--; // ???

	  while( q_l <= i_r ) {
  		
      D(" >> "<<i_n<<" = ("<<(double)(*ss_start)(i_n)/discr_multiplier<<", "<<(double)i_r/discr_multiplier<<") : "<<v_ids[i_n-1]);
      
      for (int i = 0; i < v_ids[i_n-1].size(); ++i) {
      	output.push_back( v_ids[i_n-1][i] );
      }

      i_n--;
      if(i_n < 1) break;
      i_r = (*ss_stop)( i_n );
	  }
	  D("~~~" << endl);
	}
	size_t size() {
		return 	sizeof(*this) + 
						sizeof(*sdv_start) +
						sizeof(*sdv_stop) +
						sizeof(*rs_start) +
						sizeof(*rs_stop) +
						sizeof(*ss_start) +
						sizeof(*ss_stop);
	}
};

#endif