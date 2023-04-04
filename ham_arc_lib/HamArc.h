//
// Created by SPY_LY on 20.11.2022.
//
#include <iostream>
#include <vector>
#include <filesystem>
#include "HammingEncoder.h"
#include "ArchiveManipulator.h"
#include <string>
#include <cstring>
#include "HamArcErrorChecker.h"

#ifndef TEST_LAB4_HAMARC_LIB_HAMARC_H_
#define TEST_LAB4_HAMARC_LIB_HAMARC_H_
/* Format:
 *  num_of_files file_1_offset_name file_1_offset_content file_2_offset ... file_${num_of_files}_offset (all offsets size_t numbers)*/
class HamArc {
 public:
    void OutputEveryCharacter(const std::vector<std::filesystem::path>& files);
    explicit HamArc(const std::string& archive_name);
    void Create(const std::vector<std::filesystem::path>& files);
    std::vector<std::string> GetFileList();
    void ExtractFiles(const std::vector<std::filesystem::path>& files);
    HamArc Append(const std::vector<std::filesystem::path>& files);
    HamArc Delete(const std::vector<std::filesystem::path>& files);
    void Concatenate(HamArc& arc1, HamArc& arc2);
    void Test();
    bool CheckCorrectness();
    void Damage(const std::vector<std::filesystem::path>& files_paths);
 private:
    HamArcErrorChecker error_checker_;
    struct FileInfo {
        int64_t name_offset;
        int64_t content_offset;
        FileInfo(int64_t pos, int64_t symbol_cnt);
        FileInfo();
    };
    struct Header {
        static const int32_t kHelpInfoCount = 2;
        int64_t files_num;
        std::vector<FileInfo> files;
        explicit Header(int64_t files_num);
        explicit Header(HamArc& ham_arc);
        void InitiateHeader();
    };
    void CreateFromOldWithFiles(const std::vector<std::filesystem::path>& files, HamArc* old_arc);
    std::filesystem::path archive_;
    ArchiveManipulator archive_manipulator_;
    void CreateFromOldWithoutFiles(const std::vector<std::filesystem::path>& files, HamArc* old_arc);

    void RewriteToNewArchive(HamArc& other, const Header& other_header, Header& header, size_t index_from = 0);
    void FillInfoFromHeader(const Header& header);
    void WriteFromFilesToArchive(const std::vector<std::filesystem::path>& files,
                                 Header& header,
                                 size_t index_from = 0);
    bool EndOfFileInArchive(const Header& header, size_t i, HamArc* arc) const;
    std::string GetFileName(const FileInfo& info, HamArc* arc) const;
    void ExtractFiles(const std::vector<std::filesystem::path>& files, bool need_all);
    bool ExistFileInFileList(const std::vector<std::filesystem::path>& files, const std::string& file_name) const;
};

#endif //TEST_LAB4_HAMARC_LIB_HAMARC_H_
