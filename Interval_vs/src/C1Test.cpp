#include <iostream>
#include "includes/isiis/IS-IIS.h"

// ??

#include <vector>
#include <set>
#include <algorithm>

#include <fstream>
#include <sstream>

#include <chrono>
#include <iomanip>

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
	std::vector<interval> intervals;

	long in_id = 1;
	while(getline(infile,line)){
		std::istringstream iss(line);
		double t1, t2;
		if(!(iss >> t1 >> t2)) break;
		interval in;
		in.l = t1; in.r = t2; in.value = in_id++;
		intervals.push_back(in);
	}
	int n = intervals.size();
	//sort(intervals.begin(),intervals.end(),IntervalCompare);

	Stabbing stabbing(intervals,100);
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	

	cout << "> C1      indicators:" << endl;
	cout << "   > MEMORY USAGE  = " << std::right << std::setw(10);
	cout << stabbing.size();
	cout << " Bytes" << endl;
	cout << "   > Building time = " << std::right << std::setw(10);
	cout << std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();
	cout << " microseconds" << endl;

	std::ifstream qinfile(queriesFile);
	std::ofstream outfile(outFile);
	//std::vector<interval*> output;
	std::vector<long> output;
	int cont = 1;
	std::set<long> resArray;

	std::chrono::microseconds duration(0);

	while(getline(qinfile,line)){
		std::istringstream qiss(line);
		double l, r;
		if(!(qiss >> l >> r)) break;

		std::chrono::high_resolution_clock::time_point qstart = std::chrono::high_resolution_clock::now();		

		stabbing.query(l,r,output);

		std::chrono::high_resolution_clock::time_point qend = std::chrono::high_resolution_clock::now();
		duration += std::chrono::duration_cast<std::chrono::microseconds>( qend - qstart );

		/*
		long outputSize = (int)output.size();
		cout << ">>: N = " << outputSize << endl;
		for (int i = 0; i < outputSize; ++i)
			//cout << " [" << output[i]->value << "=(" << vd[(output[i]->l)/2-1] << "," << vd[(output[i]->r)/2-1] << ")]";
			cout << " [" << output[i] << "]";
		cout << endl;
		*/

		resArray.clear();
		for(int i=0;i<output.size();i++){
			resArray.insert(output[i]);
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