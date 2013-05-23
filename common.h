#ifndef __MY_OPT_COMMON_H
#define __MY_OPT_COMMON_H

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

#define MAX_OPTS 256
#define MAX_LEN_STR 256
#define OR_MORE -1

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>

#endif /* __MY_OPT_COMMON_H */
