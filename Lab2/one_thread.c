#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
//============================================================================================================================================================
//part 1-1
//struct to pass values to the function thread
struct Convo{
    int **arr_before;
    int **arr_after;
    int *mask;
    int row, col;
    int num_convo;
};
//============================================================================================================================================================
//convolution function for threads
void *Convolution(void *item){

    struct Convo *struct_item = (struct Convo*)item;
	//traverse whole matrix
    for(int i = 0; i < (*struct_item).row; i++)
    {
        for(int j = 0; j < (*struct_item).col; j++)
        {
		//if first column ignore the value before if not enter if
            	if(j != 0)
		{
			(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j-1])*((*struct_item).mask[0]);
		}
		//if first column ignore the value before if not enter if
		if(j != (*struct_item).col-1)
		{
			(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j+1])*((*struct_item).mask[2]);
		}
		//middle element value
		(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j])*((*struct_item).mask[1]);
		(*struct_item).num_convo++; // increase number of convolution
        }
    }

    return NULL;
}
//============================================================================================================================================================
//checking function to check if code is run properly by stating name of file
int Check(int argc, char *argv[])
{
	if(argc > 2)
	{
		printf("Error too many arguments. \nhow to run: ./one_thread MxN.txt \n");
		return 0;
	}
	else if(argc < 2)
	{
		printf("Error expected one more argument \nhow to run: ./one_thread MxN.txt \n");
		return 0;
	}
    	return 1;//if everything checks well return 1 as in true
}
//============================================================================================================================================================
//main function
int main(int argc, char *argv[])
{
    	//read file
	int check = Check(argc, argv);//Check() function
	
	// Check function return 0, exit program
	if(check == 0)
	{
		return 0;
	}
	//open file
    	char *p = (char*)malloc(sizeof(char)*1);//saves file name in heap
    	p = argv[1];
    	printf("Opening %s...\n", p);//print we are opening file number given
	
	struct Convo *convo = malloc(sizeof(struct Convo));
    	int row, col, mask_row, mask_col;
//----------------------------------------------------------------------------------------------------------------------------------
	FILE *openfile = fopen(p, "r");

	//if fail to open file
	if(openfile == NULL)
	{
		printf("File does not exist. Please use a proper data file and include it in the same directory as the program.\n");
		fclose(openfile);
		free(p);
		return 0;
    	}
	//success in opening file
	else
	{
		int i,j;
		fscanf(openfile, "%d", &row); //get row
		fscanf(openfile, "%d", &col); //get column
		

		//initialize array for both original matrix and container to store convolutioned matrix
		(*convo).arr_before = malloc(sizeof(int*)*row);
		(*convo).arr_after = malloc(sizeof(int*)*row);
		for(i = 0; i < row; i++)
		{
			(*convo).arr_before[i] = malloc(sizeof(int)*col);
			(*convo).arr_after[i] = malloc(sizeof(int)*col);
		}

		//read matrix in file
		for(i = 0; i < row; i++)
		{
			for(j = 0; j < col; j++)
			{
				fscanf(openfile, "%d", &(*convo).arr_before[i][j]);
			}
		}

		fscanf(openfile, "%d", &mask_row);
		fscanf(openfile, "%d", &mask_col);

		(*convo).mask = malloc(sizeof(int)*mask_col);

		//get masking values
		fscanf(openfile, "%d", &(*convo).mask[0]);
		fscanf(openfile, "%d", &(*convo).mask[1]);
		fscanf(openfile, "%d", &(*convo).mask[2]);

		//close file
		fclose(openfile);
	}
    	//declaration
    	(*convo).row = row;
    	(*convo).col = col;
    	pthread_t thread;

	//start time
	clock_t begin = clock();

    	//create thread
    	pthread_create(&thread, NULL, Convolution, (void *)convo);

	//join thread
    	pthread_join(thread, NULL);
    	//end clock
	clock_t end = clock();
    	
	//print convoluted result, time and number of convolutions
    	printf("Convoluted Array\n");
    	for(int m = 0; m < row; m++)
	{
		for(int n = 0; n < col; n++)
		{
			printf("%d ", (*convo).arr_after[m][n]);
		}
		printf("\n");
	}
	double total_time = (double)(end-begin)/ CLOCKS_PER_SEC;
	printf("Total time: %lf seconds\n", total_time);
	printf("Total number of convolution operations performed: %d\n", (*convo).num_convo);

	return 0;
}
