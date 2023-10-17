#include <string>
#include <functional>

class Node {
public:
    int tokenNumber;
    int errorLine;
    bool parsingError;

    std::function<void(std::string)> print; 

    Node() : print([](std::string) {}) { 
        this->parsingError = false;
        this->errorLine = -1;
    }

    void initializePrintFunction(std::function<void(std::string)> printFunc) { 
        print = printFunc;
    }

    void setTokenNumber(int tokenNumber) {
        this->tokenNumber = tokenNumber;
    }

    void setParsingError(bool parsingError) {
        this->parsingError = parsingError;
    }
};