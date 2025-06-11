# Toris is a powerful DDoS tool, not to be used for illegal purposes


For installation in termux

pkg update
pkg upgrade 
pkg install git
pkg install clang 
pkg install tor

git clone https://github.com/Devil10126/Toris
cd Toris
chmod +x setup.sh
./setup.sh
g++ DDoS.cpp -o DDoS -lpthread
tsocks ./DDoS



