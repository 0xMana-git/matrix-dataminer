#pragma once
#include <mutex>
#include <unordered_map>
#include <string>


template<typename K1, typename V1 = std::string>
class ValMapper {
    std::mutex lock{};
    std::unordered_map<K1, V1> map{};

    V1 AddEntry(K1 key){
        lock.lock();
        V1 new_v = std::to_string(map.size());
        map.insert(std::pair<K1, V1>(key, new_v));
        lock.unlock();
        return new_v;
    };
public:
    V1 GetEntry(K1 key){
        const auto& iter = map.find(key);
        if(iter == map.end()) 
            return AddEntry(key);
        
        return iter->second;
    };

    
};