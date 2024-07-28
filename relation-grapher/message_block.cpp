#include "entry.h"  

double MessageBlock::GetActivity() {
    if(activity < 0){
        double delta = (double)(msg_vec->at(end).timestamp - msg_vec->at(start).timestamp);
        activity = (double)GetSpan().size() / delta;
    }
    return activity;
}
const std::vector<user_id_t>& MessageBlock::GetParticipants(){
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

const std::unordered_map<user_id_t, double>& MessageBlock::GetParticipantsRatio(){
    if(participants_ratio.size() != 0)
        return participants_ratio;

    double sum = 0;
    for(const MessageEntry& msg : GetSpan()) {
        const user_id_t& uid = msg.sender;
        if(!participants_ratio.contains(uid)){
            participants_ratio.emplace(uid, 0);
        }
        sum++;
        participants_ratio[uid]++;

    }   
    for(auto& it : participants_ratio){
        it.second /= sum;
    }
    

    return participants_ratio;
}

std::unordered_map<user_id_t, double> MessageBlock::Process(user_id_t src_user){
    using result_map = std::unordered_map<user_id_t, double>;
    result_map ratio = GetParticipantsRatio();
    result_map results;
    double activity = GetActivity();
    for(const auto& it : ratio){
        if(it.first == src_user)
            continue;
        results.emplace(it.first, Config::GetWeight(ratio[src_user], it.second, activity));
    }
    return results;
}