#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//============================================================================================================================================================
//part 1-2
struct Location{
	int i;
};

struct Convo{
	int **arr_before;
	int **arr_after;
	int *mask;
	int col;
	struct Location *locate;
};
void *Convolution(void *item)
{
    	struct Convo *struct_item = (struct Convo*)item;
    	int i = (*struct_item).locate->i;
    	
	for(int j = 0; j < (*struct_item).col; j++)
	{
		if(j != 0)
		{
			(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j-1])*((*struct_item).mask[0]);
		}
		if(j != (*struct_item).col-1)
		{
			(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j+1])*((*struct_item).mask[2]);
		}
		(*struct_item).arr_after[i][j] += ((*struct_item).arr_before[i][j])*((*struct_item).mask[1]);
	}
	
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
int main(int argc, char*argv[])
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
	
	struct Convo *convo = malloc(sizeof(struct Convo));
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
//----------------------------------------------------------------------------------------------------------------------------------
    	//declaration
    	(*convo).col = col;
    	pthread_t thread[row];
	int create_count = 0;
//----------------------------------------------------------------------------------------------------------------------------------

    	//create threads
	for(int i = 0; i < row; i++)
	{
		(*convo).locate = malloc(sizeof(struct Location));
		(*convo).locate->i = i;
		pthread_create(&thread[create_count], NULL, Convolution, (void *)convo);
		pthread_join(thread[create_count], NULL);
		create_count++;
	}
	
	
//----------------------------------------------------------------------------------------------------------------------------------
    	/*void *sum = NULL;
    	create_count = 0;
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			
			//arr_after[i][j] = *((int*)sum);
			create_count++;

		}
	}*/
//----------------------------------------------------------------------------------------------------------------------------------   
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			printf("%d ", (*convo).arr_after[i][j]);
		}
		printf("\n");
	}
	
	return 0;

}
