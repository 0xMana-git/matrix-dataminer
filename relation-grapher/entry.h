#pragma once
#include "../shared/typedefs.h"
#include "../shared/entry.h"
#include <string>
#include <vector>
#include "config.h"
#include <unordered_map>



struct MessageEntry : MessageEntryBase{

    inline static std::unordered_map<room_id_t, std::vector<MessageEntry>> messages;

    //Arbitraily set(kinda)
    size_t message_idx = 0;
    
    static void AddMessage(const MessageEntry& entry);
    static void SortMessages(room_id_t id);
    static void SortAllMessages();
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

