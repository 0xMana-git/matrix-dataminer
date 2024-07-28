#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <span>

#include "../shared/typedefs.h"
#include "../shared/entry.h"

#include "config.h"


struct MessageEntry : MessageEntryBase{

    inline static std::unordered_map<room_id_t, std::vector<MessageEntry>> messages;

    //Arbitraily set(kinda)
    size_t message_idx = 0;
    
    static void AddMessage(const MessageEntry& entry);
    static void SortMessages(room_id_t id);
    static void SortAllMessages();
};


class MessageBlock {
    
    size_t start = -1, end = -1;
    std::vector<MessageEntry> const* msg_vec = 0;
    std::vector<user_id_t> participants;
    std::unordered_map<user_id_t, double> participants_ratio;
    double activity = -1.0;

    public:
    MessageBlock(size_t bstart, size_t bstop, const std::vector<MessageEntry>& msg_vec){
        //too lazy to bound check, just dont go out of bounds please
        this->start = bstart;
        this->end = bstop;
        this->msg_vec = &msg_vec;
    }
    const auto GetSpan() const{
        return std::span(msg_vec->begin() + start, msg_vec->begin() + end);
    }
    //too lazy to implement rn, maybe later
    std::vector<MessageBlock> GetSubBlocks(ts_t max_delta_time) const{
        return {};
    }
    const std::unordered_map<user_id_t, double>& GetParticipantsRatio();
    std::unordered_map<user_id_t, double> Process(user_id_t src_user);
    double GetActivity();
    const std::vector<user_id_t>& GetParticipants();
    size_t GetParticipantsN();
    bool IsValid(){
        if(start == end)
            return false;
        if(msg_vec->at(end).timestamp - msg_vec->at(start).timestamp < Config::min_block_deltatime)
            return false;
        return true;
    }
    void InitStats(){
        GetParticipantsRatio();
        GetActivity();
        GetParticipants();
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
    //I cant make this vector const but just please dont modify it dude
    void BuildRelations(const std::vector<MessageEntry>& messages);
    std::string GetRelations() const;
    static void CreateUserIfNotExist(user_id_t uid);
    
};

