#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "utils.h"

void sleep_seconds(int seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);  // Windows: milliseconds
    #else
        sleep(seconds);         // Unix: seconds
    #endif
}

void generateSalt(char *salt) {
    sprintf(salt, "%05d", rand() % 100000);
}

void hashPassword(const char *password, const char *salt, char *hashedOutput) {
    char passwordWithSalt[200];
    sprintf(passwordWithSalt, "%s%s", password, salt);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)passwordWithSalt, strlen(passwordWithSalt), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashedOutput + (i * 2), "%02x", hash[i]);
    }
    hashedOutput[64] = '\0';
}