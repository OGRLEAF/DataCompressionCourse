#include<stdlib.h>
#include<stdio.h>

#define ASSERT(CMP, MSG) if(!CMP) {printf("[ASSERT FAILED] %s\r\nexited\r\n", MSG); exit(1);}

#define ASSERT_EQUAL(X, Y, MSG) ASSERT(X==Y, MSG)