#include <iostream>
#include <cstring>
#include <cstdlib>

#include "tiempo.h"
#include "mbr_range.h"

#define MAX_RESULTS 2000000

int cmplong (const void * a, const void * b)
{
  return ( *(long*)a - *(long*)b );
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Usage: %s input_file file_type [IdX1Y1X2Y2 | ...].\n", argv[0]);
		return -1;
	}

	uint fileType = 0;
	uint queryFileType = 0;
	uint structureType = 0;

	if (strcmp(argv[2], "IdX1Y1X2Y2") == 0) fileType = 0;
	else if (strcmp(argv[2], "X1Y1X2Y2") == 0) fileType = 1;
	else if (strcmp(argv[2], "IdY1X1Y2X2") == 0) fileType = 2;
	else if (strcmp(argv[2], "Y1X1Y2X2") == 0) fileType = 3;
	else
	{
		printf("Unknown file type.");
		return -1;
	}
	if (strcmp(argv[argc - 3], "IdX1Y1X2Y2") == 0) queryFileType = 0;
	else if (strcmp(argv[argc - 3], "X1Y1X2Y2") == 0) queryFileType = 1;
	else if (strcmp(argv[argc - 3], "IdY1X1Y2X2") == 0) queryFileType = 2;
	else if (strcmp(argv[argc - 3], "Y1X1Y2X2") == 0) queryFileType = 3;
	else
	{
		printf("Unknown query file type.");
		return -1;
	}
	if (strcmp(argv[argc - 1], "COMPACT") == 0) structureType = COMPACT;
	else if (strcmp(argv[argc - 1], "NOCOMPACT") == 0) structureType = NO_COMPACT;
	else
	{
		printf("Unknown structure type.");
		return -1;
	}

	FILE* outputFile = fopen(argv[3], "a");

	//read input elements
	uint number_of_input_bboxes;
	mbr *input_bboxes;
	if(fileType == 0) {
		mbr::readMBRs(argv[1], &input_bboxes, &number_of_input_bboxes);
	} else if(fileType == 1) {
		mbr::readMBRsAndAssignIDs(argv[1], &input_bboxes, &number_of_input_bboxes);
	} else {
		printf("Not supported file type.");
	}

	printf("Creando estructuras..\n");
	mbr_range is = mbr_range(input_bboxes, number_of_input_bboxes, structureType);
	printf("Estructuras creadas..\n");


	long* resultWT = (long*) malloc(MAX_RESULTS * sizeof(long));
	uint k;
	for(k = 4; k < argc - 3; k++) {
		//read input queries
		uint number_of_input_queries;
		mbr *input_queries;
		if(queryFileType == 0) {
			mbr::readMBRs(argv[k], &input_queries, &number_of_input_queries);
		} else if(queryFileType == 1) {
			mbr::readMBRsAndAssignIDs(argv[k], &input_queries, &number_of_input_queries);
		} else {
		  printf("Not supported query type.");
		}
		
		uint i,j,l;
		uint resultSizeWT;
		double time = 0;
		
		if(!strcmp(argv[argc - 2], "time")) {
			for(i = 0; i < number_of_input_queries; i++) {
				tTiempo mitiempo1;
				tTiempo mitiempo2;
				getTiempo(&mitiempo1);
				
				for(j = 0; j < 5; j++) {
					resultSizeWT = 0;
					is.query(&input_queries[i], resultWT, &resultSizeWT);
				}
				
				getTiempo(&mitiempo2);
				time += ((mitiempo2.user - mitiempo1.user) / 5);
				//fprintf(outputFile, "%ld;%ld;%lf\n", input_queries[i].id, resultSizeWT, (mitiempo2.user - mitiempo1.user) / 10);
			}		
			fprintf(outputFile, "%lf;", time / number_of_input_queries);
		} else if(!strcmp(argv[argc - 2], "test")) {
			uint error = 0;
			for(i = 0; i < number_of_input_queries; i++) {
				resultSizeWT = 0;
				is.query(&input_queries[i], resultWT, &resultSizeWT);
				printf("Query %ld: %d elements: ", input_queries[i].getId(), (uint)resultSizeWT);
				qsort(resultWT, resultSizeWT, sizeof(long), cmplong);
				for(j = 0; j < resultSizeWT; j++) {
					printf("%ld; ", resultWT[j]);
				}
				printf("\n");
				
			}
			if(!error) {
				fprintf(outputFile, "All tests OK.\n");
			}
		} else {
			printf("Unknown option: %s\n", argv[argc - 2]);
		}
	
		free(input_queries);
	}
	
	free(resultWT);
	free(input_bboxes);

	fprintf(outputFile, "\n");
	fclose(outputFile);

	return 0;
}
