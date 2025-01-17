#pragma once
#include <optional>
#include <string>
#include "typedefs.h"
#include "utils.h"
#include "valmapper.h"

namespace EventsDBEnum {
        enum EventsDBIndex {
            event_id = 2,
            type = 3,
            room_id = 4,
            origin_server_ts = 10,
            sender = 12 
        };
};

struct MessageEntryBase {
protected:
    constexpr static bool parse_message_contents = false;
    constexpr static bool allow_encrypted = true;
    constexpr static bool remap_room_ids = true;
    
public:
    msg_id_t msg_id = "";
    ts_t timestamp = 0;
    room_id_t room_id = "";
    user_id_t sender = "";
    bool is_reply = 0;
    user_id_t replying_to = "";
    
    inline bool operator<(const MessageEntryBase& other) const{
        return timestamp < other.timestamp;
    }
    inline std::string ToString() const {
        return "msg_id: " + msg_id + " room id: " + room_id + " timestamp: " + std::to_string(timestamp) + " sender: " + sender + " is_reply: " + std::to_string(is_reply) + (is_reply ? " replying to: " + replying_to : "");
        
    }
    static std::optional<MessageEntryBase> FromDBEntry(const std::string& entry) {
        static ValMapper<std::string> mapper;
        std::vector<std::string> substrs = string_split(entry, "|");
        if(substrs[EventsDBEnum::type] != "m.room.message"){
            if(!allow_encrypted)
                return {};
            if(substrs[EventsDBEnum::type] != "m.room.encrypted")
                return {};
        }
        
        std::string room_id_str = substrs[EventsDBEnum::room_id];
        if(remap_room_ids)
            room_id_str = mapper.GetEntry(room_id_str);
        

        return MessageEntryBase(substrs[EventsDBEnum::event_id], 
                            stoll(substrs[EventsDBEnum::origin_server_ts]), 
                            room_id_str, 
                            substrs[EventsDBEnum::sender]);

    }
    MessageEntryBase(){};
    MessageEntryBase(msg_id_t msg_id, ts_t timestamp, room_id_t room_id, user_id_t sender, bool is_reply = 0, user_id_t replying_to = ""){
        this->msg_id = msg_id;
        this->timestamp = timestamp;
        this->room_id = room_id;
        this->sender = sender;
        this->is_reply = is_reply;
        this->replying_to = replying_to;
    }
};


// # sqlite> PRAGMA table_info(events);
// # 0|stream_ordering|INTEGER|0||1
// # 1|topological_ordering|BIGINT|1||0
// # 2|event_id|TEXT|1||0
// # 3|type|TEXT|1||0
// # 4|room_id|TEXT|1||0
// # 5|content|TEXT|0||0
// # 6|unrecognized_keys|TEXT|0||0
// # 7|processed|BOOL|1||0
// # 8|outlier|BOOL|1||0
// # 9|depth|BIGINT|1|0|0
// # 10|origin_server_ts|BIGINT|0||0
// # 11|received_ts|BIGINT|0||0
// # 12|sender|TEXT|0||0
// # 13|contains_url|BOOLEAN|0||0
// # 14|instance_name|TEXT|0||0
// # 15|state_key|TEXT|0|NULL|0
// # 16|rejection_reason|TEXT|0|NULL|0