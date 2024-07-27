#include "entry.h"
#include "config.h"
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <thread>
#include "../shared/thread_safe_queue.h"

#define threads_n 32

using args_pair = std::pair<UserEntry*, std::vector<MessageEntry>&>;
using TaskQueue = ThreadSafeQueue<args_pair>;
void BuildRelationsWrapper(UserEntry* user, std::vector<MessageEntry>& msgs){
    user->BuildRelations(msgs);
}


void PoolWorker(TaskQueue* queue){

    while(true){
        auto task = queue->Pop();
        if(!task.has_value())
            return;
        task.value().first->BuildRelations(task.value().second);
    }
    
}
void RunProcsMultithreaded(int n_threads, std::vector<args_pair>& args_vec){
    std::vector<std::thread> thread_vec;
    TaskQueue queue;
    for(args_pair& args : args_vec){
        queue.Push(args);
    }
    for (int i = 0; i < n_threads; i++)
        thread_vec.push_back(std::thread(PoolWorker, &queue));
    
    for (std::thread& t : thread_vec)
        t.join();

    
}



int main(){


    int n_lines;
    std::unordered_map<std::string, UserEntry> users;
    using user_lookup_pair = std::pair<std::string, UserEntry>;
    //std::cin >> n_lines;

       
    
    //messages.reserve(n_lines);
    for(int i = 0; !std::cin.eof(); i++) {
        std::string line;
        std::cin >> line;
        auto res = MessageEntry::FromDBEntry(line);
        if(!res.has_value())
            continue;
        MessageEntry entry;
        //weird stuff happens here but it SHOULD work
        *(MessageEntryBase*)(&entry) = res.value();
        
        //std::cout << entry.ToString() << "\n";
        MessageEntry::AddMessage(entry);

    }
    MessageEntry::SortAllMessages();
    for(auto& it : MessageEntry::messages) {
        std::vector<MessageEntry>& msgs = it.second;

        for(const MessageEntry& entry : msgs){
            UserEntry::CreateUserIfNotExist(entry.sender);
            if(entry.is_reply) {
                UserEntry::CreateUserIfNotExist(entry.replying_to);
            }   
        
        }
        //std::cout << "Finished parsing input data. Got total of " << messages.size() << " Messages and " << UserEntry::user_entries.size() << " User entries Processing...\n";
        std::vector<args_pair> args_v;
        for(auto& it2 : UserEntry::user_entries){
            args_v.push_back(args_pair((&it2.second), msgs)); 
        }
        RunProcsMultithreaded(threads_n, args_v);
    }
    
    for(auto& it : UserEntry::user_entries){
        std::cout << "\nRelations of: " << it.first << "\n" << it.second.GetRelations() << "\n\n";
    }
    
}