#include <string>
#include <vector>
#include <mutex>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "valmapper.h"

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}







// # sqlite> PRAGMA table_info(events);
// # 0|stream_ordering|INTEGER|0||1
// # 1|topological_ordering|BIGINT|1||0
// # 2|event_id|TEXT|1||0
// # 3|type|TEXT|1||0
// # 4|room_id|TEXT|1||0
// # 5|content|TEXT|0||0
// # 6|unrecognized_keys|TEXT|0||0
// # 7|processed|BOOL|1||0
// # 8|outlier|BOOL|1||0
// # 9|depth|BIGINT|1|0|0
// # 10|origin_server_ts|BIGINT|0||0
// # 11|received_ts|BIGINT|0||0
// # 12|sender|TEXT|0||0
// # 13|contains_url|BOOLEAN|0||0
// # 14|instance_name|TEXT|0||0
// # 15|state_key|TEXT|0|NULL|0
// # 16|rejection_reason|TEXT|0|NULL|0
#define event_id 2
#define origin_server_ts 10
#define sender 12
#define type 3
#define room_id 4


void emptyproc(){};
using proc_t = decltype(emptyproc);
using lookup_pair_t = std::pair<std::string, int>;
std::mutex stdin_mtx, stdout_mtx;

ValMapper<std::string> room_id_map;
std::string StdinReadLine() {
    std::string str;

    stdin_mtx.lock();
    std::cin >> str;
    stdin_mtx.unlock();

    return str;
}


void StdoutWriteLine(std::string& str){
    str += "\n";

    stdout_mtx.lock();
    std::cout << str;
    stdout_mtx.unlock();

}

void WorkerProc() {
    //Dont know if this is thread-safe, sure as shit hope it is
    std::string in, out;
    while(!std::cin.eof()){
        in = StdinReadLine();
        std::vector<std::string> substrs = split(in, "|");
        if(substrs[type] != "m.room.message")
            continue;
        out = substrs[event_id] + " " + room_id_map.GetEntry(substrs[room_id]) + " " + substrs[origin_server_ts] + " " + substrs[sender] + " 0";
        StdoutWriteLine(out);

    }
    
}


void RunProcsMultithreaded(int n_threads, proc_t proc){
    std::vector<std::thread> thread_vec;
    for (int i = 0; i < n_threads; i++)
        thread_vec.push_back(std::thread(proc));
    
    for (std::thread& t : thread_vec)
        t.join();

    
}








int main(){
    RunProcsMultithreaded(1, WorkerProc);
}
    