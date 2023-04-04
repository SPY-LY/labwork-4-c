//
// Created by SPY_LY on 20.11.2022.
//

#include "ArchiveManipulator.h"
#include <cstring>
#include <utility>
#include <bitset>
ArchiveManipulator::ArchiveManipulator(std::filesystem::path archive) : archive_(std::move(archive)) {
    current_block_ = std::string(size_of_block_to_read_, '\0');
}
void ArchiveManipulator::Start(int64_t start_pointer, bool on_creation) {
    OpenStream(on_creation);
    pointer_of_block_ = 0;
    pointer_in_block_ = 0;
    GetPortion(false);
    Seekp(start_pointer);
    GetPortion(false);
}
void ArchiveManipulator::OpenStream(bool on_creation) {
    if (on_creation) {
        fstream_ = std::fstream(archive_, std::ios::out | std::ios::binary | std::ios::trunc);
        fstream_.close();
    }
    fstream_ = std::fstream(archive_, std::ios::binary | std::fstream::out | std::fstream::in);
}
void ArchiveManipulator::Seekp(int64_t pointer) {
    int64_t distance;
    if (pointer >= pointer_of_block_) {
        distance = (pointer - pointer_of_block_) / size_of_block_to_read_;
    } else {
        distance = (pointer - pointer_of_block_ + 1) / size_of_block_to_read_ - 1;
    }
    PutInFile(false);
    ChangePointerOfBlockBy(distance);
    pointer_in_block_ = pointer - pointer_of_block_;
    GetPortion(false);
}
void ArchiveManipulator::GetPortion(bool dont_decode) {
    fstream_.seekp(pointer_of_block_);
    size_t i = 0;
    for (; i < size_of_block_to_read_; i++) {
        int32_t x;
        x = fstream_.get();
        if (x == -1) {
            break;
        }
        current_block_[i] = x;
    }
    if (i != size_of_block_to_read_) {
        current_block_[i] = '\0';
    }
    real_block_size_ = i;
    if (!dont_decode) {
        current_block_ = hamming_encoder_.decode(current_block_, real_block_size_);
    }
}
int64_t ArchiveManipulator::Tellp() const {
    return pointer_of_block_ + pointer_in_block_;
}

void ArchiveManipulator::PutInFile(bool encodee) {
    fstream_.clear();
    fstream_.seekp(pointer_of_block_);

    if (!encodee) {
        current_block_ = hamming_encoder_.encode(current_block_, real_block_size_);
    }
    for (size_t i = 0; i < real_block_size_; i++) {
        fstream_.put(current_block_[i]);
    }
}
void ArchiveManipulator::Write(const std::string& str) {
    for (char i : str) {
        Write(i);
    }
}
void ArchiveManipulator::ChangePointerOfBlockBy(int64_t distance) {
    pointer_of_block_ += distance * size_of_block_to_read_;
}
void ArchiveManipulator::Write(char c) {
    if (current_block_[pointer_in_block_] == '\0' && pointer_in_block_ + 1 > real_block_size_) {
        real_block_size_ = pointer_in_block_ + 1;
        current_block_[pointer_in_block_ + 1] = '\0';
    }
    current_block_[pointer_in_block_++] = c;
    if (pointer_in_block_ == max_size_of_block_) {
        PutInFile(false);
        ChangePointerOfBlockBy(1);
        GetPortion(false);
        pointer_in_block_ = 0;
    }
}

void ArchiveManipulator::Write(int64_t c) {
    for (size_t i = 0; i < sizeof(int64_t); i++) {
        Write(char((255 & (c >> ((sizeof(int64_t) - 1) * 8)))));
        c <<= 8;
    }
}

int64_t ArchiveManipulator::GetInt64_t() {
    int64_t return_value = 0;
    for (size_t i = 0; i < sizeof(int64_t); i++) {
        return_value <<= 8;
        return_value += (unsigned char)Get();
    }
    return return_value;
}
char ArchiveManipulator::Get() {
    char value = current_block_[pointer_in_block_++];
    if (pointer_in_block_ == max_size_of_block_) {
        PutInFile(false);
        ChangePointerOfBlockBy(1);
        GetPortion(false);
        pointer_in_block_ = 0;
    }
    return value;
}
bool ArchiveManipulator::Eof() const {
    return pointer_in_block_ == real_block_size_ && fstream_.eof();
}
std::string ArchiveManipulator::Get(size_t cnt) {
    std::string str;
    for (size_t i = 0; i < cnt; i++) {
        str += Get();
    }
    return str;
}
void ArchiveManipulator::Rename(const std::string& new_name) {
    std::filesystem::rename(archive_, archive_.parent_path() / (new_name));
}
void ArchiveManipulator::Delete() const {
    std::filesystem::remove(archive_);
}

void ArchiveManipulator::End() {
    PutInFile(false);
    fstream_.close();
}
bool ArchiveManipulator::CheckCorrectness() {
    bool return_value = true;
    OpenStream(false);
    pointer_of_block_ = 0;
    pointer_in_block_ = 0;
    GetPortion(true);
    while (true) {
        bool this_correctness = hamming_encoder_.CheckCorrectness(current_block_, real_block_size_);
        if (!this_correctness) {
            PutInFile(true);
        }
        return_value = return_value && this_correctness;
        if (Eof()) {
            break;
        }
        ChangePointerOfBlockBy(1);
        GetPortion(true);
    }
    End();
    return return_value;
}
void ArchiveManipulator::Damage() {
    fstream_.clear();
    fstream_.seekp(pointer_of_block_);
    current_block_ = hamming_encoder_.encode(current_block_, real_block_size_);
    for (size_t i = 0; i < real_block_size_; i++) {
        if (i == 4) {
            std::cout << "was " << std::bitset<8>(current_block_[i]) << " ";
            std::cout << std::endl;
            if (((current_block_[i] >> 4) & 1) == 1) {
                current_block_[i] = ((~(1 << 4)) & current_block_[i]);
            }
            else {
                current_block_[i] = (((1 << 4)) | current_block_[i]);
            }
            std::cout << "became " << std::bitset<8>(current_block_[i]) << " ";
            std::cout << std::endl;
        }
        fstream_.put(current_block_[i]);
    }
    for (int i = 0; i < current_block_.size(); i++) {
        std::cout << std::bitset<8>(current_block_[i]) << "";
    }
    std::cout << std::endl;
}
