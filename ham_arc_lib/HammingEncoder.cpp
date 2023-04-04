//
// Created by SPY_LY on 20.11.2022.
//
#include "HammingEncoder.h"
#include <bitset>
#include <vector>
std::string HammingEncoder::decode(const std::string& portion, int64_t& size) {
    std::string new_portion = std::string(size_of_block_to_read_, '\0');
    int ind_in_portion = 0, ind_in_new_portion = 0, bit_ind_in_new_portion = 0, bit_ind_in_portion = 0;
    for (; ind_in_portion != size; NextBit(ind_in_portion, bit_ind_in_portion)) {
        if (IsParityBit(ind_in_portion, bit_ind_in_portion)) {
            continue;
        }
        SetBit(new_portion[ind_in_new_portion], bit_ind_in_new_portion, portion[ind_in_portion], bit_ind_in_portion);
        NextBit(ind_in_new_portion, bit_ind_in_new_portion);
    }
        size = ind_in_new_portion;
    return new_portion;
}
void HammingEncoder::NextBit(int& ind_in_portion, int& bit_ind_in_portion) const {
    bit_ind_in_portion++;
    if (bit_ind_in_portion == byte_size) {
        bit_ind_in_portion = 0;
        ind_in_portion++;
    }
}
std::string HammingEncoder::encode(const std::string& portion, int64_t& size) {
    std::string new_portion = std::string(size_of_block_to_read_, '\0');
    int ind_in_portion = 0, ind_in_new_portion = 0, bit_ind_in_new_portion = 0, bit_ind_in_portion = 0;
    for (; ind_in_portion < size; NextBit(ind_in_new_portion, bit_ind_in_new_portion)) {
        if (IsParityBit(ind_in_new_portion, bit_ind_in_new_portion)) {
            SetBitToZero(new_portion[ind_in_new_portion], bit_ind_in_new_portion);
            CoverAll(new_portion, ind_in_new_portion, bit_ind_in_new_portion);
            continue;
        }
        SetBit(new_portion[ind_in_new_portion], bit_ind_in_new_portion, portion[ind_in_portion], bit_ind_in_portion);
        CoverAll(new_portion, ind_in_new_portion, bit_ind_in_new_portion);
        NextBit(ind_in_portion, bit_ind_in_portion);
    }
    if (size == 0) {

    }
    else if (bit_ind_in_new_portion == 0) {
        size = ind_in_new_portion;
    }
    else {
        size = ind_in_new_portion + 1;
    }
    return new_portion;
}
bool HammingEncoder::IsParityBit(int byte_num, int bit_num) {
    return !((byte_num * byte_size + bit_num + 1) & (byte_num * byte_size + bit_num));
}
void HammingEncoder::SetBit(char& char_to_set, int bit_ind_to, const char& char_from_set, int bit_ind_from) {
    char one_in_right_place = (1 << (byte_size - bit_ind_to - 1));
    char bit_in_right_place = (GetBit(char_from_set, bit_ind_from) << (byte_size - bit_ind_to - 1));
    char_to_set = ((char_to_set & (~one_in_right_place)) | bit_in_right_place);
}
char HammingEncoder::GetBit(const char& char_from, int bit_ind) const {
    char c = (char_from << bit_ind);
    char value_to_return = ((c >> (byte_size - 1)) & 1);
    return value_to_return;
}
void HammingEncoder::SetBitToZero(char& char_to_set, int bit_ind) {
    SetBit(char_to_set, bit_ind, 0, 0);
}
void HammingEncoder::CoverAll(std::string& str, int byte_ind, int bit_ind) {
    int real_parity_bit = 1;
    for (;real_parity_bit <= byte_ind * byte_size + bit_ind; real_parity_bit *= 2) {
        if (0 == (real_parity_bit & (byte_size * byte_ind + bit_ind + 1))){
            continue;
        }
        auto& char_to_set = str[(real_parity_bit - 1) / byte_size];
        auto bit_ind_to = (real_parity_bit - 1) % byte_size;
        auto get_bit_1 = GetBit(str[(real_parity_bit - 1) / byte_size], (real_parity_bit - 1) % byte_size);
        auto get_bit_2 = GetBit(str[byte_ind], bit_ind);
        auto char_from_set = char(get_bit_1
                ^ get_bit_2);
        auto bit_ind_from = byte_size - 1;
        SetBit(char_to_set, bit_ind_to, char_from_set, bit_ind_from);
    }
}
bool HammingEncoder::CheckCorrectness(std::string& str, int64_t size) {
    int real_parity_bit = 1;
    std::vector<int> not_correct_parity;
    for (;real_parity_bit <= size * byte_size; real_parity_bit *= 2) {
        int xor_ans = 0;
        for (int bit_to_test = real_parity_bit; bit_to_test <= size * byte_size; bit_to_test = ((bit_to_test + 1) | real_parity_bit)) {
            xor_ans ^= GetBit(str[(bit_to_test - 1) / byte_size], (bit_to_test - 1) % byte_size);
        }
        if (xor_ans != 0) {
            not_correct_parity.push_back(real_parity_bit);
        }
    }
    if (not_correct_parity.empty()) {
        return true;
    }
    int damage_index = 0;
    for (int i : not_correct_parity) {
        damage_index += i;
    }
    char damage_bit = (GetBit(str[(damage_index - 1) / byte_size], (damage_index - 1) % byte_size) ^ '\1');
    SetBit(str[(damage_index - 1) / byte_size], (damage_index - 1) % byte_size, damage_bit, byte_size - 1);
    return false;
}


/*for (int byte_ind = 0, bit_ind = 0; byte_ind != size; NextBit(byte_ind, bit_ind)) {
        for (int real_parity_bit = 1; real_parity_bit <= str.size() * byte_size; real_parity_bit *= 2) {
            if (0 == (real_parity_bit & (byte_size * byte_ind + bit_ind + 1))) {
                continue;
            }
            auto& char_to_set = str[(real_parity_bit - 1) / byte_size];
            auto bit_ind_to = (real_parity_bit - 1) % byte_size;
            auto get_bit_1 = GetBit(str[(real_parity_bit - 1) / byte_size], (real_parity_bit - 1) % byte_size);
            auto get_bit_2 = GetBit(str[byte_ind], bit_ind);
            auto char_from_set = char(get_bit_1
                                              ^ get_bit_2);
            auto bit_ind_from = byte_size - 1;
            SetBit(char_to_set, bit_ind_to, char_from_set, bit_ind_from);
        }
    }   */
