#include <string>
#include <cstdio>

class NumericReader : public IReader {
public:
    int points_in_digit;
    NumericReader() {
        points_in_digit = 0;
    }

    bool errorExists() override { return points_in_digit > 1; }
    void reset() override { this->points_in_digit = 0; }

    bool canStartParsing(std::string source, int& idx) override {
        return isdigit(source[idx]);
    }

    bool read(std::string source, int& idx, std::string& buffer) override {       
        char ch = source[idx];
        if (isdigit(ch)) {
            buffer += ch;
            ++idx;
            return true;
        } else if (ch == '.') {
            buffer += ch;
            this->points_in_digit += 1;
            if (errorExists())
                return false;
            ++idx;
            return true;
        } else {
            return false;
        }
    }

    std::string getTokenType(std::string value) override {
        return (this->points_in_digit == 0 ? "int" : "float");
    }
};