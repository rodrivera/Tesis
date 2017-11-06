#include "interval.h"

int interval::readIntervals(char *input_file_name, interval** input_data, uint *size) {
	FILE *input_file = fopen(input_file_name, "r");
	
	if(input_file == NULL) { return -1; }
	
	//count number of elements
	int i = 0;
	int number_of_elements = 0;
	while((i = getc(input_file)) != EOF) {
		if(i == '\n') { number_of_elements++;}
	}
	//restore file at the begin
	i = 0;
	rewind(input_file);
	
	//obtain result
	interval *temp_input = (interval *) malloc(number_of_elements * sizeof(interval));	
	long id = 0;
	double min = 0;
	double max = 0;
	while(fscanf(input_file, "%li %lf %lf", &id, &min, &max) != EOF) {
		temp_input[i] = interval(id, min, max);
		i++;
	}
	
	*size = i;
	*input_data = temp_input;
	
	fclose(input_file);
	
	return 0;
}
