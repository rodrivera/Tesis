#include <iostream>
#include "includes/schmidt/FastStabbing.h"

#include <ogdf/basic/Array.h>
#include <ogdf/basic/basic.h>

#include <vector>
#include <set>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <chrono>
#include <iomanip>

using namespace ogdf;

int main(int argc, char const *argv[])
{

	if(argc != 4)
	{
		cout << "Usage: ./fnrtest [intervalsFile] [queriesFile] [outFile]" << endl;
		return -1;
	}

	const char* intervalsFile 	= argv[1];
	const char* queriesFile 		= argv[2];
	const char* outFile 				= argv[3];

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		std::ifstream infile(intervalsFile);
		std::string line;
		std::vector<double> vl, vr;

		while(getline(infile,line)){
			std::istringstream iss(line);
			double t1, t2;
			if(!(iss >> t1 >> t2)) break;

			vl.push_back(t1);
			vr.push_back(t2);
		}
		int n = vl.size();
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
		vl.clear(); vr.clear();

		Array<interval> a(n);
		for (int i = 0; i < n; ++i) {
			a[i].l = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vlo[i]) - vd.begin()) + 1 );
			a[i].r = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vro[i]) - vd.begin()) + 1 );
			a[i].value = (i+1);
			//cout << "<" << a[i].l << ", " << a[i].r << "> o (" << vlo[i] << ", " << vro[i] << ")" << endl;
		}

		IntervalComparerInv comp;
		a.quicksortCT(0,n-1,comp);

	FastStabbing stabbing(a,n,n*4);

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	
	cout << "> Schmidt indicators:" << endl;
	cout << "   > MEMORY USAGE  = " << std::right << std::setw(10);
	cout << stabbing.size();
	cout << " Bytes" << endl;
	cout << "   > Building time = " << std::right << std::setw(10);
	cout << std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();
	cout << " microseconds" << endl;
	cout << "   > Height        = " << std::right << std::setw(10);
	cout << stabbing.height();
	cout << endl;
	
	std::ifstream qinfile(queriesFile);
	std::ofstream outfile(outFile);
	std::vector<interval*> output;
	int cont = 1;
	std::set<long> resArray;

	std::chrono::microseconds duration(0);

	while(getline(qinfile,line)){
		std::istringstream qiss(line);
		double l, r;
		if(!(qiss >> l >> r)) break;
		long numComparisons = 0;

		std::chrono::high_resolution_clock::time_point qstart = std::chrono::high_resolution_clock::now();		

		int fl = 2 * ( (std::lower_bound(vd.begin(),vd.end(),l) - vd.begin()) + 1 );
		int fr = 2 * ( (std::lower_bound(vd.begin(),vd.end(),r) - vd.begin()) + 1 );
		if (l != vd[fl/2 - 1]) fl--;
		if (r != vd[fr/2 - 1]) fr--;
		stabbing.query(fl,fr,output,false,numComparisons);

		std::chrono::high_resolution_clock::time_point qend = std::chrono::high_resolution_clock::now();
		duration += std::chrono::duration_cast<std::chrono::microseconds>( qend - qstart );

			/*
			long outputSize = (int)output.size();
			cout << ">>: N = " << outputSize << endl;
			for (int i = 0; i < outputSize; ++i)
				cout << " [" << output[i]->value << "=(" << vd[(output[i]->l)/2-1] << "," << vd[(output[i]->r)/2-1] << ")]" << endl;
			cout << endl;	
			*/

		resArray.clear();
		for(int i=0;i<output.size();i++){
			resArray.insert(output[i]->value);
		}

		outfile << "Test #" << cont++ << endl;
		for (std::set<long>::iterator it=resArray.begin(); it!=resArray.end(); ++it){
			outfile << *it << " ";
		}
		outfile << endl << endl;

	}

	cout << "   > Queries time  = " << std::right << std::setw(10);
	cout << duration.count();
	cout << " microseconds" << endl;
	outfile.close();

	return 0;
}