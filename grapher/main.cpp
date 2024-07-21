#include "entry.h"
#include "config.h"
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <thread>

void BuildRelations(UserEntry& user, const std::vector<MessageEntry>& messages) {
    bool current_user_talking = false;
    std::unordered_map<user_id_t, const MessageEntry*> latest_msgs;
    std::unordered_set<msg_id_t> processed;
    for(const MessageEntry& entry : messages) {
        if(!(entry.sender == user.user_id)) {

            latest_msgs[entry.sender] = &entry;
            current_user_talking = false;
            continue;
        }
        if(current_user_talking)
            continue;
        current_user_talking = true;

        for(auto& it : latest_msgs) {
            if(processed.count(it.second->msg_id) != 0){
                continue;
            }
            const MessageEntry& latest = *it.second;
            processed.insert(latest.msg_id);

            ts_t delta_time = entry.timestamp - latest.timestamp;
            int message_between = entry.message_idx - latest.message_idx;
            relation_weight_t weight = 0;
            if(entry.is_reply){
                if(entry.replying_to == latest.sender){
                    weight = Config::GetWeight(delta_time, message_between, true);
                }
                
            }
            weight = Config::GetWeight(delta_time, message_between, false);
            user.AddWeight(latest.sender, weight);
            //std::cout << "\nAdded " << weight << " to " << latest.sender << ", from " << user.user_id;
        }

    }
}
using args_pair = std::pair<UserEntry&, const std::vector<MessageEntry>&>;
using proc_t = decltype(BuildRelations);

void RunProcsMultithreaded(int n_threads, proc_t proc, std::vector<args_pair>& args_vec){
    std::vector<std::thread> thread_vec;
    for (int i = 0; i < n_threads; i++)
        thread_vec.push_back(std::thread(proc, std::ref(args_vec[i].first), args_vec[i].second));
    
    for (std::thread& t : thread_vec)
        t.join();

    
}



int main(){


    int n_lines;
    std::vector<MessageEntry> messages;
    std::unordered_set<UserEntry> users;
    //std::cin >> n_lines;

       
    
    //messages.reserve(n_lines);
    for(int i = 0; !std::cin.eof(); i++) {
        MessageEntry entry;
        entry.message_idx = i;
        
        std::cin >> entry.msg_id >> entry.timestamp >> entry.sender >> entry.is_reply;
        if( entry.is_reply ){
            std::cin >> entry.replying_to;
        }
            
            
        messages.push_back(entry);

    }
    std::sort(messages.begin(), messages.end());
    for(const MessageEntry& entry : messages){
        users.insert(UserEntry(entry.sender));
        if(entry.is_reply){
            users.insert(UserEntry(entry.replying_to));
        }
        
    }
    std::cout << "Finished parsing input data. Processing...\n";
    std::vector<args_pair> args_v;
    for(const UserEntry& user : users){
        //lmao
        args_v.push_back(args_pair(const_cast<UserEntry&>(user), messages)); 
    }
    RunProcsMultithreaded(args_v.size(), BuildRelations, args_v);
    for(const UserEntry& user : users){
        std::cout << "\nRelations of: " << user.user_id << "\n" << user.GetRelations() << "\n\n";
    }
    
}