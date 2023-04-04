//
// Created by SPY_LY on 11.10.2022.
//

#include "ConsoleParser.h"

#include <cstring>
#include <filesystem>
#include <utility>

Flag::Flag(std::string long_version, std::string short_version, Behavior expected_behavior)
        : long_version(std::move(long_version)), short_version(std::move(short_version)),
          expected_behavior(expected_behavior),
          required(true) {}

Flag::Flag(std::string long_version, std::string short_version, Behavior expected_behavior, variant default_data)
        : long_version(std::move(long_version)), short_version(std::move(short_version)),
          expected_behavior(expected_behavior), data(std::move(default_data)),
          required(false) {
}

std::ostream& operator<<(std::ostream& os, const std::monostate& mono) {
    os << "no value";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector) {
    os << "{";
    for (const T& el : vector) {
        os << el << " ";
    }
    os << "}";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::pair<T, T>& pair) {
    os << "{";
    os << pair.first << ", " << pair.second;
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Flag& flag) {
    os << "long_version: " << flag.long_version << " short_version: " << flag.short_version << " data: ";
    std::visit([&os](const auto& t) {
        os << t << std::endl;
    }, flag.data);
    return os;
}

ConsoleParser::ConsoleParser(int32_t argc, char** argv, std::vector<Flag> flags)
        : argc_(argc), argv_(argv),
          flags_(std::move(flags)),
          options_(),
          correspondence_(),
          ind_(1),
          options_determined_(false), free_arguments_() {
    for (size_t i = 0; i < flags_.size(); i++) {
        correspondence_[flags_[i].long_version] = i;
        correspondence_[flags_[i].short_version] = i;
    }
}

bool ConsoleParser::Init() {
    while (!EndOfConsole()) {
        ReadNextOption();
    }
    return TestInput();
}
variant ConsoleParser::Extract(const std::string& long_version) const {
    for (const Flag& flag : flags_) {
        if (flag.long_version == long_version) {
            return (flag.data);
        }
    }
    exit(16);
}
variant ConsoleParser::Extract(const Behavior& behavior) {
    for (const Flag& flag : flags_) {
        if (flag.expected_behavior == behavior) {
            return (flag.data);
        }
    }
    exit(16);
}

void ConsoleParser::ReadNextOption() {
    if (argv_[ind_][0] == '-') {
        determineOption(argv_[ind_++]);
    } else {
        free_arguments_.emplace_back(argv_[ind_++]);
    }
}

void ConsoleParser::determineOption(const char* test_option, const char* help_info) {
    options_determined_ = true;
    std::vector<size_t> parsedTestOptionsIndex;
    if (strlen(test_option) >= 2 && test_option[0] == test_option[1] && test_option[0] == '-') {
        if (!CheckExistFlag(test_option)) {
            for (size_t i = 0; i < strlen(test_option); i++) {
                if (test_option[i] == '=') {
                    char* parsed = new char[i + 1];
                    strncpy(parsed, test_option, i);
                    parsed[i] = '\0';
                    determineOption(parsed, test_option + i + 1);
                    return;
                }
            }
        }
        parsedTestOptionsIndex.push_back(correspondence_[test_option]);
    } else if (strlen(test_option) >= 2 && test_option[0] == '-') {
        auto limit = strlen(test_option);
        for (size_t i = 1; i < limit; i++) {
            std::cerr << test_option[i] << std::endl;
            std::string to_test = "-";
            to_test += test_option[i];
            if (!CheckExistFlag(to_test)) {
                exit(12);
            }
            parsedTestOptionsIndex.push_back(correspondence_[to_test]);
        }
    } else if (test_option[0] != '-') {
        help_info = test_option;
    } else {
        exit(40);
    }
    for (auto optionIndex : parsedTestOptionsIndex) {
        ProccessOption(optionIndex, help_info);
    }
}

bool ConsoleParser::CheckExistFlag(const std::string& flag_name) {
    std::cout << "@" << flag_name << "@";
    if (correspondence_.find(flag_name) == correspondence_.end()) {
        return false;
    }
    return true;
}

void ConsoleParser::ProccessOption(size_t flag_index, const char* help_info) {
    Flag& flag = flags_[flag_index];
    switch (flag.expected_behavior) {
        case Behavior::kCheckout:
            flag.data = true;
            break;
        case Behavior::kWaitForPath:
        case Behavior::kWaitForFile:
        case Behavior::kWaitForString:
            flag.data = ReadNextString();
            break;
        case Behavior::kWaitForUint16_t:
            flag.data = ReadNextUint16_t();
            break;
        case Behavior::kWaitForInt32_t:
            flag.data = ReadNextInt32_t();
            break;
        case Behavior::kWaitForUint32_t:
            flag.data = ReadNextUint32_t();
            break;
        case Behavior::kWaitForUint64_t:
            flag.data = ReadNextUint64_t();
            break;
        case Behavior::kWaitForFileInsideOrSepareted:
            if (help_info == nullptr) {
                flag.data = ReadNextString();
            } else {
                flag.data = help_info;
            }
            break;
        case Behavior::kWaitForPair:
            flag.data = std::pair<std::string, std::string>{ReadNextString(), ReadNextString()};
            break;
    }

}

std::string ConsoleParser::ReadNextString() {
    CheckRead();
    return argv_[ind_++];
}

void ConsoleParser::CheckRead() {
    if (EndOfConsole()) {
        exit(17);
    }
}

uint16_t ConsoleParser::ReadNextUint16_t() {
    uint32_t num = std::stoi(ReadNextString());
    if (std::numeric_limits<uint16_t>::max() < num) {
        exit(14);
    }
    return num;
}

int32_t ConsoleParser::ReadNextInt32_t() {
    int32_t num = std::stoi(ReadNextString());
    return num;
}

uint64_t ConsoleParser::ReadNextUint64_t() {
    uint64_t num = std::stoull(ReadNextString());
    return num;
}

uint32_t ConsoleParser::ReadNextUint32_t() {
    uint64_t num = std::stoul(ReadNextString());
    if (std::numeric_limits<uint32_t>::max() < num) {
        exit(15);
    }
    return num;
}

bool ConsoleParser::TestInput() {
    return std::all_of(flags_.begin(), flags_.end(), [](const Flag& flag) {
        return !flag.required || flag.data.index() != 0;
    });
}

template<typename T, typename M>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<T, M>& unordered_map) {
    os << std::endl << '{';
    for (const auto& x : unordered_map) {
        os << '{' << x.first << ", " << x.second << "}, ";
    }
    os << '}' << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ConsoleParser& parser) {
    os << "options_: " << parser.options_ << " options_determined_: " << parser.options_determined_ << " ind_: "
       << parser.ind_
       << " argc_: " << parser.argc_ << " argv_: " << parser.argv_ << " correspondence_: " << parser.correspondence_
       << " flags_: " << parser.flags_;
    return os;
}

std::variant<std::monostate, variant> ConsoleParser::OptionalExtract(const std::string& string) {
    for (const Flag& flag : flags_) {
        if (flag.long_version == string) {
            if (flag.data.index() == 0) {
                return std::monostate();
            }
            return flag.data;
        }
    }
    exit(16);
}
bool ConsoleParser::ExtractBool(const std::string& string) {
    return false;
}
std::vector<std::string> ConsoleParser::GetFreeArguments() {
    return free_arguments_;
}



