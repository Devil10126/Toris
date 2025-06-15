#!/bin/bash

# Update and upgrade the system
echo "[+] Updating and upgrading system..."
pkg update && pkg upgrade -y

# Install dependencies for cURL, Tor, and other tools
echo "[+] Installing dependencies..."
pkg install -y curl tor git clang libcurl-dev libssl-dev

# Install additional dependencies if required for compiling
pkg install -y make build-essential

# Check if curl is installed
echo "[+] Checking if curl is installed..."
if ! command -v curl &> /dev/null
then
    echo "[!] curl is not installed, installing..."
    pkg install curl
fi

# Install Tor
echo "[+] Checking if Tor is installed..."
if ! command -v tor &> /dev/null
then
    echo "[!] Tor is not installed, installing..."
    pkg install tor
fi

# Start Tor service manually in Termux
echo "[+] Starting Tor..."
tor &

# Configure permissions for Termux if necessary
echo "[+] Configuring Termux environment..."
termux-setup-storage

echo "[✔] Setup completed! You can now run the DDoS tool."
