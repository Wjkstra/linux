#ifndef _threadCheck_H_
#define _threadCheck_H_

#include <stdlib.h>
#include <stdio.h>

#define THREAD_ERR_CHECK(ret,msg) {if(ret != 0){fprintf(stderr,"%s:%s\n",msg,strerror(ret));}}

#endif

