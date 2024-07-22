#pragma once
#include "typedefs.h"
#include <string>
#include <vector>
#include "config.h"



struct MessageEntry {
    msg_id_t msg_id = "";
    ts_t timestamp = 0;
    room_id_t room_id = "";
    user_id_t sender = "";
    bool is_reply = 0;
    user_id_t replying_to = "";
    

    //Arbitraily set(kinda)
    size_t message_idx = 0;
    bool operator<(const MessageEntry& other){
        return timestamp < other.timestamp;
    }
    std::string ToString(){
        return "msg_id: " + msg_id + " room id: " + room_id + " timestamp: " + std::to_string(timestamp) + " sender: " + sender + " is_reply: " + std::to_string(is_reply) + (is_reply ? " replying to: " + replying_to : "");
    }
};





struct UserEntry {
    user_id_t user_id;
    //MessageEntry can be immutable anyway so i dont give a fuck
    //std::vector<const MessageEntry*> messages;

    //relation TO, not necessarily bidirectional(like if someone simps for another person and they never respond)
    relations_map_t relations_map;
    inline static std::unordered_map<user_id_t, UserEntry> user_entries;
    //add weight of self to user 
    void AddWeight(const user_id_t& user, relation_weight_t weight);
    bool operator==(const UserEntry& other) const{
        return other.user_id == user_id;
    }

    UserEntry(user_id_t uid) {
        user_id = uid;
        relations_map = {};
    }
    
    void BuildRelations(const std::vector<MessageEntry>& messages);
    std::string GetRelations() const;
    static void CreateUserIfNotExist(user_id_t uid);
    
};

