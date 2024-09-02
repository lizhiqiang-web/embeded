#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H
extern"C"{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FPATH ""
typedef struct people{
   char nmae[10];
   char tele_num[18];
   char id[15];
}people;

bool file_add(people p);
bool file_del(int k);
people file_fine(int k);
bool file_check(people k);
}

#endif // FILE_OPERATION_H
