#include "entry.h"  

#include <unordered_set>
#include <iostream>
#include <algorithm>



void MessageEntry::AddMessage(const MessageEntry& entry){
    if(messages.find(entry.room_id) == messages.end())
        messages[entry.room_id] = std::vector<MessageEntry>();
    
    messages[entry.room_id].push_back(entry);
}
void MessageEntry::SortMessages(room_id_t id){
    std::sort(messages[id].begin(), messages[id].end());
    for(size_t i = 0; i < messages[id].size(); i++){
        messages[id][i].message_idx = i;
    }
}
void MessageEntry::SortAllMessages(){
    for(auto& it : messages){
        SortMessages(it.first);
    }
}


void UserEntry::AddWeight(const user_id_t& user, relation_weight_t weight){
    if(!relations_map.contains(user)){
        relations_map[user] = 0.0;
    }
    relations_map[user] += weight;
}

void UserEntry::BuildRelations(const std::vector<MessageEntry>& messages) { 
    using result_map = std::unordered_map<user_id_t, double>;
    size_t block_begin, block_end;
    bool is_in_block = false;
    ts_t last_message_ts = 0;
    std::vector<MessageBlock> message_blocks;
    for(size_t i = 0; i < messages.size(); i++){
        const MessageEntry& msg = messages[i];
        if(!is_in_block) {
            if(msg.sender != this->user_id)
                continue;
            is_in_block = true;
            last_message_ts = msg.timestamp;
            block_begin = i;
            block_end = i;
            continue;
        }
        if(msg.sender != this->user_id){
            if((msg.timestamp - last_message_ts) < Config::convo_deltatime_threshold)
                continue;
            MessageBlock block(block_begin, block_end, messages);
            if(block.IsValid()){
                message_blocks.push_back(block);
            }
                
            is_in_block = false;
            continue;
        }
        last_message_ts = msg.timestamp;
        block_end = i;
                
    }

    std::vector<result_map> result;
    for(MessageBlock& block : message_blocks){
        //you can only process in parallel if you initialize everything first
        result.push_back(block.Process(this->user_id));
    }
    for(const result_map& res : result){
        for(const auto& it : res) {
            this->AddWeight(it.first, it.second);
        }
    }
}

std::string UserEntry::GetRelations() const {
        std::string out;
        for(auto& it : relations_map){
            if(it.second < Config::relation_signficance_threshold)
                continue;

            out += it.first + " ";
            out += std::to_string(it.second) + "\n";
        }
        return out;
}

void UserEntry::CreateUserIfNotExist(user_id_t user_id) {
    auto it = user_entries.find(user_id);
    if(it != user_entries.end())
        return;
    user_entries.insert(std::pair<user_id_t, UserEntry>(user_id, UserEntry(user_id)));
}



