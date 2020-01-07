#include <iostream>
#include <fstream>
#include <locale>
#include <string>
#include <list>
#include <codecvt>
#include <thread>
#include <future>
#include <vector>

const int thread_num = 4;

int grep(std::string filename, std::wstring word) {
    std::locale loc("pl_PL.UTF-8");
    std::wfstream file(filename);
    file.imbue(loc);
    std::wstring line;
    unsigned int count = 0;
    while (getline(file, line)) {
        for (auto pos = line.find(word,0);
             pos != std::string::npos;
             pos = line.find(word, pos+1))
            count++;
    }
    return count;
}

void promised_grep(std::list<std::string>& filenames, std::wstring& word, std::promise<unsigned int>& word_promise) {
    int result = 0;
    for (auto file : filenames) {
        result += grep(file, word);
    }
    word_promise.set_value(result);
}

int main() {
    // std::ios::sync_with_stdio(false);
    std::locale loc("pl_PL.UTF-8");
    std::wcout.imbue(loc);
    std::wcin.imbue(loc);

    std::wstring word;
    std::getline(std::wcin, word);

    std::wstring s_file_count;
    std::getline(std::wcin, s_file_count);
    int file_count = std::stoi(s_file_count);

    std::list<std::string> filenames[thread_num];

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    for (int file_num = 0; file_num < file_count; file_num++) {
        std::wstring w_filename;
        std::getline(std::wcin, w_filename);
        std::string s_filename = converter.to_bytes(w_filename);
        filenames[file_num%thread_num].push_back(s_filename);
    }

    std::vector<std::promise<unsigned int>> word_count;
    std::vector<std::future<unsigned int>> word_future;

    for (int i = 0; i < thread_num; i++) {
        word_count.push_back(std::promise<unsigned int>{});
        word_future.push_back(word_count[i].get_future());
    }

    std::vector<std::thread> thr;
    for (int i = 0; i < thread_num; i++) {
        std::list<std::string> & f  = filenames[i];
        std::promise<unsigned int> & p = word_count[i];
        thr.push_back(std::thread{[&f, &word, &p]{ promised_grep(f, word, p); }});
    }

    int count = 0;
    for (int i = 0; i < thread_num; i++) {
        count += word_future[i].get();
    }

    std::cout << count << std::endl;

    for (int i = 0; i < thread_num; i++)
        thr[i].join();
}
