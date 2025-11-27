#include <stdio.h>
#include <ctype.h>
#include "validation.h"
#include "auth.h"

int isAllLowercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!islower(str[i])) {
            return 0;
        }
    }
    return 1;
}

int isValidUsername(char *username, size_t len) {
    if (len != 5) {
        printf(" Username too small or long. Please try again.\n");
        return 0;
    }

    if (!isAllLowercase(username)) {
        printf(" Username must contain all lowercase.\n");
        return 0;
    }
    
    if (checkIfUsernameExists(username)) {
        printf(" Username already exists. Please choose another one.\n");
        return 0;
    }
    
    return 1;
}

int isValidPassword(char *password, size_t len) {
    if (len != 8) {
        printf(" Password must be exactly 8 characters.\n");
        return 0;
    }
    return 1;
}