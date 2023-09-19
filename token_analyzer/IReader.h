#include <string>

class IReader {
public:
    virtual bool errorExists() = 0;
    virtual void reset() = 0;
    virtual bool canStartParsing(std::string source, int& idx) = 0;
    virtual bool read(std::string source, int& idx, std::string& buffer) = 0;
    virtual std::string getTokenType(std::string value) = 0;
};