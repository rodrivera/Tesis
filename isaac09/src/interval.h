#ifndef INTERVAL_H
#define	INTERVAL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class interval {
  public:
	interval(long _id, double _min, double _max) {
	  id = _id;
	  min = _min;
	  max = _max;
	}
	~interval() {}
	long getId() {return id;}
	double getMin() {return min;}
	double getMax() {return max;}
	uint getMinRank() {return minRank;}
	uint getMaxRank() {return maxRank;}
	void setMinRank(uint _minRank) {minRank=_minRank;}
	void setMaxRank(uint _maxRank) {maxRank = _maxRank;}
	static int readIntervals(char *input_file_name, interval **input_data, uint *size);

  private:
	long id;
	double min,max;
	uint minRank,maxRank;
};
#endif
