#ifndef VALIDATION_H
#define VALIDATION_H

#include <stddef.h>

int isAllLowercase(char *str);
int isValidUsername(char *username, size_t len);
int isValidPassword(char *password, size_t len);

#endif