#include <atomic>
#include <condition_variable>

#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <mutex>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "timing.h"
std::mutex mtx;


using namespace std;

map<string, int> m;
mutex myMutex;
mutex myMutex2;
condition_variable cv;
condition_variable conditionVariable;

deque<vector<string>> dq;
deque <map<string, int>> dm;
atomic<bool> done {false};
using namespace std;
int N=0;                            //кількість потоків(потім змінюється відповідно до вказаного)
string fileO, fileWA, fileWN;       // файл з якого беруться слова і два файли для запису (WA - по афавіту,WN- за кількістю)
int block_size = 10; //      змінити


void reading(){                     //відкривання файлу з даними, виділення шляхів до файлів і кількості потоків
    ifstream myfile;
    vector<string> words;
    string word;
    myfile.open("Read.txt");    //файл який має три шляхи для файлів і кількість потоків
    if (!myfile.is_open()) {
        cerr << "Error" << endl;
    }
    while (myfile >> word) {        //додаємо рядок без =
        words.push_back(word);
    }

    for (int i = 0; i < words.size(); ++i) {     //прінт всіх
        cout << words[i] << ' ';
        if( words[i].find("infile=") != std::string::npos ){
            size_t pos = words[i].find("infile=");
            fileO = words[i].substr(pos+8);
            fileO = fileO.substr(0, fileO.length()-1);
        }
        else if( words[i].find("out_by_a=") != std::string::npos){
            size_t pos = words[i].find("out_by_a=");
            fileWA = words[i].substr(pos+10);
            fileWA = fileWA.substr(0, fileWA.length()-1);
        }
        else if( words[i].find("out_by_n=") != std::string::npos){
            size_t pos = words[i].find("out_by_n=");
            fileWN = words[i].substr(pos+10);
            fileWN = fileWN.substr(0, fileWN.length()-1);
        }
        else if( words[i].find("threads=") != std::string::npos){
            size_t pos = words[i].find("threads=");
            N = atoi(words[i].substr(pos+8).c_str());
        }
        else if( words[i].find("block_size=") != std::string::npos ){
            size_t pos = words[i].find("block_size=");
            block_size = atoi(words[i].substr(pos+11).c_str());
        }
    }
}

void printMap(const map<string, int> &m) {
    for (auto &elem : m) {
        cout << elem.first << " " << elem.second << "\n";
    }
}

int write_file(string path, map <string, int> a){
    ofstream myfile;
    myfile.open (path);
    for(auto &elem : a){
        myfile << elem.first + "   " + to_string(elem.second) + "\n";
    }
    myfile.close();
    return 0;
}


int write_vector_file(string path, vector<pair<string, int>> v ){
    ofstream myfile;
    myfile.open (path);
    for(const auto& p: v){
        myfile << p.first+ "   " + to_string(p.second) + "\n";
    }
    myfile.close();
    return 0;
}



void counting_words_worker(vector<string>words){
    map<string, int> localm;
    for (int i = 0; i != words.size(); ++i) {
        std::istringstream iss(words[i]);
        for(string s; iss >> s; )
            ++localm[s];
    }
    mtx.lock();
    dm.push_back(localm);
    mtx.unlock();
}

map<string, int> reducing_words_worker(map<string, int> first, map<string, int > second){
    myMutex2.lock();
    for(auto it = first.begin(); it != first.end(); ++it) second[it->first] += it->second;
    myMutex2.unlock();
    return second;
}

void producer(string path) {
    ifstream myfile;
    string word;
    myfile.open(path);
    if (!myfile.is_open()) {
        cerr << "Error" << endl;
        exit(1);
    }



    while (myfile)
    {
        size_t line_index = 0;
        string line;
        vector<string> lines;
        while(line_index < block_size && getline(myfile, line) )
        {
            ++line_index;
            lines.push_back(line);
        }

        {
            lock_guard<mutex> lg(myMutex);
            dq.push_back(lines);
        }
        cv.notify_one();
    }
    done = true;
    myfile.close();
    cv.notify_all();
    return;
}



void consumer() {
    unique_lock<mutex> ul(myMutex);
    while (!(done && dq.empty())) {
        if (!dq.empty()) {
            vector<string> v = dq.front();
            dq.pop_front();
            ul.unlock();
            counting_words_worker(v);
            conditionVariable.notify_one();
            ul.lock();
        } else {
            cv.wait(ul);
        }
    }
    conditionVariable.notify_all();
}

void reducer(){
    while (! dm.empty()) {
        unique_lock<mutex> ul(myMutex);
        if (dm.size() != 1) {
            map<string, int> map1 = dm.front();
            dm.pop_front();
            map<string, int> map2 = dm.front();
            dm.pop_front();
            ul.unlock();
            map<string, int> map3 = reducing_words_worker(map1, map2);
            //printMap(map3);
            ul.lock();
            dm.push_back(map3);
        } else {
            break;
        }
    }
    if(dm.size() == 1){
        m = dm.front();
    }

}

int main() {
    auto stage1_start_time = get_current_time_fenced();
    reading();

    thread my_thread[N];
    thread other_thread[N];
    thread thread1 = thread(producer, fileO);
    auto finish_time = get_current_time_fenced();
    auto reading_time = finish_time - stage1_start_time;    //reading time
    auto stage2_start_time = get_current_time_fenced(); //початок підрахунку

    for(int id = 0; id < N; id++){
        my_thread[id] =  thread(consumer);}
    thread1.join();
    for(int id = 0; id < N; id++){
        my_thread[id].join();}

    for(int id = 0; id < N; id++){
        other_thread[id] =  thread(reducer);}
    for(int id = 0; id < N; id++){
        other_thread[id].join();}

    auto stage3_start_time = get_current_time_fenced(); //кінець підрахунку
    auto counting_time = stage3_start_time - stage2_start_time;

    printMap(m);

    //перетворення в вектор для сортування за 2 аргументом
    struct less_second {
        typedef pair<string, int> type;
        bool operator ()(type const& a, type const& b) const {
            return a.second < b.second;
        }
    };
    vector<pair<string, int> > mapcopy(m.begin(), m.end());
    sort(mapcopy.begin(), mapcopy.end(), less_second());

    auto stage4_start_time = get_current_time_fenced(); //початок запису
    //запис в файли
    write_file(fileWA, m);
    write_vector_file(fileWN, mapcopy);
    auto stage5_start_time = get_current_time_fenced(); //кінець запису
    auto writing_time = stage5_start_time - stage4_start_time;

    auto all_time = writing_time + counting_time + reading_time;


    cout << "ONLY reading time: " << to_us(reading_time) << endl;
    cout << "ONLY counting time: " << to_us(counting_time) << endl;
    cout << "ONLY writing time: " << to_us(writing_time) << endl;
    cout << "ALL time: " << to_us(all_time) << endl;

    fstream log;
    log.open("./result.txt", fstream::app);
    std::chrono::duration<double, std::milli> r_ms = reading_time;
    std::chrono::duration<double, std::milli> w_ms = writing_time;
    std::chrono::duration<double, std::milli> c_ms = counting_time;
    std::chrono::duration<double, std::milli> all_ms = all_time;
    log<< r_ms.count();
    log<< "  ";
    log<< w_ms.count();
    log<< "  ";
    log<< c_ms.count();
    log<< "  ";
    log<< all_ms.count();
    log<< " \n";
    log.close();
    pthread_exit(NULL);
}