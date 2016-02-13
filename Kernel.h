#ifndef KERNEL_H
#define KERNEL_H

#include <assert.h>

#define PUBLIC
#define PRIVATE						static
#define SIZE_T						size_t
#define CONST						const
#define BOOL						int
#define FALSE						0
#define TRUE						(!FALSE)

#define ASSERT(x)					assert(x)
#define FREE(x)						free(x);x=NULL 

#define IS_MULTIPLE_OF_2(x)			(((x)%2)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_3(x)			(((x)%3)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_4(x)			(((x)%4)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_5(x)			(((x)%5)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_6(x)			(((x)%6)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_7(x)			(((x)%7)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_8(x)			(((x)%8)==0 ? TRUE : FALSE)
#define IS_MULTIPLE_OF_9(x)			(((x)%9)==0 ? TRUE : FALSE)

typedef enum {
	
	SUCCESS = 0,
	ERROR	= 1,

} ERROR_CHECK;

#endif /* KERNEL_H */
