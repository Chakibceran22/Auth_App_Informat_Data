#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>   
#include <openssl/sha.h>

char PASSWORD_FILE[120] = "password.txt";
char BANNED_FILE[120] = "banned.txt";

void generateSalt(char *salt) {
    sprintf(salt, "%05d", rand() % 100000);  // Generates 00000-99999
}
int isAllLowercase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!islower(str[i]))
        {
            return 0;
        }
    }
    return 1;
}
int checkIfUsernameExists(const char *username) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        return 0; // File doesn't exist, so username can't exist
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char storedUsername[10];
        sscanf(line, "%9[^:]", storedUsername);
        if (strcmp(storedUsername, username) == 0) {
            fclose(file);
            return 1; // Username found
        }
    }

    fclose(file);
    return 0; // Username not found
}

int isValidUsername(char *username, size_t len)
{
    if (len != 5)
    {
        printf(" Username too small or long. Please try again.\n");
        return 0;
    }

    if (!isAllLowercase(username))
    {
        printf(" Username must contain all lowercase.\n");
        return 0;
    }
    if(checkIfUsernameExists(username)) {
        printf(" Username already exists. Please choose another one.\n");
        return 0;
    }
    return 1;
}

int isValidPassword(char *password, size_t len)
{
    if (len != 8)
    {
        printf(" Password must be exactly 8 characters.\n");
        return 0;
    }
    return 1;
}

void hashPassword(const char *password, const char *salt, char *hashedOutput) {
    // Concatenate password + salt
    char passwordWithSalt[200];
    sprintf(passwordWithSalt, "%s%s", password, salt);
    
    // Hash with SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)passwordWithSalt, strlen(passwordWithSalt), hash);
    
    // Convert to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hashedOutput + (i * 2), "%02x", hash[i]);
    }
    hashedOutput[64] = '\0';
}

void signUp()
{
    printf("\n=== SIGNUP ===");
    char username[10];
    char password[90];
    char salt[6];

    while (1)
    {
        printf("\n Username (5 lowercase characters only): ");

        if (fgets(username, sizeof(username), stdin) == NULL)
        {
            printf("Error reading username.\n");
            continue;
        }

        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n')
        {
            //sanitizing the input and taking off the newline character
            username[len - 1] = '\0';
            len--;
        }
        else
        {
            //clearing buffer if the characters are too long to not retrigger the fgets
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (!isValidUsername(username, len))
            continue;

        break;
    }

    while (1)
    {
        printf(" Password (8 characters ): ");
        if (fgets(password, sizeof(password), stdin) == NULL)
        {
            printf("Error reading password.\n");
            continue;
        }
        size_t len = strlen(password);
        if (len > 0 && password[len - 1] == '\n')
        {
            password[len - 1] = '\0';
            len--;
        }
        else
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (!isValidPassword(password, len))
            continue;

        break;
    }
    generateSalt(salt);
    char hashedPassword[65];
    hashPassword(password, salt, hashedPassword);

    
    FILE *file = fopen(PASSWORD_FILE, "a");
    if (file == NULL)
    {
        printf("Error opening password file.\n");
        return;
    }

    fprintf(file, "%s:%s:%s\n", username, hashedPassword, salt);
    fclose(file);
    printf("Username '%s' saved to file!\n", username);
}


void signIn() {

}

int main()
{
    srand(time(NULL)); //for creating the raond seed
    signUp();
    return 0;
}