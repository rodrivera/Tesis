#ifndef MBR_H
#define	MBR_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class mbr {
  public:
	mbr(long _id, double _minx, double _miny, double _maxx, double _maxy) {
	  id = _id;
	  minx = _minx;
	  miny = _miny;
	  maxx = _maxx;
	  maxy = _maxy;
	}
	~mbr() {}
	long getId() {return id;}
	double getMinX() {return minx;}
	double getMinY() {return miny;}
	double getMaxX() {return maxx;}
	double getMaxY() {return maxy;}
	static int readMBRs(char *input_file_name, mbr **input_data, uint *size);
	static int readMBRsAndAssignIDs(char *input_file_name, mbr **input_data, uint *size);

  private:
	long id;
	double minx,miny,maxx,maxy;
};
#endif
