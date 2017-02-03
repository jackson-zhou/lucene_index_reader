#pragma once
#include <stdlib.h>
#include "reader.h"
// return numFields
int read_tim(const char* indexPath);
// return numFields
int read_tim_with_fp(FILE* termsIn,FILE* indexIn,int f_start,int f_len);
