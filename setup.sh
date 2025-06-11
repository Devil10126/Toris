#!/data/data/com.termux/files/usr/bin/bash

echo -e "\033[1;36m[*] Starting HTTPS DDoS Tool Setup...\033[0m"

# Update and upgrade packages
termux-change-repo
pkg update -y && pkg upgrade -y

# Install required dependencies
echo -e "\033[1;32m[+] Installing dependencies...\033[0m"
pkg install -y clang libcurl curl tsocks || {
    echo -e "\033[1;31m[!] Dependency installation failed. Retrying...\033[0m"
    sleep 2
    pkg install -y clang libcurl curl tsocks
}

# Final instructions
echo -e "\033[1;32m[✔] Setup complete.\033[0m"
echo -e "\033[1;34mTo compile:\033[0m clang++ DDoS_https.cpp -o DDoS_https -lcurl -lpthread"
echo -e "\033[1;34mTo run:\033[0m ./DDoS_https"
