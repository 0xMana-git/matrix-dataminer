#include <vector>
#include <iostream>
#include <string>


int main(int argc, char** argv){
    
    std::string username(argv[1]);
    std::string temp, file;
    bool found_flag = false;
    while(std::getline(std::cin, temp)){ 
        if(temp.length() == 1 && found_flag)
            return 0;
        if(found_flag){
            
            if(argc == 3){
                if(temp.find(argv[2]) != std::string::npos){
                    std::cout << temp << "\n";
                    return 0;
                }
                continue;
            }
            std::cout << temp << "\n";
        }

        if(temp.find("Relations of: " + username) != std::string::npos){
            found_flag = true;
        }

	};
}