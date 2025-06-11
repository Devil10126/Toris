#!/data/data/com.termux/files/usr/bin/bash

echo -e "\033[1;36m[*] Starting DDoS Tool Setup...\033[0m"

# Fix repository issues
termux-change-repo

# Update system
pkg update -y && pkg upgrade -y

# Install required dependencies
echo -e "\033[1;32m[+] Installing dependencies...\033[0m"
pkg install -y tor tsocks g++ net-tools iproute2 netcat jq || {
    echo -e "\033[1;31m[!] Dependency installation failed. Retrying...\033[0m"
    sleep 2
    pkg install -y tor tsocks g++ net-tools iproute2 netcat jq
}

# Configure tsocks
echo -e "\033[1;36m[*] Configuring tsocks...\033[0m"
mkdir -p ~/.termux
cat > ~/.tsocks.conf <<EOF
server = 127.0.0.1
server_type = 5
server_port = 9050
EOF

# Start Tor service
echo -e "\033[1;32m[+] Starting Tor service...\033[0m"
(pkill tor || true) && tor &

# Show optional torrc config
echo -e "\033[1;33m[!] Optional: For identity rotation, edit ~/.torrc and enable ControlPort & password.\033[0m"
echo -e "\033[1;33m    Add this to ~/.torrc:\033[0m"
echo "ControlPort 9051"
echo "HashedControlPassword <your-hashed-password>"

# Final instructions
echo -e "\033[1;32m[✔] Setup complete.\033[0m"
echo -e "\033[1;34mCompile: g++ cyber_stress_tor.cpp -o cyber_stress_tor -lpthread\033[0m"
echo -e "\033[1;34mRun: tsocks ./cyber_stress_tor\033[0m"
