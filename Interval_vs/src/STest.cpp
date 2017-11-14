#include <iostream>
#include "includes/schmidt/FastStabbing.h"

#include <ogdf/basic/Array.h>
#include <ogdf/basic/basic.h>

#include <set>
#include <map>
#include <fstream>
#include <sstream>

#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace ogdf;

FastStabbing* stabbing;
std::vector<double> vd;

void readIntervals(const char *filename){
	ifstream infile(filename);
	string line;
	std::vector<double> vl, vr;

	while(getline(infile,line)){
		istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;

		vl.push_back(t1);
		vr.push_back(t2);
	}
	int n = vl.size();
	Array<interval> a(n);

	vector<double> vlo = vl;
	vector<double> vro = vr; // DELETE : R in trajectories should be sorted
	
	vd.clear();
	sort(vl.begin(), vl.end());
	sort(vr.begin(), vr.end()); // DELETE : R in trajectories should be sorted
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

	for (int i = 0; i < n; ++i) {
		a[i].l = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vlo[i]) - vd.begin()) + 1 );
		a[i].r = 2 * ( (std::lower_bound(vd.begin(),vd.end(),vro[i]) - vd.begin()) + 1 );
		a[i].value = (i+1);
		cout << "<" << a[i].l << ", " << a[i].r << "> o (" << vlo[i] << ", " << vro[i] << ")" << endl;
	}

	IntervalComparerInv comp;
	a.quicksortCT(0,n-1,comp);

	stabbing = new FastStabbing(a,n,n*4);
}

void readQueries(const char *inFilename, const char *outFilename){
	ifstream infile(inFilename);
	ofstream outfile(outFilename);
	string line;
	int cont = 1;
	vector<interval*> output;
	set<long> resArray;

	chrono::microseconds duration(0);

	while(getline(infile,line)){
		istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;

		long numComparisons = 0;

		int fl = 2 * ( (std::lower_bound(vd.begin(),vd.end(),t1) - vd.begin()) + 1 );
		int fr = 2 * ( (std::lower_bound(vd.begin(),vd.end(),t2) - vd.begin()) + 1 );
		if (t1 != vd[fl/2 - 1]) fl--;
		if (t2 != vd[fr/2 - 1]) fr--;
		output.clear();

		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		//**************************

		//cout << "q: orignal: (" << t1 << ", " << t2 << ")" << endl;
		cout << "   dicret.: <" << fl << ", " << fr << ">" << endl;
		stabbing->query( fl, fr, output, false, numComparisons);
		//cout << "qEnd " << endl;

		//**************************
		chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

		/*
		resArray.clear();
		for(int i=0;i<output.size();i++){
			resArray.insert(output[i]->value);
		}
		*/
		
		duration += chrono::duration_cast<chrono::microseconds>( end - start );	

		/*
		outfile << "Test #" << cont++ << endl;
		for (set<long>::iterator it=resArray.begin(); it!=resArray.end(); ++it){
			outfile << *it << " ";
		}
		outfile << endl << endl;
		*/

		outfile << "Test #" << cont++ << endl;
		long outputSize = (int)output.size();
		cout << ">>: N = " << outputSize << endl;
		for (int i = 0; i < outputSize; ++i) {
			cout<<" ["<<output[i]->value<<"=("<<vd[(output[i]->l)/2-1]<<","<<vd[(output[i]->r)/2-1]<<")]"<<endl;
			outfile << output[i]->value << " ";
		}
		outfile << endl << endl;
	}

	cout << "   > Queries time  = " << right << setw(10);
	cout << duration.count();
	cout << " microseconds" << endl;
	outfile.close();
}

int main(int argc, char const *argv[])
{

	if(argc != 4)
	{
		cout << "Usage: ./fnrtest [intervalsFile] [queriesFile] [outFile]" << endl;
		return -1;
	}

	const char* intervalsFile 	= argv[1];
	const char* queriesFile 		= argv[2];
	const char* outFile 			= argv[3];
	
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	//stabbing = new RTree<long, double, 1, float>();



	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	chrono::high_resolution_clock::time_point start2 = chrono::high_resolution_clock::now();
	readIntervals(intervalsFile);

	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

	cout << "> Schmidt indicators:" << endl;
	cout << "   > Building time = " << right << setw(10);
	cout << chrono::duration_cast<chrono::microseconds>( end - start2 ).count();
	cout << " microseconds" << endl;

	readQueries(queriesFile, outFile);
	//cout << endl;

	return 0;
}
