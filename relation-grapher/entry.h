#pragma once

#include <string>
#include <vector>
#include <unordered_map>
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
    std::vector<MessageEntry>* messages;
    size_t start, end;
    public:
    MessageBlock(size_t start, size_t end, std::vector<MessageEntry>& messages){
        //too lazy to bound check, just dont go out of bounds please
        this->messages = &messages;
        this->start = start;
        this->end = end;
    }
    auto GetSpan() const{
        return std::span(messages->begin() + start, messages->begin() + end);
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
    //void ProcessMessageBlock(const MessageBlock& messages);
    void BuildRelations(const std::vector<MessageEntry>& messages);
    std::string GetRelations() const;
    static void CreateUserIfNotExist(user_id_t uid);
    
};

