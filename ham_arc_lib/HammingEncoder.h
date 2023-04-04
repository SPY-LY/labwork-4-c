//
// Created by SPY_LY on 20.11.2022.
//
#include <iostream>

#ifndef TEST_LAB4_HAM_ARC_LIB_HAMMINGENCODER_H_
#define TEST_LAB4_HAM_ARC_LIB_HAMMINGENCODER_H_

class HammingEncoder{
 public:
    std::string decode(const std::string& portion, int64_t& size);
    std::string encode(const std::string& portion, int64_t& size);
    bool CheckCorrectness(std::string& str, int64_t size);
 private:
    int64_t size_of_block_to_read_ = 16;
    const int64_t byte_size = 8;
    void NextBit(int& ind_in_portion, int& bit_ind_in_portion) const;
    bool IsParityBit(int portion, int portion_1);
    void SetBit(char& char_to_set, int bit_ind_to, const char& char_from_set, int bit_ind_from);
    char GetBit(const char& char_from_set, int bit_ind_from) const;
    void SetBitToZero(char& i, int portion);
    void CoverAll(std::string& str, int byte_ind, int bit_ind);
};

#endif //TEST_LAB4_HAM_ARC_LIB_HAMMINGENCODER_H_
