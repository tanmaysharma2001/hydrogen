#include <iostream>
#include <vector>
#include <string>

#include "../token_analyzer/manager.cpp"

int main(int argc, char** argv) {
    std::string file = argv[1];
    
    Manager manager;
    std::string source;
    
    freopen(file.c_str(), "r", stdin);
    
    std::string line;
    while (std::getline(std::cin, line)) 
        source += line;
    
    manager.Analyze(source);
}