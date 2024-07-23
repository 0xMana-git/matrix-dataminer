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

#define lines_per_chunk 128

#define threads_n 32
#define remap_room_name true


void emptyproc(){};
using proc_t = decltype(emptyproc);
using lookup_pair_t = std::pair<std::string, int>;
std::mutex stdin_mtx, stdout_mtx;

ValMapper<std::string> room_id_map;
using string_vec_t = std::vector<std::string>;
string_vec_t StdinReadLines(int n_lines = lines_per_chunk) {
    string_vec_t lines(n_lines);

    stdin_mtx.lock();
    for(int i = 0; i < n_lines; i++)
        std::cin >> lines[i];
    stdin_mtx.unlock();

    return lines;
}


void StdoutWriteLines(string_vec_t& lines){
    for(std::string& line : lines){
        line += "\n";
    }

    stdout_mtx.lock();
    for(std::string& line : lines){
        std::cout << line;
    }
    stdout_mtx.unlock();

}

void WorkerProc() {
    //Dont know if this is thread-safe, sure as shit hope it is
    string_vec_t in, out;
    while(!std::cin.eof()){
        in = StdinReadLines();
        out.clear();
        for(std::string& line : in){
            if(line.size() < 2)
                continue;
            std::vector<std::string> substrs = split(line, "|");
        if(substrs[type] != "m.room.message")
            continue;
        std::string room_id_str;
        if(remap_room_name){
            room_id_str = room_id_map.GetEntry(substrs[room_id]);
        }
        else {
            room_id_str = substrs[room_id];
        };
            
        out.push_back(substrs[event_id] + " " + room_id_str + " " + substrs[origin_server_ts] + " " + substrs[sender] + " 0");
        }
        
        StdoutWriteLines(out);

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
    RunProcsMultithreaded(threads_n, WorkerProc);
}
    