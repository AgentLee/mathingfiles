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

#define FUSE_USE_VERSION 30

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <fuse/fuse.h>
#include <math.h>

typedef struct pathInfo 
{
	char *name;
	char *doc_desc;
	int arg;
	char *(*f)();
} pathInfo;

const char *add_desc = "Adds 2 numbers together.\n";
const char *sub_desc = "Subtracts 2 numbers together.\n";
const char *mul_desc = "Multiplies 2 numbers together.\n";
const char *div_desc = "Divides 2 numbers together.\n";
const char *exp_desc = "Takes the exponent.\n";
const char *fib_desc = "Finds the Fibonacci sequence.\n";
const char *fact_desc = "Finds prime factors.\n";

char *add(char *a, char *b);
char *sub(char *a, char *b);
char *mul(char *a, char *b);
char *divi(char *a, char *b);
char *expo(char *a, char *b);
char *fib(char *a, char *b);
char *factor(char *a, char *b);

char answer[2048];
static time_t timestamp = 0;

const pathInfo pathMath[] = {
	{"add", "adding\n", 3, add},
	{"sub", "subtracting\n", 3, sub},
	{"mul", "multiplying\n", 3, mul},
	{"div", "dividing\n", 3, divi},
	{"exp", "exponentialing\n", 3, expo},
	{"fib", "fibonacciing\n", 2, fib},
	{"factor", "factorialing\n", 2, factor},
	{0, 0, 0, 0}
};

// FUSE function implementations. 
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

	while(pathMath[j].name != 0)
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
	} else if(strcmp(path, "/add") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/sub") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/mul") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/div") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/fib") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/exp") == 0) {
		filler(buf, "doc", NULL, 0);
	} else if(strcmp(path, "/factor") == 0) {
		filler(buf, "doc", NULL, 0);
	} else {
		return -ENOENT;
	}

	return 0;
}			

static int mathfs_open(const char *path, struct fuse_file_info *fi)
{
	char pathcpy[100];
	char *split[2048];
	int i = 0;
	int j = 0;

	memcpy(pathcpy, path, strlen(path) + 1);

	split[i] = strtok(pathcpy, "/");
	if(split[i] == NULL)
	{
		split[i] = "/";
		i++;
	}

	while(split[i] != NULL)
	{
		i++;
		split[i] = strtok(NULL, "/");
	}

	while(pathMath[j].name != 0)
	{
		if(strcmp(split[0], pathMath[j].name) == 0)
		{
			if(strcmp(split[1], "doc") == 0)
			{
				if((fi->flags & 3) != O_RDONLY)
					return -EACCES;
			} else if(i == pathMath[j].arg) {
				if((fi->flags & 3) != O_RDONLY)
					return -EACCES;
			}

			break;
		}
		j++;
	}

	if(pathMath[j].name == 0)
		return -ENOENT;

	return 0;
}

static int mathfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    (void) fi;
	size_t length;
	
	char pathcpy[100];
	char *split[2048];
	int i = 0;
	int j = 0;
	
	memcpy(pathcpy, path, strlen(path) + 1);

	split[i] = strtok(pathcpy, "/");
	if(split[i] == NULL)
	{
		split[i] = "/";
		i++;
	}

	while(split[i] != NULL)
	{
		i++;
		split[i] = strtok(NULL, "/");
	}

	while(pathMath[j].name != 0)
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
			} else if(i == pathMath[j].arg) {
				length = strlen(pathMath[j].f(split[i-2], split[i-1]));

				if(offset < length)
				{
					if(offset + size > length)
						size = length - offset;
			
					memcpy(buf, pathMath[j].f(split[i-2], split[i-1]) + offset, size);
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

	return 0;
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

/***** MATH FUNCTIONS *****/
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
	sprintf(answer, "%f\n", x+y);

	return answer;	
}

char *sub(char *a, char *b)
{
	memset(answer, 0, 2048);

	double x = atof(a);
	double y = atof(b);

	sprintf(answer, "%f\n", x-y);

	return answer;
	
}
char *mul(char *a, char *b)
{
	memset(answer, 0, 2048);

	double x = atof(a);
	double y = atof(b);

	sprintf(answer, "%f\n", x*y);

	return answer;	
}

char *divi(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	if(strcmp(b, "0") == 0)
	{
		return "DIVIDE BY 0 ERROR\n";
	}

	double x = atof(a);
	double y = atof(b);
	
	sprintf(answer, "%f\n", x/y);

	return answer;
}

char *expo(char *a, char *b)
{
	memset(answer, 0, 2048);
	
	double x = atof(a);
	double y = atof(b);

	sprintf(answer, "%f\n", pow(x, y));

	return answer;	
}

long long prime(long long x)
{
	int i;
	int count = 0;
	long long limit = (long long)ceil(sqrt((double)x));

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
	long long i;
	long long x;
	long long iLimit;
	double limit = atof(b);
	
	strcpy(answer, "");

	if(fabs(limit - floor(limit)) > .001)
	{
		return "VALUE MUST BE AN INT\n";
	}	

	x = (long long)limit;
	iLimit = x;

	if(prime(x))
	{
		sprintf(factorial, "%lld\n", x);
		strcat(answer, factorial);

		return answer;
	}

	if(x % 2 == 0)
	{
		sprintf(factorial, "%i\n", 2);
		strcat(answer, factorial);
	}

	for(i = 3; i <= iLimit; i += 2)
	{
		if(prime(i) == 1)
		{
			if(x % i == 0)
			{
				iLimit = iLimit/i;
			
				sprintf(factorial, "%lld\n", i);
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
	int i = 2;
	long long j = 1;
	long long k = 1;
	long long fib;
	double limit = atof(b);

	strcpy(answer, "1\n1\n");

	if(fabs(limit-floor(limit)) > 0.001)
	{
		return "VALUE MUST BE INT\n";
	}

	while(i < limit)
	{
		fib = j + k;

		sprintf(fibonacci, "%lld\n", fib);
		strcat(answer, fibonacci);

		k = j;
		j = fib;
		i++;
	}

	return answer;	
}
