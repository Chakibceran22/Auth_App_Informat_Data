# Guide
this is a simple project for an auth app asked by our teacher and it was implmeented in python now we need it in C

## Running the programme 
```bash
# we use this commande to generate an output fie named app.exe and linking to open ssl lib for the hashing function
gcc main.c auth.c utils.c validation.c -o app -lssl -lcrypto 
```

## Main Conditins:
- usename must be 5 lowercase characters only
- password must be 8 characters or digits from 0-9
- output saved as follows: **username**:**password**:**salt**,  in a file named password.txt

## Disclamer 
you are free to use this code for the homewrk but change some small details  