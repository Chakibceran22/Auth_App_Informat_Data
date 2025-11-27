#ifndef AUTH_H
#define AUTH_H

void signUp();
void signIn();
void banUser(const char *username);
int checkIfUsernameExists(const char *username);
int checkIfUserIsBanned(const char *username);
int getUserCredentials(const char *username, char *storedHash, char *storedSalt);

extern char PASSWORD_FILE[120];
extern char BANNED_FILE[120];

#endif