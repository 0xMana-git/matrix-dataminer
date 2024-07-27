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
        relations_map[user] = 0;
    }
    relations_map[user] += weight;
}

void UserEntry::ProcessMessageBlock(MessageBlock& messages){
    size_t current_user_sent_msgs = 0;
    std::unordered_map<user_id_t, const MessageEntry*> latest_msgs;
    std::unordered_set<msg_id_t> processed;
    for(const MessageEntry& entry : messages.GetSpan()) {
        if(!(entry.sender == this->user_id)) {

            latest_msgs[entry.sender] = &entry;
            current_user_sent_msgs = 0;
            continue;
        }

        current_user_sent_msgs++;

        for(auto& it : latest_msgs) {
            if(processed.count(it.second->msg_id) != 0){
                continue;
            }
            const MessageEntry& latest = *it.second;
            processed.insert(latest.msg_id);

            ts_t delta_time = entry.timestamp - latest.timestamp;
            int message_between = entry.message_idx - latest.message_idx;
            
            relation_weight_t weight = 0;
            //is reply is actually fairly complex, ill figure it out l8r

            weight = Config::GetWeight(delta_time, message_between, false, messages.GetParticipantsN(), current_user_sent_msgs, messages.GetActivity());

            this->AddWeight(latest.sender, weight);
        
        }

    }
}
void UserEntry::BuildRelations(std::vector<MessageEntry>& messages) { 
    using message_vec_iter = std::vector<MessageEntry>::iterator;
    message_vec_iter block_begin_it, block_end_it;
    bool is_in_block = false;
    ts_t last_message_ts = 0;
    std::vector<MessageBlock> message_blocks;
    for(auto it = messages.begin(); it != messages.end(); it++){
        auto msg = *it;
        if(!is_in_block) {
            if(msg.sender != this->user_id)
                continue;
            is_in_block = true;
            last_message_ts = msg.timestamp;
            block_begin_it = it;
            block_end_it = it;
            continue;
        }
        if(msg.sender != this->user_id){
            if((msg.timestamp - last_message_ts) < Config::convo_deltatime_threshold)
                continue;
            message_blocks.push_back(MessageBlock(block_begin_it, block_end_it));
            is_in_block = false;
            continue;
        }
        last_message_ts = msg.timestamp;
        block_end_it = it;
                
    }
    for(MessageBlock& block : message_blocks){
        //you CAN process this in parallel if you figure it out :D
        this->ProcessMessageBlock(block);
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



double MessageBlock::GetActivity() {
    if(activity < 0){
        double delta = (double)(end->timestamp - start->timestamp) * 0.001;
        activity = (double)GetSpan().size() / delta;
    }
    return activity;
}
const std::vector<user_id_t> MessageBlock::GetParticipants(){
    if(participants.size() != 0)
        return participants;
    std::unordered_set<user_id_t> participants_set;
    for(const MessageEntry& entry : GetSpan())
        participants_set.insert(entry.sender);

    for(const user_id_t& uid : participants_set)
        participants.push_back(uid);
        
    return participants;
}
size_t MessageBlock::GetParticipantsN() {
    if(participants.size() == 0)
        GetParticipants();
    return participants.size();
}