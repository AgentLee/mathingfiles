/****************************/
/**** CS416 Assignment 7 ****/
/****************************/
/****************************/
/*							*/
/*		Jonathan Lee		*/
/*		Niharika Nagar		*/
/*		Sehaj Singh			*/
/*							*/
/****************************/
/****************************/

#define FUSE_USE_VERSION 30

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <fuse/fuse.h>
#include <math.h>

/* Let's make it pretty */
#define RED 	"\x1b[31m"
#define GREEN	"\x1b[32m"
#define YELLOW 	"\x1b[33m"
#define BLUE	"\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN	"\x1b[36m"
#define RESET 	"\x1b[0m"

/*
 * Set a timestamp so the system can display the correct time and such 
 * It printed December 31, 1969 without it so...
 */
static time_t timestamp = 0;

/* Holds everything about the path together in a nice struct */
typedef struct pathInfo 
{
	char *name;
	char *doc_desc;
	int arg;
	char *(*f)();
} pathInfo;

/* Math Functions used later */
char *add(char *a, char *b);
char *sub(char *a, char *b);
char *mul(char *a, char *b);
char *divi(char *a, char *b);
char *expo(char *a, char *b);
char *fib(char *a, char *b);
char *factor(char *a, char *b);

/* Global variables */
char answer[2048];	// set as global so the math functions can work better
int parseSize; // used for our parsing function to keep track of the number of components 

/* Table of pathInfo's for the different operations */
const pathInfo pathMath[] = {
	{"add", "Adds 2 numbers.\nThe file add/a/b contains the sum a+b.\n", 3, add},
	{"sub", "Subtracts 2 numbers.\nThe file sub/a/b contains the difference a-b.\n", 3, sub},
	{"mul", "Multiplies 2 numbers.\nThe file mul/a/b contains the product a*b.\n", 3, mul},
	{"div", "Divides 2 numbers.\nThe file div/a/b contains the quotient a/b.\n", 3, divi},
	{"exp", "Raises a number to an exponent.\nThe file exp/a/b contains a raised to the power of b.\n", 3, expo},
	{"fib", "Gets the fibonacci sequence.\nThe file fib/n contains the first n fibonacci numbers.\n", 2, fib},
	{"factor", "Finds factors of a number.\nThe file factor/n contains the prime factors of n.\n", 2, factor},
};

/* FUSE function implementations */
static int mathfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	int i = 0;
	int j = 0;
	char pathcpy[100];
	char *split[2048];

	memset(stbuf, 0, sizeof(struct stat));
	memcpy(pathcpy, path, strlen(path)+1);

	split[i] = strtok(pathcpy, "/");

	stbuf->st_atime = timestamp;
	stbuf->st_mtime = timestamp;
	stbuf->st_ctime = timestamp;

	if(split[i] == NULL)
	{
		split[i] = "/";
		
		stbuf->st_mode = S_IFDIR | 0775;
		stbuf->st_nlink = 9;

		return 0;
	}

	while(split[i] != NULL)
	{
		i++;
		split[i] = strtok(NULL, "/");
	}

	while(j < 8)
	{
		if(strcmp(split[0], pathMath[j].name) == 0)
		{
			if(i == 1)
			{
				stbuf->st_mode = S_IFDIR | 0775;
				stbuf->st_nlink = 3;
	
				return 0;
			} else {
				if(strcmp(split[1], "doc") == 0)
				{
					stbuf->st_mode = S_IFREG | 0664;
					stbuf->st_nlink = 3;
					stbuf->st_size = strlen(pathMath[j].doc_desc);
				
					return 0;
				} else if(i == pathMath[j].arg) {
					stbuf->st_mode = S_IFREG | 0664;
					stbuf->st_nlink = 3;
					stbuf->st_size = strlen(pathMath[j].f(split[i-2], split[i-1]));

					return 0;
				} else if(i > pathMath[j].arg) {
					return -ENOENT;
				} else {
					stbuf->st_mode = S_IFDIR | 0775;
					stbuf->st_nlink = 3;
		
					return 0;
				}
			}	
		}		

		j++;
	}

	if(pathMath[j].name == 0)
		res = -ENOENT;
	
	return res;
}

static int mathfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
			struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	if(strcmp(path, "/") == 0)
	{
		filler(buf, "add", NULL, 0);
		filler(buf, "sub", NULL, 0);
		filler(buf, "mul", NULL, 0);
		filler(buf, "div", NULL, 0);
		filler(buf, "exp", NULL, 0);
		filler(buf, "fib", NULL, 0);
		filler(buf, "factor", NULL, 0);
	} else if(strcmp(path, "/add") == 0 || strcmp(path, "/sub") == 0 || strcmp(path, "/mul") == 0 || strcmp(path, "/div") == 0 || strcmp(path, "/fib") == 0 || strcmp(path, "/exp") == 0 || strcmp(path, "/factor") == 0)
	{
		filler(buf, "doc", NULL, 0);
	} else {
		return -ENOENT;
	}

	return 0;
}			


/*	goes through path and tokenizes it to get each component
 *	used for mathfs_open() & mathfs_read() */
char** parser(char** split, char* pathcpy){
 
  int i = 0;
  
  split[i] = strtok(pathcpy, "/");
  if(split[i] == NULL) {
    split[i] = "/";
    i++;
  }
  while(split[i] != NULL) {
    i++;
    split[i] = strtok(NULL, "/");
  }

  parseSize = i;
  return split;

}

