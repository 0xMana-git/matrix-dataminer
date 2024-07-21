#include <vector>
#include <iostream>
#include <string>


bool contains_substr(const std::string& s, const std::string& substr){
    return s.find(substr) != std::string::npos;
}

int main(int argc, char** argv){
    
    std::string username(argv[1]);
    std::string temp, file;
    bool found_flag = false;
    while(std::getline(std::cin, temp)){ 
        if((!contains_substr(temp, "@")) && found_flag){
            found_flag = false;
            std::cout << "\n";
            continue;
        }
            

        if(found_flag){
            if(argc == 3){
                if(contains_substr(temp, argv[2]))
                    std::cout << temp << "\n";
                continue;
            }
            // if(temp.find("0.020000") != std::string::npos){
            //     continue;
            // }
            std::cout << temp << "\n";
        }

        if(contains_substr(temp, "Relations of: ") && contains_substr(temp, argv[1])){
            std::cout << temp << "\n";
            found_flag = true;
        }

	};
}