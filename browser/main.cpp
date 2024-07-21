#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

bool contains_substr(const std::string& s, const std::string& substr){
    return s.find(substr) != std::string::npos;
}

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
using entry_t = std::pair<std::string, double>;
bool entry_cmp(const entry_t& first, const entry_t& second){
    return first.second > second.second;
}

int main(int argc, char** argv){
    
    std::string username(argv[1]);
    std::string temp, file;
    bool found_flag = false;
    std::vector<entry_t> entries;
    while(std::getline(std::cin, temp)){ 
        
        if((!contains_substr(temp, "@")) && found_flag){
            found_flag = false;
            std::sort(entries.begin(), entries.end(), entry_cmp);

            for(entry_t& entry : entries){
                std::cout << entry.first << " " << entry.second << "\n";
            }
            entries.clear();
            std::cout << "\n";
            continue;
        }
            

        if(found_flag){
            if(argc == 3)
                if(!contains_substr(temp, argv[2]))
                    continue;
                    
            std::vector<std::string> entry_raw = split(temp, " ");
            entries.push_back(entry_t(entry_raw[0], stod(entry_raw[1])));
        }

        if(contains_substr(temp, "Relations of: ") && contains_substr(temp, argv[1])){
            std::cout << temp << "\n";
            found_flag = true;
        }

	};
}