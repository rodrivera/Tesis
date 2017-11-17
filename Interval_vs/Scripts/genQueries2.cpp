#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

int main(int argc, char const *argv[])
{

	if (argc < 2){
		cout << "Usage: ./gen num_queries" << endl;
		return -1;
	}

	double minT = 0.0, maxT = 100.0;
	double dT = maxT-minT;

	int num = atoi(argv[1]);

	srand(time(NULL));

	for (int i = 0; i < num; ++i){

		double random = ((double) rand()) / (double) RAND_MAX;
    double qT1 = random * dT;
    random = ((double) rand()) / (double) RAND_MAX;
    double qT2 = random * dT;

    if (qT1 < qT2) {
    	cout << qT1 << " " << qT2 << endl;
    } else {
    	cout << qT2 << " " << qT1 << endl;
    }
	}

	return 0;
}
