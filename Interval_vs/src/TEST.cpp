#include "includes/schmidt/FastStabbing.h"
#include <vector>

#include <ogdf/basic/Array.h>
#include <ogdf/basic/basic.h>

#include <iostream>
#include <algorithm>

using namespace ogdf;

int main(){

	int n = 10;

	Array<int> queries(20);
	std::vector<interval*> output;

		/**************
				INI DISC
		**************/

		cin >> n;
		std::vector<double> vl, vr;
		int l,r;
		for (int i = 0; i < n; ++i){
			cin >> l >> r;
			vl.push_back(l);
			vr.push_back(r);
		}
		std::vector<double> vlo=vl, vro=vr;

		std::vector<double> vd;
		sort(vl.begin(), vl.end());
		sort(vr.begin(), vr.end());
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

		Array<interval> a(n);
		for (int i = 0; i < n; ++i) {
			a[i].l = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vlo[i]) - vd.begin()) + 1 );
			a[i].r = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vro[i]) - vd.begin()) + 1 );
			a[i].value = (i+1);
			cout << "<" << a[i].l << ", " << a[i].r << "> o (" << vlo[i] << ", " << vro[i] << ")" << endl;
		}

		/**************
				FIN DISC
		**************/

		IntervalComparerInv comp;
		a.quicksortCT(0,n-1,comp);

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
			cout << " [" << output[i]->value << "=(" << vd[(output[i]->l)/2-1] << "," << vd[(output[i]->r)/2-1] << ")]";
		}
		cout << endl;	
	}

	return 0;
}