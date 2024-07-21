#pragma once
#include "typedefs.h"
#include <string>
#include <vector>
#include "config.h"



struct MessageEntry {
    msg_id_t msg_id = "";
    ts_t timestamp = 0;
    user_id_t sender = "";
    bool is_reply = 0;
    user_id_t replying_to = "";
    

    //Arbitraily set(kinda)
    size_t message_idx = 0;
    bool operator<(const MessageEntry& other){
        return timestamp < other.timestamp;
    }
    std::string ToString(){
        return "msg_id: " + msg_id + " timestamp: " + std::to_string(timestamp) + " sender: " + sender + " is_reply: " + std::to_string(is_reply) + (is_reply ? " replying to: " + replying_to : "");
    }
};





struct UserEntry {
    user_id_t user_id;
    //MessageEntry can be immutable anyway so i dont give a fuck
    //std::vector<const MessageEntry*> messages;

    //relation TO, not necessarily bidirectional(like if someone simps for another person and they never respond)
    relations_map_t relations_map;
    //add weight of self to user 
    inline void AddWeight(const user_id_t& user, relation_weight_t weight){
        if(relations_map.find(user) == relations_map.end()){
            relations_map[user] = weight;
            return;
        }
        relations_map[user] += weight;
    }
    size_t hash() const {
        return std::hash<std::string>()(user_id);
    }
    bool operator==(const UserEntry& other) const{
        return other.user_id == user_id;
    }
    UserEntry(user_id_t uid) {
        user_id = uid;
        relations_map = {};
    }
    inline std::string GetRelations() const{
        std::string out;
        for(auto& it : relations_map){
            if(it.second < Config::relation_signficance_threshold)
                continue;
            
            out += it.first + " ";
            out += std::to_string(it.second) + "\n";
        }
        return out;
    }
};

template<>
struct std::hash<UserEntry>
{
    std::size_t operator()(UserEntry const& e) const 
    {
       return e.hash();
    }
};
