#include "FastStabbing.h"
#include <vector>

#include <ogdf/basic/Array.h>
#include <ogdf/basic/basic.h>

#include <iostream>

using namespace ogdf;

int main(){

	int n = 100;

	Array<interval> a(n);
	Array<int> queries(20);
	std::vector<interval*> output;


	bool identical = false;
	do {
		for (int i = 0; i < n; ++i) {
			do {
				//a[i].l = randomNumber(1,n*2);
				a[i].l = i+1;
				//a[i].r = randomNumber(1,n*2);
				a[i].r = i+7;
			} while (a[i].l > a[i].r); // intervals [a,a] are allowed
			a[i].parent = NULL;
			a[i].leftsibling = NULL;
			a[i].rightchild = NULL;
			a[i].pIt = NULL;
			a[i].smaller = NULL;
			#ifdef OGDF_DEBUG
			a[i].stabbed = false;
			#endif
		}
		IntervalComparerInv comp;
		a.quicksortCT(0,n-1,comp);
		identical = false;
		for (int i = 1; i < n; ++i) {
			if (a[i].l == a[i-1].l && a[i].r == a[i-1].r) {
				identical = true;
				break;
			}
		}
	} while (identical); // while identical intervals exist



	for (int i = 0; i < 20; ++i){
		queries[i] = 4*i + 2;
	}
	
	FastStabbing stabbing(a,100,100*2);

	for (int j = 0; j < 20; ++j) {
		long numComparisons = 0;
		stabbing.query(queries[j],output,false,numComparisons);
		long outputSize = (int)output.size();

		cout << "*****" << endl;

		for (int i = 0; i < outputSize; ++i)
		{
			cout << " (" << output[i]->l << "," << output[i]->r << ")";
		}

		cout << endl << "*****" << endl;
	}



	return 0;
}