#ifndef SRC_STR_CUT_H_
#define SRC_STR_CUT_H_

#include "Configuracion.h"

void str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return;
}

#endif /* SRC_STR_CUT_H_ */
