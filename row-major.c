#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int search_matrix(int *matrix, int rows, int cols, int target, int row_major)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int index = row_major ? i * cols + j : j * rows + i;
			if (matrix[index] == target)
			{
				printf("Found %d at position [%d][%d]\n", target, i, j);
				return 1;
			}
		}
	}
	printf("%d not found in matrix.\n", target);
	return 0;
}

int main()
{
	int rows = 3;
	int cols = 3;

	int *matrix = malloc(rows * cols * sizeof(int));

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			matrix[i * cols + j] = i * cols + j + 1;
		}
	}

	printf("Matrix (row-major):\n");
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			printf("%2d ", matrix[i * cols + j]);
		}
		printf("\n");
	}

	srand(time(NULL));
	int target = rand() % (rows * cols) + 1;

	search_matrix(matrix, rows, cols, target, 1);

	free(matrix);
}
