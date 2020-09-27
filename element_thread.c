#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
//============================================================================================================================================================
//part 1-3
struct Convo{
	int **arr_before;
	int **arr_after;
	int *mask;
	int *col;
	int num_convo;
	int i, j;
};
//============================================================================================================================================================
void *Convolution(void *item){

    	struct Convo *struct_item = (struct Convo*)item;
    	int i = (*struct_item).i;
    	int j = (*struct_item).j;
    	int col = *(*struct_item).col;
    	
    	if(j != 0)
	{
		(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j-1])*((*struct_item).mask[0]);
	}
	if(j != col-1)
	{
		(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j+1])*((*struct_item).mask[2]);
	}
	(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j])*((*struct_item).mask[1]);
	(*struct_item).num_convo++;
	
    	return NULL; 
}
//============================================================================================================================================================
int Check(int argc, char *argv[])
{
	if(argc > 2)
	{
		printf("Error too many arguments\n");
		return 0;
	}
	else if(argc < 2)
	{
		printf("Error expected one more argument\n");
		return 0;
	}
    	return 1;//if everything checks well return 1 as in true
}

//============================================================================================================================================================
int main(int argc, char *argv[])
{
    	//read file
	int check = Check(argc, argv);//Check() function
	
	//if Check function return 0, exit program
	if(check == 0)
	{
		return 0;
	}

    	char *p = (char*)malloc(sizeof(char)*1);//saves file name in heap
    	p = argv[1];
    	printf("Opening %s...\n", p);//print we are opening file number given
	
	int **arr_before;
	int **arr_after;
	int *mask;
    	int row, col, mask_row, mask_col;
//----------------------------------------------------------------------------------------------------------------------------------
	FILE *openfile = fopen(p, "r");


	if(openfile == NULL)
	{
		printf("File does not exist. Please use a proper data file and include it in the same directory as the program.\n");
		fclose(openfile);
		free(p);
		return 0;
    	}
	else
	{
		int i,j;
		fscanf(openfile, "%d", &row);
		fscanf(openfile, "%d", &col);

		//initialize array for both original matrix and container to store convolutioned matrix
		arr_before = malloc(sizeof(int*)*row);
		arr_after = malloc(sizeof(int*)*row);
		for(i = 0; i < row; i++)
		{
			arr_before[i] = malloc(sizeof(int)*col);
			arr_after[i] = malloc(sizeof(int)*col);
		}

		//read matrix in file
		for(i = 0; i < row; i++)
		{
			for(j = 0; j < col; j++)
			{
				fscanf(openfile, "%d", &arr_before[i][j]);
			}
		}

		fscanf(openfile, "%d", &mask_row);
		fscanf(openfile, "%d", &mask_col);

		mask = malloc(sizeof(int)*mask_col);

		//get masking values
		fscanf(openfile, "%d", &mask[0]);
		fscanf(openfile, "%d", &mask[1]);
		fscanf(openfile, "%d", &mask[2]);

		//close file
		fclose(openfile);
	}
//----------------------------------------------------------------------------------------------------------------------------------
    	//declaration
    	
    	int total_elements = row*col;
	int num_convo = 0;
    	pthread_t thread[total_elements];
    	int create_count = 0;
//----------------------------------------------------------------------------------------------------------------------------------
	struct Convo *convo = malloc(sizeof(struct Convo)*total_elements);
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			convo[create_count].arr_before = arr_before;
			convo[create_count].arr_after = arr_after;
			convo[create_count].mask = mask;
			convo[create_count].col = &col;
			convo[create_count].num_convo = num_convo;
			convo[create_count].j = j;
			convo[create_count].i = i;
			create_count++;
		}
	}
	create_count = 0;
	clock_t begin = clock();
	
    	//create threads
	while(create_count != total_elements)
	{
		pthread_create(&thread[create_count], NULL, Convolution, &convo[create_count]);
		create_count++;
	}
	
	
//----------------------------------------------------------------------------------------------------------------------------------
    	
    	create_count = 0;
    	
	while(create_count != total_elements)
	{
		pthread_join(thread[create_count], NULL);
		create_count++;
	}
	
	clock_t end = clock();
 //----------------------------------------------------------------------------------------------------------------------------------   
	printf("Convoluted Array\n");
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			printf("%d ", (*convo).arr_after[i][j]);
		}
		printf("\n");
	}
	printf("row: %d\n", row);
	printf("col: %d\n", col);
	create_count = 0;
	while(create_count != total_elements)
	{
		num_convo += convo[create_count].num_convo;
		create_count++;
	}
	double total_time = (double)(end-begin)/ CLOCKS_PER_SEC;
	printf("Total time: %lf seconds\n", total_time);
	printf("Total number of convolution operations performed: %d\n", num_convo);
	return 0;
}
