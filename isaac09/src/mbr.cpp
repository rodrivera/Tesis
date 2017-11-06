#include "mbr.h"

int mbr::readMBRs(char *input_file_name, mbr** input_data, uint *size) {
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
	mbr *temp_input = (mbr *) malloc(number_of_elements * sizeof(mbr));	
	long id = 0;
	double minx = 0;
	double miny = 0;
	double maxx = 0;
	double maxy = 0;
	while(fscanf(input_file, "%li %lf %lf %lf %lf", &id, &minx, &miny, &maxx, &maxy) != EOF) {
		temp_input[i] = mbr(id, minx, miny, maxx, maxy);
		i++;
	}
	
	*size = i;
	*input_data = temp_input;
	
	fclose(input_file);
	
	return 0;
}

int mbr::readMBRsAndAssignIDs(char *input_file_name, mbr** input_data, uint *size) {
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
	mbr *temp_input = (mbr *) malloc(number_of_elements * sizeof(mbr));	
	long id = 0;
	double minx = 0;
	double miny = 0;
	double maxx = 0;
	double maxy = 0;
	while(fscanf(input_file, "%lf %lf %lf %lf", &minx, &miny, &maxx, &maxy) != EOF) {
		temp_input[i] = mbr(id++, minx, miny, maxx, maxy);
		i++;
	}
	
	*size = i;
	*input_data = temp_input;
	
	fclose(input_file);
	
	return 0;
}
