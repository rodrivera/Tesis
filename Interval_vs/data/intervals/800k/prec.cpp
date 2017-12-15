#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[]){
	double n1, n2;
	int prec = atoi(argv[1]);
	cout << fixed << setprecision(prec);
	while(cin >> n1 >> n2) {
		cout << n1 << " " << n2 << endl;
	}
	return 0;
}