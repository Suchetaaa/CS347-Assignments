#include "alloc.h"

/* Code to allocate page of 4KB size with mmap() call and
* initialization of other necessary data structures.
* return 0 on success and 1 on failure (e.g if mmap() fails)
*/

class Node
{
public:
	int size;
	bool used;
};

char *region;
Node array[4096];

int init()
{
	// Write your code below
	region = (char*)mmap(
    NULL,   // Map from the start of the 2^20th page
    4096,                         // for one page length
    PROT_READ|PROT_WRITE|PROT_EXEC,
    MAP_ANON|MAP_PRIVATE, 0, 0            // to a private block of hardware memory
	);
	if (region == MAP_FAILED) {
	   perror("Could not mmap");
	   return 1;
	 }
	else 
		{
			printf("Hello\n");

			for (int i = 0; i < 4096; i++)
			{
				array[i].size = 1;
				array[i].used = 0;
			}
			
		}
}

/* optional cleanup with munmap() call
* return 0 on success and 1 on failure (if munmap() fails)
*/
int cleanup()
{

	// Write your code below
	int check_cleanup = munmap(region, 4096);
	if (check_cleanup != 0)
	{
		perror("Could not clean");
		return -1;
	}

	else
	{
		printf("Yayay\n");
		return 0;
	}



  
}

/* Function to allocate memory of given size
* argument: bufSize - size of the buffer
* return value: on success - returns pointer to starting address of allocated memory
*               on failure (not able to allocate) - returns NULL
*/
char *alloc(int bufSize)
{
	// write your code below
	int iter = 0;
	bool allocation = 0;
	int i;

	//printf("Hi1\n");

	while(allocation != 1 && iter < 4096)
	{
		if (array[iter].used == 0)
		{
			int size_upto_now = 1;
			for (i = iter+1; i < 4096; i++)
			{
				if (array[i].used == 0)
				{
					size_upto_now += 1;
				}

				else break;
			}

			//printf("%d\n",size_upto_now );
			if (size_upto_now >= bufSize) allocation = 1;
			else 
			{
				iter = i;
			}
		}

		else
		{
			iter = iter + 1;
		}

	}

	if (allocation == 1) 
		{
			for (int j = iter+1; j < iter + bufSize; j++)
			{
				array[j].used = 1;
				array[j].size = 1;
			}

			array[iter].size = bufSize;
			array[iter].used = 1;

			printf("%d\n",iter );
			return region+iter;

		}
	else printf("Sorrryyy\n");
  
}


/* Function to free the memory
* argument: takes the starting address of an allocated buffer
*/
void dealloc(char *memAddr)
{
	// write your code below
	int free_iter = (int)(memAddr - region);
	printf("%d\n",free_iter );

	int sizee = array[free_iter].size;

	for (int i = free_iter; i < free_iter + sizee; i++)
	{
		array[i].size = 1;
		array[i].used = 0;
	}

}

// int main(int argc, char const *argv[])
// {
// 	init();
// 	char* return_val = alloc(8);

// 	printf("%p\n",return_val );


// 	char* return_val2 = alloc(16);
	
// 	printf("%p\n",return_val2 );

// 	char* return_val3 = alloc(16);
	
// 	printf("%p\n",return_val3 );

// 	dealloc(return_val2);

// 	char* tp = alloc(8);

// 	printf("%p\n",tp );
// 	return 0;
// }