#include <string>

class Node {
public:
    int tokenNumber;
    int errorLine;
    bool parsingError;

    Node() {
        this->parsingError = false;
        this->errorLine = -1;
    }

    void setTokenNumber(int tokenNumber) {
        this->tokenNumber = tokenNumber;
    }

    void setParsingError(bool parsingError) {
        this->parsingError = parsingError;
    }
};