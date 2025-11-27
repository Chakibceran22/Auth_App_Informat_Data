#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <openssl/sha.h>
#include <unistd.h>

char PASSWORD_FILE[120] = "password.txt";
char BANNED_FILE[120] = "banned.txt";

void generateSalt(char *salt)
{
    sprintf(salt, "%05d", rand() % 100000); // Generates 00000-99999
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
int checkIfUsernameExists(const char *username)
{
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL)
    {
        return 0; // File doesn't exist, so username can't exist
    }

    char line[200];
    while (fgets(line, sizeof(line), file))
    {
        char storedUsername[10];
        sscanf(line, "%9[^:]", storedUsername);
        if (strcmp(storedUsername, username) == 0)
        {
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
    if (checkIfUsernameExists(username))
    {
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

int checkIfUserIsBanned(const char *username)
{
    FILE *file = fopen(BANNED_FILE, "r");
    if (file == NULL)
    {
        return 0; // File doesn't exist, so user can't be banned
    }

    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        if (strcmp(line, username) == 0)
        {
            fclose(file);
            return 1; // User is banned
        }
    }

    fclose(file);
    return 0; // User is not banned
}

void hashPassword(const char *password, const char *salt, char *hashedOutput)
{
    // Concatenate password + salt
    char passwordWithSalt[200];
    sprintf(passwordWithSalt, "%s%s", password, salt);

    // Hash with SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)passwordWithSalt, strlen(passwordWithSalt), hash);

    // Convert to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(hashedOutput + (i * 2), "%02x", hash[i]);
    }
    hashedOutput[64] = '\0';
}

int getUserCredentials(const char *username, char *storedHash, char *storedSalt) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        return 0; // File doesn't exist
    }
    
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char storedUsername[10];
        char tempHash[65];
        char tempSalt[6];
        
        // Parse the line: username:hash:salt
        if (sscanf(line, "%9[^:]:%64[^:]:%5s", storedUsername, tempHash, tempSalt) == 3) {
            if (strcmp(storedUsername, username) == 0) {
                // Found the user - copy hash and salt
                strcpy(storedHash, tempHash);
                strcpy(storedSalt, tempSalt);
                fclose(file);
                return 1; // Success
            }
        }
    }
    
    fclose(file);
    return 0; // Username not found
}


void banUser(const char *username)
{
    FILE *file = fopen(BANNED_FILE, "a");
    if (file == NULL)
    {
        printf("Error opening banned file.\n");
        return;
    }

    fprintf(file, "%s\n", username);
    fclose(file);
    printf("User '%s' has been banned.\n", username);
}

void signUp()
{
    printf("\n=== SIGNUP ===");
    char username[10];
    char password[10];
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
            // sanitizing the input and taking off the newline character
            username[len - 1] = '\0';
            len--;
        }
        else
        {
            // clearing buffer if the characters are too long to not retrigger the fgets
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

void signIn()
{
    printf("\n=== SIGNIN ===");
    char username[10];
    char password[10];
    char salt[6];
    while (1)
    {
        printf("\n Username: ");
        if (fgets(username, sizeof(username), stdin) == NULL)
        {
            printf("Error reading username.\n");
            continue;
        }
        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n')
        {
            // sanitizing the input and taking off the newline character
            username[len - 1] = '\0';
            len--;
        }
        else
        {
            // clearing buffer if the characters are too long to not retrigger the fgets
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        if (!checkIfUsernameExists(username))
        {
            printf(" Username does not exist. Please try again.\n");
            continue;
        }
        if (checkIfUserIsBanned(username))
        {
            printf(" This user is banned. Access denied.\n");
            return;
        }

        int timeouts[4] = {5, 10, 15, 20};

        int timeoutIndex = 0;
        while (1)
        {
            int attempts = 1;
            while (attempts < 4)
            {
                printf(" Attempt %d - Password: ", attempts);
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
                char storedHash[65];
                getUserCredentials(username, storedHash, salt);
                char hashedInput[65];
                hashPassword(password, salt, hashedInput);
                if (strcmp(hashedInput, storedHash) == 0)
                {
                    printf(" Authentication successful! Welcome, %s.\n", username);
                    return;
                }
                else
                {
                    printf(" Incorrect password.\n");
                    attempts++;
                }

            }
            
            if (timeoutIndex > 3) {
                printf(" Too many failed attempts. User '%s' is now banned.\n", username);
                banUser(username);
                return;
            }
            else
            {
                int timeout = timeouts[timeoutIndex];
                timeoutIndex++;
                printf(" Too many failed attempts. Please wait %d seconds before trying again.\n", timeout);
                sleep(timeout);
            }
        }

        break;
    }
}

int main()
{
    srand(time(NULL)); // for creating the raond seed
    while (1)
    {
        printf("\n=== AUTHENTICATION APP ===\n");
        printf("1. Sign Up\n");
        printf("2. Sign In\n");
        printf("3. Exit\n");
        printf("Choose an option: ");

        int choice;
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n')
                ; // Clear invalid input
            continue;
        }
        while (getchar() != '\n')
            ; // Clear newline character from input buffer

        switch (choice)
        {
        case 1:
            signUp();
            break;
        case 2:
            signIn();
            break;
        case 3:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}