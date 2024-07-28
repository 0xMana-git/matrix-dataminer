#pragma once
#include "../shared/typedefs.h"
#include <iostream>
#include <limits>
#include <cassert>

namespace Config {

    //These fields are not used anywhere else, feel free to remove them if you're reimplementing
    constexpr relation_weight_t reply_multiplier = 1;
    constexpr ts_t delta_min = 1000 * 5;
    constexpr ts_t delta_max = 1000 * 60;
    //dunno dude, its kinda arbitrary
    constexpr relation_weight_t reply_flat_addition = reply_multiplier * 1.337;
    constexpr relation_weight_t inbetween_weight_multiplier = 1;
    constexpr int inbetween_max_msgs = 10;
    
    inline relation_weight_t GetWeight(double user_ratio, double other_ratio, double overall_activity) {
        relation_weight_t weight = 1;

        weight *= overall_activity;
        weight *= other_ratio;
        weight *= user_ratio;
        weight *= 10000;
        return weight;
    }

    //Actually used elsewhere
    constexpr relation_weight_t relation_signficance_threshold = 0.1;

    constexpr ts_t convo_deltatime_threshold = 1000 * 1800;
    constexpr ts_t min_block_deltatime = 1000 * 60;
}