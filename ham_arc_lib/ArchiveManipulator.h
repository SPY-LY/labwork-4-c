//
// Created by SPY_LY on 20.11.2022.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include "HammingEncoder.h"
#ifndef TEST_LAB4_HAM_ARC_LIB_ARCHIVEMANIPULATOR_H_
#define TEST_LAB4_HAM_ARC_LIB_ARCHIVEMANIPULATOR_H_

class ArchiveManipulator {
 public:
    ArchiveManipulator() = default;
    explicit ArchiveManipulator(std::filesystem::path  archive);
    void Start(int64_t start_pointer = 0, bool on_creation = false);
    void End();
    void Seekp(int64_t pointer);
    int64_t Tellp() const;
    void Write(const std::string& str);
    void Write(char c);
    int64_t GetInt64_t();
    char Get();
    std::string Get(size_t cnt);
    bool Eof() const;
    void Rename(const std::string& new_name);
    void Delete() const;
    void Write(int64_t c);
    bool CheckCorrectness();
    int64_t GetBlockSize() {
        return size_of_block_to_read_;
    }
    void Damage();
 private:
    std::fstream fstream_;
    std::string current_block_;
    std::filesystem::path archive_;
    int64_t pointer_of_block_;
    int64_t pointer_in_block_;
    int64_t size_of_block_to_read_ = 16;
    int64_t max_size_of_block_ = 15;
    int64_t real_block_size_ = 0;
    void GetPortion(bool dont_decode);
    void PutInFile(bool encodee);
    HammingEncoder hamming_encoder_;
    void ChangePointerOfBlockBy(int64_t distance);
    void OpenStream(bool on_creation);
};

#endif //TEST_LAB4_HAM_ARC_LIB_ARCHIVEMANIPULATOR_H_
