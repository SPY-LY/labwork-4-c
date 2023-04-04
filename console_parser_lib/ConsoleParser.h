//
// Created by SPY_LY on 11.10.2022.
//

#ifndef TEST_LAB3_CONSOLEPARSER_H
#define TEST_LAB3_CONSOLEPARSER_H

#include <variant>
#include <iostream>
#include <vector>
#include <unordered_map>

using variant = std::variant<std::monostate,
                             bool,
                             uint16_t,
                             uint32_t,
                             int32_t,
                             uint64_t,
                             std::string,
                             std::vector<std::string>,
                             std::pair<std::string, std::string>>;

enum class Behavior {
    kCheckout,
    kWaitForString,
    kWaitForPath,
    kWaitForFile,
    kWaitForUint16_t,
    kWaitForInt32_t,
    kWaitForUint32_t,
    kWaitForUint64_t,
    kWaitForFileInsideOrSepareted,
    kWaitForPair
};

struct Flag {

    Flag(std::string long_version, std::string short_version, Behavior expected_behavior);

    Flag(std::string long_version, std::string short_version, Behavior expected_behavior,
         variant default_data);

    friend std::ostream& operator<<(std::ostream& os, const Flag& flag);

    std::string long_version;
    std::string short_version;
    Behavior expected_behavior;
    variant data;
    bool required;
};

class ConsoleParser {
 public:

    ConsoleParser(int32_t argc, char** argv, std::vector<Flag> flags);

    std::vector<std::string> GetFreeArguments();

    bool Init();

    variant Extract(const std::string& long_version) const;

    variant Extract(const Behavior& behavior);

    friend std::ostream& operator<<(std::ostream& os, const ConsoleParser& parser);

    std::variant<std::monostate, variant> OptionalExtract(const std::string& string);
    bool ExtractBool(const std::string& string);
 private:

    void ReadNextOption();

    void CheckRead();

    void determineOption(const char* test_option, const char* help_info = nullptr);

    bool CheckExistFlag(const std::string& flag_name);

    void ProccessOption(size_t flag_index, const char* help_info);

    std::string ReadNextString();

    uint16_t ReadNextUint16_t();

    int32_t ReadNextInt32_t();

    uint64_t ReadNextUint64_t();

    uint32_t ReadNextUint32_t();

    bool TestInput();

    bool EndOfConsole() const {
        return ind_ >= argc_;
    }

    int32_t argc_;
    char** argv_;
    std::vector<Flag> flags_;
    std::vector<std::string> free_arguments_;
    std::unordered_map<std::string, std::string> options_;
    std::unordered_map<std::string, size_t> correspondence_;
    size_t ind_;
    bool options_determined_;
};

#endif //TEST_LAB3_CONSOLEPARSER_H
