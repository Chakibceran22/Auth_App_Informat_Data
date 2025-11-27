#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "auth.h"
#include "utils.h"
#include "validation.h"

char PASSWORD_FILE[120] = "password.txt";
char BANNED_FILE[120] = "banned.txt";

int checkIfUsernameExists(const char *username) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        return 0;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char storedUsername[10];
        sscanf(line, "%9[^:]", storedUsername);
        if (strcmp(storedUsername, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int checkIfUserIsBanned(const char *username) {
    FILE *file = fopen(BANNED_FILE, "r");
    if (file == NULL) {
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int getUserCredentials(const char *username, char *storedHash, char *storedSalt) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char storedUsername[10];
        char tempHash[65];
        char tempSalt[6];
        
        if (sscanf(line, "%9[^:]:%64[^:]:%5s", storedUsername, tempHash, tempSalt) == 3) {
            if (strcmp(storedUsername, username) == 0) {
                strcpy(storedHash, tempHash);
                strcpy(storedSalt, tempSalt);
                fclose(file);
                return 1;
            }
        }
    }
    
    fclose(file);
    return 0;
}

void banUser(const char *username) {
    FILE *file = fopen(BANNED_FILE, "a");
    if (file == NULL) {
        printf("Error opening banned file.\n");
        return;
    }

    fprintf(file, "%s\n", username);
    fclose(file);
    printf("User '%s' has been banned.\n", username);
}

void signUp() {
    printf("\n=== SIGNUP ===");
    char username[10];
    char password[10];
    char salt[6];

    while (1) {
        printf("\n Username (5 lowercase characters only): ");

        if (fgets(username, sizeof(username), stdin) == NULL) {
            printf("Error reading username.\n");
            continue;
        }

        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n') {
            username[len - 1] = '\0';
            len--;
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        
        if (!isValidUsername(username, len))
            continue;

        break;
    }

    while (1) {
        printf(" Password (8 characters): ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            printf("Error reading password.\n");
            continue;
        }
        size_t len = strlen(password);
        if (len > 0 && password[len - 1] == '\n') {
            password[len - 1] = '\0';
            len--;
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        
        if (!isValidPassword(password, len))
            continue;

        break;
    }
    
    generateSalt(salt);
    char hashedPassword[65];
    hashPassword(password, salt, hashedPassword);

    FILE *file = fopen(PASSWORD_FILE, "a");
    if (file == NULL) {
        printf("Error opening password file.\n");
        return;
    }

    fprintf(file, "%s:%s:%s\n", username, hashedPassword, salt);
    fclose(file);
    printf("Username '%s' saved to file!\n", username);
}

void signIn() {
    printf("\n=== SIGNIN ===");
    char username[10];
    char password[10];
    char salt[6];
    
    while (1) {
        printf("\n Username: ");
        if (fgets(username, sizeof(username), stdin) == NULL) {
            printf("Error reading username.\n");
            continue;
        }
        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n') {
            username[len - 1] = '\0';
            len--;
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        
        if (!checkIfUsernameExists(username)) {
            printf(" Username does not exist. Please try again.\n");
            continue;
        }
        
        if (checkIfUserIsBanned(username)) {
            printf(" This user is banned. Access denied.\n");
            return;
        }

        int timeouts[4] = {5, 10, 15, 20};
        int timeoutIndex = 0;
        
        while (1) {
            int attempts = 1;
            while (attempts < 4) {
                printf(" Attempt %d - Password: ", attempts);
                if (fgets(password, sizeof(password), stdin) == NULL) {
                    printf("Error reading password.\n");
                    continue;
                }
                size_t len = strlen(password);
                if (len > 0 && password[len - 1] == '\n') {
                    password[len - 1] = '\0';
                    len--;
                } else {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                }
                
                char storedHash[65];
                getUserCredentials(username, storedHash, salt);
                char hashedInput[65];
                hashPassword(password, salt, hashedInput);
                
                if (strcmp(hashedInput, storedHash) == 0) {
                    printf(" Authentication successful! Welcome, %s.\n", username);
                    return;
                } else {
                    printf(" Incorrect password.\n");
                    attempts++;
                }
            }
            
            if (timeoutIndex > 3) {
                printf(" Too many failed attempts. User '%s' is now banned.\n", username);
                banUser(username);
                return;
            } else {
                int timeout = timeouts[timeoutIndex];
                timeoutIndex++;
                printf(" Too many failed attempts. Please wait %d seconds before trying again.\n", timeout);
                sleep(timeout);
            }
        }

        break;
    }
}