#pragma once
#include "../shared/typedefs.h"



namespace Config {

    //These fields are not used anywhere else, feel free to remove them if you're reimplementing
    constexpr relation_weight_t reply_multiplier = 1;
    constexpr ts_t delta_min = 1000 * 5;
    constexpr ts_t delta_max = 1000 * 60;
    //dunno dude, its kinda arbitrary
    constexpr relation_weight_t reply_flat_addition = reply_multiplier * 1.337;
    constexpr relation_weight_t inbetween_weight_multiplier = 1;
    constexpr int inbetween_max_msgs = 10;
    
    inline static relation_weight_t GetWeight(ts_t delta_time, int messages_inbetween, bool is_replying, double activity = 0.00) {
        relation_weight_t weight = 0;
        if (is_replying)
            weight += reply_flat_addition;
        delta_time = std::min(delta_min, delta_time);
        if(delta_time < delta_max){
            //In seconds this time :)
            double delta_time_d = double(delta_time) / 1000;
        
            weight += (1 / delta_time_d) * reply_multiplier;
        }
        //messages_inbetween += 1;
        if(messages_inbetween < inbetween_max_msgs){
            weight += (1 / (double)messages_inbetween) * inbetween_weight_multiplier;
        }
        

        return weight;
        
    
        
    }

    //Actually used elsewhere
    constexpr relation_weight_t relation_signficance_threshold = 1;

}