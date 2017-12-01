#include "sdsl/bit_vectors.hpp"
#include "sdsl/rank_support.hpp"
#include "sdsl/select_support.hpp"

using namespace sdsl;
using namespace std;

int main(int argc, char* argv[]) {

    long discr_multiplier   = 100000;
    long domain_size        = 100;
    bit_vector start    = bit_vector(domain_size*discr_multiplier, false);
    bit_vector stop     = bit_vector(domain_size*discr_multiplier, false);

    double l, r;
    for (int i=0; i < 10; ++i) {
        cin >> l >> r;
        start[ floor(l * discr_multiplier) ] = true;
        stop[ floor(r * discr_multiplier) ] = true;
    }

    sd_vector<> sdv_start(start);
    sd_vector<> sdv_stop(stop);
    rank_support_sd<> rs_start(&sdv_start);
    //rank_support_sd<> rs_stop(&sdv_stop);
    //select_support_sd<> ss_start(&sdv_start);
    select_support_sd<> ss_stop(&sdv_stop);
    util::clear(sdv_start);
    util::clear(sdv_stop);

    cout << endl << "Queries: " << endl;
    for (int i=0; i < 5; ++i) {
        cout << "#"<< i+1 <<" : " << endl;
        cin >> l >> r;
        if(!(0<=l&&l<=domain_size&&0<=r&&r<=domain_size&&l<=r)) continue;
        long q_l = l * discr_multiplier; 
        long q_r = r * discr_multiplier;

        long i_n = rs_start( q_r + 1 ); // que query is exclusive
        long i_r = ss_stop( i_n );

        while( i_r >= q_l ) {

            cout <<" >> "<<i_n;
            //cout << " = ("<<(double)ss_start(i_n)/discr_multiplier<<", "<<(double)i_r/discr_multiplier<<")";
            cout << endl;

            i_n--;
            if(i_n < 1) break;
            i_r = ss_stop( i_n );
        }

    }

    return 0;
}

