#pragma once
#include <string>
#include <unordered_map>






using int64 = long long;
using uint64 = unsigned long long;


using user_id_t = std::string;
using msg_id_t = std::string;
using relation_weight_t = double;
using relations_map_t = std::unordered_map<user_id_t, relation_weight_t>;
// id use time_t but it is nonstandard
//timestamp type, in milliseconds
using ts_t = uint64;