#include "entry.h"
#include "config.h"
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <thread>


using args_pair = std::pair<UserEntry*, const std::vector<MessageEntry>&>;

void BuildRelationsWrapper(UserEntry* user, const std::vector<MessageEntry>& msgs){
    user->BuildRelations(msgs);
}

void RunProcsMultithreaded(int n_threads, std::vector<args_pair>& args_vec){
    std::vector<std::thread> thread_vec;
    for (int i = 0; i < n_threads; i++)
        thread_vec.push_back(std::thread(BuildRelationsWrapper, args_vec[i].first, args_vec[i].second));
    
    for (std::thread& t : thread_vec)
        t.join();

    
}



int main(){


    int n_lines;
    std::vector<MessageEntry> messages;
    std::unordered_map<std::string, UserEntry> users;
    using user_lookup_pair = std::pair<std::string, UserEntry>;
    //std::cin >> n_lines;

       
    
    //messages.reserve(n_lines);
    for(int i = 0; !std::cin.eof(); i++) {
        MessageEntry entry;
        
        std::cin >> entry.msg_id >> entry.room_id >> entry.timestamp >> entry.sender >> entry.is_reply;

        //if somehow already eof
        if(entry.msg_id == "")
            break;

        if( entry.is_reply ){
            std::cin >> entry.replying_to;
        }
            
        //std::cout << entry.ToString() << "\n";
        messages.push_back(entry);

    }
    std::sort(messages.begin(), messages.end());
    for(int i = 0; i < messages.size(); i++){
        messages[i].message_idx = i;
    }

    for(const MessageEntry& entry : messages){
        UserEntry::CreateUserIfNotExist(entry.sender);
        if(entry.is_reply){
            UserEntry::CreateUserIfNotExist(entry.replying_to);
        }
        
    }
    std::cout << "Finished parsing input data. Got total of " << messages.size() << " Messages and " << UserEntry::user_entries.size() << " User entries Processing...\n";
    std::vector<args_pair> args_v;
    for(auto& it : UserEntry::user_entries){
        
        //lmao
        args_v.push_back(args_pair((&it.second), messages)); 
    }
    RunProcsMultithreaded(args_v.size(), args_v);
    for(auto& it : UserEntry::user_entries){
        std::cout << "\nRelations of: " << it.first << "\n" << it.second.GetRelations() << "\n\n";
    }
    
}