import hashlib
import random
import time
import os

PASSWORD_FILE = "password.txt"
BANNED_FILE = "banned.txt"

def signup():
    print("\n=== SIGNUP ===")

    while True:
        username = input("Username (min 5 lowercase characters): ")
        if len(username) >= 5 and username.islower() and username.isalpha():
            break
        print("Invalid username! Must be at least 5 lowercase letters.")

    while True:
        password = input("Password (min 8 chars: lowercase, uppercase, digits): ")
        if len(password) >= 8:
            has_lower = any(c.islower() for c in password)
            has_upper = any(c.isupper() for c in password)
            has_digit = any(c.isdigit() for c in password)
            if has_lower and has_upper and has_digit:
                break
        print("Invalid password! Must be 8+ chars with lowercase, uppercase, and digits.")

    salt = ''.join([str(random.randint(0, 9)) for _ in range(5)])

    salted_password = password + salt
    hashed_password = hashlib.sha256(salted_password.encode()).hexdigest()

    with open(PASSWORD_FILE, 'a') as f:
        f.write(f"{username}:{hashed_password}:{salt}\n")

    print(f"User '{username}' registered successfully!")

def signin():
    print("\n=== SIGNIN ===")

    username = input("Username: ")

    if os.path.exists(BANNED_FILE):
        with open(BANNED_FILE, 'r') as f:
            banned_users = [line.strip() for line in f]
            if username in banned_users:
                print("Account BANNED! Too many failed attempts.")
                return

    if not os.path.exists(PASSWORD_FILE):
        print("No users registered yet!")
        return

    with open(PASSWORD_FILE, 'r') as f:
        users = {}
        for line in f:
            parts = line.strip().split(':')
            if len(parts) == 3:
                users[parts[0]] = {'hash': parts[1], 'salt': parts[2]}

    if username not in users:
        print("User not found!")
        return

    stored_hash = users[username]['hash']
    salt = users[username]['salt']

    attempt_count = 0
    lockout_delays = [5, 10, 15, 20]
    lockout_index = 0

    while True:
        password = input("Password: ")
        salted_password = password + salt
        hashed_input = hashlib.sha256(salted_password.encode()).hexdigest()

        if hashed_input == stored_hash:
            print(f"Welcome {username}!")
            return

        attempt_count += 1
        print(f"Incorrect password! Attempt {attempt_count}/3")

        if attempt_count == 3:
            if lockout_index >= len(lockout_delays):
                print("Account BANNED! Too many failed attempts.")
                with open(BANNED_FILE, 'a') as f:
                    f.write(f"{username}\n")
                return

            delay = lockout_delays[lockout_index]
            print(f"Too many failed attempts! Locked out for {delay} seconds...")
            time.sleep(delay)

            lockout_index += 1
            attempt_count = 0
            print("You can try again now.")

def main():
    print("=== AUTHENTICATION SYSTEM ===")

    while True:
        print("\n1. Signup")
        print("2. Signin")
        print("3. Exit")

        choice = input("Choose an option (1/2/3): ")

        if choice == '1':
            signup()
        elif choice == '2':
            signin()
        elif choice == '3':
            print("Goodbye!")
            break
        else:
            print("Invalid choice! Please enter 1, 2, or 3.")

if __name__ == "__main__":
    main()