static int mathfs_open(const char *path, struct fuse_file_info *fi)
{
	char pathcpy[100];
	memcpy(pathcpy, path, strlen(path) + 1);

	char **split = (char**)malloc(2048);
	parser(split, pathcpy);
	
	int j = 0;
	while(j < 8)
	{
		if(strcmp(split[0], pathMath[j].name) == 0)
		{
			if(strcmp(split[1], "doc") == 0)
			{
				if((fi->flags & 3) != O_RDONLY)
					return -EACCES;
			} else if(parseSize == pathMath[j].arg) {
				if((fi->flags & 3) != O_RDONLY)
					return -EACCES;
			}

			break;
		}
		j++;
	}

	if(pathMath[j].name == 0)
		return -ENOENT;
	free(split);
	return 0;
}


static int mathfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;
	size_t length;
	
	char pathcpy[100];
	memcpy(pathcpy, path, strlen(path) + 1);

	char **split=(char**)malloc(2048);
	split = parser(split, pathcpy);
	int j = 0;
	
	while(j < 8)
	{
		if(strcmp(split[0], pathMath[j].name) == 0)
		{
			if(strcmp(split[1], "doc") == 0)
			{
				length = strlen(pathMath[j].doc_desc);
				
				if(offset < length)
				{
					if(offset + size > length)
						size = length - offset;
					
					memcpy(buf, pathMath[j].doc_desc + offset, size);
				} else 	
					size = 0;

				return size;
			} else if(parseSize == pathMath[j].arg) {
				length = strlen(pathMath[j].f(split[parseSize-2], split[parseSize-1]));

				if(offset < length)
				{
					if(offset + size > length)
						size = length - offset;
			
					memcpy(buf, pathMath[j].f(split[parseSize-2], split[parseSize-1]) + offset, size);
				} else 
					size = 0;

				return size;
			}
			
			break;
		}		

		j++;
	}

	if(pathMath[j].name == 0)
		return -ENOENT;
	free(split);
	return 0;
}	



/* Math Operations */
int isDecimal(char *n)
{
	int i;
	for(i = 0; i < strlen(n); i++)
	{
		if(n[i] == '.')
		{
			return 1;
		}
	}

	return 0;
}

char *add(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	double x = atof(a);
	double y = atof(b);

	sprintf(answer, CYAN "%g\n" RESET, x+y);

	return answer;	
}

char *sub(char *a, char *b)
{
	memset(answer, 0, 2048);

	double x = atof(a);
	double y = atof(b);

	sprintf(answer, CYAN "%g\n" RESET, x-y);

	return answer;
	
}
char *mul(char *a, char *b)
{
	memset(answer, 0, 2048);

	double x = atof(a);
	double y = atof(b);

	sprintf(answer, CYAN "%g\n" RESET, x*y);

	return answer;	
}

char *divi(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	if(strcmp(b, "0") == 0)
	{
		return RED "DIVIDE BY 0 ERROR\n" RESET;
	}

	double x = atof(a);
	double y = atof(b);
	
	sprintf(answer,  CYAN "%g\n" RESET, x/y);

	return answer;
}

char *expo(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	double x = atof(a);
	double y = atof(b);

	sprintf(answer, CYAN "%g\n" RESET, pow(x, y));

	return answer;	
}

long long int prime(long long int x)
{
	int i;
	int count = 0;
	long long int limit = (long long int)ceil(sqrt((double)x));

	if(x == 1)
	{
		return 0;
	}

	for(i = 1; i <= limit; i++)
	{
		if(x % i == 0)
		{
			count++;
		}

		if(count >= 2)
		{
			return 0;
		}
	}

	return 1;
}

char *factor(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	char factorial[1024];
	long long int i;
	long long int x;
	long long int iLimit;
	double limit = atof(b);
	
	strcpy(answer, "");

	if(fabs(limit - floor(limit)) > .001)
	{
		return RED "VALUE MUST BE AN INT\n" RESET;
	}	

	x = (long long int)limit;
	iLimit = x;

	if(prime(x))
	{
		sprintf(factorial, CYAN "%lld\n" RESET, x);
		strcat(answer, factorial);

		return answer;
	}

	if(x % 2 == 0)
	{
		sprintf(factorial, CYAN "%i\n" RESET, 2);
		strcat(answer, factorial);
	}

	for(i = 3; i <= iLimit; i += 2)
	{
		if(prime(i) == 1)
		{
			if(x % i == 0)
			{
				iLimit = iLimit/i;
			
				sprintf(factorial, CYAN "%lld\n" RESET, i);
				strcat(answer, factorial);
			}
		}
	}

	return answer;	
}

char *fib(char *a, char *b)
{
	memset(answer, 0, 2048);

	char fibonacci[1024];
	
	long j = 1;
	long k = 1;
	long fib;
	double limit = atof(b);

	if(limit==0){
		strcpy(answer, CYAN "0\n" RESET);
		return answer;
	}

	if(limit ==1 ){
		strcpy(answer, CYAN "1\n" RESET);
		return answer;
	}

	if(limit < 0){
		return RED "VALUE MUST BE POSITIVE\n" RESET;
	}
	strcpy(answer, CYAN "1\n1\n" RESET);

	if(fabs(limit-floor(limit)) > 0.001)
	{
		return RED "VALUE MUST BE INT\n" RESET;
	}
	int i;
	for(i=2; i < limit; i++)
	{
		fib = j + k;
		j = k;
		k = fib;

		sprintf(fibonacci,  CYAN "%lld" RESET, k);
		if(i < (limit))
			strcat(fibonacci,"\n");

		strcat(answer, fibonacci);
	}

	return answer;	
}

static struct fuse_operations mathfs_oper = {
    .getattr = mathfs_getattr,
    .readdir = mathfs_readdir,
    .open = mathfs_open,
    .read = mathfs_read,
};

int main(int argc, char **argv)
{
	timestamp = time(NULL);

    return fuse_main(argc, argv, &mathfs_oper, NULL);
}