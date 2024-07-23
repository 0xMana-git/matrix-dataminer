#include "typedefs.h"




struct MessageEntryBase {
    msg_id_t msg_id = "";
    ts_t timestamp = 0;
    room_id_t room_id = "";
    user_id_t sender = "";
    bool is_reply = 0;
    user_id_t replying_to = "";

    inline bool operator<(const MessageEntryBase& other){
        return timestamp < other.timestamp;
    }
    inline std::string ToString(){
        return "msg_id: " + msg_id + " room id: " + room_id + " timestamp: " + std::to_string(timestamp) + " sender: " + sender + " is_reply: " + std::to_string(is_reply) + (is_reply ? " replying to: " + replying_to : "");
    }
};