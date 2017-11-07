#include "FastStabbing.h"
#include <vector>

#include <ogdf/basic/Array.h>
#include <ogdf/basic/basic.h>

#include <iostream>
#include <algorithm>

using namespace ogdf;

int main(){

	int n = 10;

	Array<interval> a(n);
	Array<int> queries(20);
	std::vector<interval*> output;

	/*
	bool identical = false;
	do {
		for (int i = 0; i < n; ++i) {
			do {
				a[i].l = randomNumber(1,n*2);
				a[i].r = randomNumber(1,n*2);
			} while (a[i].l > a[i].r); // intervals [a,a] are allowed

		}
		*/

		/**************
				INI DISC
		**************/

		std::vector<double> vl(10), vr(10); // vr already sorted

		vl[0] = 2.42; vr[0] = 2.97;
		vl[1] = 3.2; vr[1] = 4.1;
		vl[2] = 2.97; vr[2] = 6.87;
		vl[3] = 1.01; vr[3] = 8.7;
		vl[4] = 7.43; vr[4] = 10.65;
		vl[5] = 10.32; vr[5] = 12.12;
		vl[6] = 5.12; vr[6] = 14.82;
		vl[7] = 6.87; vr[7] = 16.43;
		vl[8] = 17.95; vr[8] = 18.12;
		vl[9] = 3.2; vr[9] = 20.13;

		// 2 -> 1.01
		// 4 -> 2.42
		// 6 -> 2.97
		// 8 -> 3.2
		// 10 -> 4.1
		// 12 -> 5.12
		// 14 -> 6.87

		std::vector<double> vlo(10);
		vlo[0] = 2.42;
		vlo[1] = 3.2;
		vlo[2] = 2.97;
		vlo[3] = 1.01;
		vlo[4] = 7.43;
		vlo[5] = 10.32;
		vlo[6] = 5.12;
		vlo[7] = 6.87;
		vlo[8] = 17.95;
		vlo[9] = 3.2;

		std::vector<double> vd;
		sort(vl.begin(), vl.end());
		std::vector<double>::iterator vl_i = vl.begin();
		std::vector<double>::iterator vr_i = vr.begin();
		if ( !vl.empty() ) {
			vd.push_back(*(vl_i++));
		}
		while (vl_i != vl.end() && vr_i != vr.end() ){
			if (vd.back() == *vl_i) { vl_i++; continue; }
			if (vd.back() == *vr_i) { vr_i++; continue; }
			if (*vl_i <= *vr_i){
				vd.push_back(*(vl_i++));
			} else {
				vd.push_back(*(vr_i++));
			}
		}
		while (vl_i != vl.end() ) {
			if (vd.back() == *vl_i) { vl_i++; continue; }
			vd.push_back(*(vl_i++));
		}
		while (vr_i != vr.end() ) {
			if (vd.back() == *vr_i) { vr_i++; continue; }
			vd.push_back(*(vr_i++));
		}
		//vl.clear();
		//vr.clear();

		for (int i = 0; i < n; ++i) {
			a[i].l = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vlo[i]) - vd.begin()) + 1 );
			a[i].r = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vr[i]) - vd.begin()) + 1 );
			cout << "(" << a[i].l << ", " << a[i].r << ") o (" << vlo[i] << ", " << vr[i] << ")" << endl;
		}

		/**************
				FIN DISC
		**************/

		IntervalComparerInv comp;
		a.quicksortCT(0,n-1,comp);
	
	/*
		identical = false;
		for (int i = 1; i < n; ++i) {
			if (a[i].l == a[i-1].l && a[i].r == a[i-1].r) {
				identical = true;
				break;
			}
		}
	} while (identical); // while identical intervals exist
	*/

	cout << "Creating structure.." << endl;
	FastStabbing stabbing(a,n,n*4);
	cout << "Created" << endl;

	cout << "Queries:" << endl;
	while(true){
		double l, r;
		cin >> l >> r;

		int fl = 2 * ( (std::lower_bound(vd.begin(),vd.end(),l) - vd.begin()) + 1 );
		int fr = 2 * ( (std::lower_bound(vd.begin(),vd.end(),r) - vd.begin()) + 1 );

		if (l != vd[fl/2 - 1]) fl--;
		if (r != vd[fr/2 - 1]) fr--;

		cout << " <" << fl << ", " << fr << ">" << endl;

		long numComparisons = 0;
		stabbing.query(fl,fr,output,false,numComparisons);

		long outputSize = (int)output.size();
		cout << ">>: N = " << outputSize << endl;
		for (int i = 0; i < outputSize; ++i)
		{
			cout << " (" << vd[(output[i]->l)/2-1] << "," << vd[(output[i]->r)/2-1] << ")";
		}
		cout << endl;	
	}

	return 0;
}