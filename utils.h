#ifndef UTILS_H
#define UTILS_H

void generateSalt(char *salt);
void hashPassword(const char *password, const char *salt, char *hashedOutput);
void sleep_seconds(int seconds);

#endif