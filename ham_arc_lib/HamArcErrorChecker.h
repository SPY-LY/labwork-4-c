//
// Created by SPY_LY on 22.11.2022.
//
#include <iostream>
#include <filesystem>
#include <vector>
#include <cstring>
#ifndef TEST_LAB4_HAM_ARC_LIB_HAMARCERRORCHECKER_H_
#define TEST_LAB4_HAM_ARC_LIB_HAMARCERRORCHECKER_H_

class HamArcErrorChecker {
 public:
    HamArcErrorChecker();
    explicit HamArcErrorChecker(const std::filesystem::path& archive);
    static void TestFilesExistence(const std::vector<std::filesystem::path>& files);
    void TestArchiveName() const;
    void TestArchiveExistence(bool need = true) const;
    void TestArchiveCreation() const;
    static void TestFilesCorrectness(const std::vector<std::filesystem::path>& files);
    static void TestExistenceFileInList(const std::vector<std::filesystem::path>& files, const std::string& file_name);
    class HamArcException : public std::exception {

     public:
        explicit HamArcException(std::string message) {
            message_ = new char(message.size());
            strcpy(message_, message.c_str());
            };
        const char* what() {
            return message_;
        }
     private:
        char* message_;
    };
    std::filesystem::path archive_;
 private:
};

#endif //TEST_LAB4_HAM_ARC_LIB_HAMARCERRORCHECKER_H_
