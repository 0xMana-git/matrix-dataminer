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
        if(relations_map.find(user) == relations_map.end()){
            relations_map[user] = weight;
            return;
        }
        relations_map[user] += weight;
    }

void UserEntry::BuildRelations(const std::vector<MessageEntry>& messages){ 
    bool current_user_talking = false;
    std::unordered_map<user_id_t, const MessageEntry*> latest_msgs;
    std::unordered_set<msg_id_t> processed;
    for(const MessageEntry& entry : messages) {
        if(!(entry.sender == this->user_id)) {

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
                if(entry.replying_to == latest.sender)
                    weight = Config::GetWeight(delta_time, message_between, true);
            }
            weight = Config::GetWeight(delta_time, message_between, false);

            this->AddWeight(latest.sender, weight);
        
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