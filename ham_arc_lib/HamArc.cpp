//
// Created by SPY_LY on 20.11.2022.
//
#include <fstream>
#include <cmath>
#include "HamArc.h"
#include "HamArcErrorChecker.h"

void HamArc::Create(const std::vector<std::filesystem::path>& files) {
    error_checker_.TestFilesExistence(files);
    error_checker_.TestArchiveName();
    error_checker_.TestArchiveExistence(false);
    error_checker_.TestArchiveCreation();

    Header header(files.size());

    archive_manipulator_.Start(header.files[0].name_offset, true);
    WriteFromFilesToArchive(files, header);
    FillInfoFromHeader(header);
    archive_manipulator_.End();
}

// If empty list - maybe there's error occurred
std::vector<std::string> HamArc::GetFileList() {
    error_checker_.TestArchiveExistence();
    archive_manipulator_.Start();
    Header header(*this);
    std::vector<std::string> file_list(header.files_num);
    for (size_t i = 0; i < header.files.size(); i++) {
        archive_manipulator_.Seekp(header.files[i].name_offset);
        std::string file_name;
        for (; archive_manipulator_.Tellp() < header.files[i].content_offset; ) {
            file_name += archive_manipulator_.Get();
        }
        file_list[i] = file_name;
    }
    archive_manipulator_.End();
    return file_list;
}

void HamArc::ExtractFiles(const std::vector<std::filesystem::path>& files) {
    ExtractFiles(files, files.empty() ? true : false);
}

void HamArc::ExtractFiles(const std::vector<std::filesystem::path>& files, bool need_all) {
    error_checker_.TestArchiveExistence();

    archive_manipulator_.Start();
    Header header(*this);
    for (size_t i = 0; i < header.files.size(); i++) {
        archive_manipulator_.Seekp(header.files[i].name_offset);
        std::string file_name;
        for (; archive_manipulator_.Tellp() < header.files[i].content_offset; ) {
            file_name += archive_manipulator_.Get();
        }
        if (!ExistFileInFileList(files, file_name) && !need_all) {
            continue;
        }
        std::ofstream ofstream(file_name);

        while (!(EndOfFileInArchive(header, i, this))) {
            ofstream << archive_manipulator_.Get();
        }
    }
    archive_manipulator_.End();
}

void HamArc::OutputEveryCharacter(const std::vector<std::filesystem::path>& files) {
    error_checker_.TestArchiveExistence();

    archive_manipulator_.Start();
    while (!archive_manipulator_.Eof()) {
        char c = archive_manipulator_.Get();
        std::cout << c << " " << int(c) << std::endl;
    }
    archive_manipulator_.End();
}

bool HamArc::EndOfFileInArchive(const Header& header, size_t i, HamArc* arc) const {
    return arc->archive_manipulator_.Eof()
            || (i + 1 < header.files_num
                    && arc->archive_manipulator_.Tellp() == header.files[i + 1].name_offset);
}

HamArc HamArc::Append(const std::vector<std::filesystem::path>& files) {
    error_checker_.TestArchiveExistence();
    HamArc new_arc(archive_.string() + "t");
    new_arc.CreateFromOldWithFiles(files, this);
    archive_manipulator_.End();
    archive_manipulator_.Delete();
    new_arc.archive_manipulator_.Rename(archive_.filename().string());
    return new_arc;
}

void HamArc::CreateFromOldWithFiles(const std::vector<std::filesystem::path>& files, HamArc* old_arc) {
    error_checker_.TestArchiveName();
    error_checker_.TestArchiveCreation();
    error_checker_.TestArchiveExistence(false);
    error_checker_.TestFilesExistence(files);

    old_arc->archive_manipulator_.Start();
    Header old_header(*old_arc);
    Header header(files.size() + old_header.files_num);

    archive_manipulator_.Start(header.files[0].name_offset, true);
    RewriteToNewArchive(*old_arc, old_header, header);
    WriteFromFilesToArchive(files, header, old_header.files_num);
    FillInfoFromHeader(header);
    archive_manipulator_.End();
    old_arc->archive_manipulator_.End();
}

void HamArc::CreateFromOldWithoutFiles(const std::vector<std::filesystem::path>& files, HamArc* old_arc) {
    error_checker_.TestFilesCorrectness(files);
    error_checker_.TestArchiveName();
    error_checker_.TestArchiveExistence(false);
    error_checker_.TestArchiveCreation();

    old_arc->archive_manipulator_.Start();
    Header old_header(*old_arc);
    Header header(old_header.files_num - files.size());
    archive_manipulator_.Start(header.files[0].name_offset, true);
    for (size_t i = 0, index_in_old_archive = 0;; index_in_old_archive++) {
        if (i >= header.files.size()) {
            if (index_in_old_archive == old_header.files_num) {
                break;
            }
            std::string file_name = GetFileName(old_header.files[index_in_old_archive], old_arc);
            error_checker_.TestExistenceFileInList(files, file_name);
        }
        header.files[i].name_offset = archive_manipulator_.Tellp();
        std::string file_name = GetFileName(old_header.files[index_in_old_archive], old_arc);
        if (!ExistFileInFileList(files, file_name)) {
            archive_manipulator_.Write(file_name);
            header.files[i].content_offset = archive_manipulator_.Tellp();

            old_arc->archive_manipulator_.Seekp(old_header.files[index_in_old_archive].content_offset);
            while (!EndOfFileInArchive(old_header, index_in_old_archive, old_arc)) {
                archive_manipulator_.Write(static_cast<char>(old_arc->archive_manipulator_.Get()));
            }
            i++;
        }
    }
    FillInfoFromHeader(header);
    archive_manipulator_.End();
    old_arc->archive_manipulator_.End();
}
bool HamArc::ExistFileInFileList(const std::vector<std::filesystem::path>& files, const std::string& file_name) const {
    return std::any_of(files.begin(), files.end(), [file_name](const std::filesystem::path& file) {
        return file.filename().string() == file_name;
    });
}
HamArc HamArc::Delete(const std::vector<std::filesystem::path>& files) {
    error_checker_.TestArchiveExistence();
    HamArc new_arc(archive_.string() + "t");
    new_arc.CreateFromOldWithoutFiles(files, this);
    archive_manipulator_.End();
    archive_manipulator_.Delete();
    new_arc.archive_manipulator_.Rename(archive_.filename().string());
    return new_arc;
}
void HamArc::Concatenate(HamArc& arc1, HamArc& arc2) {
    std::cout << "aaa" << std::endl;
    arc1.error_checker_.TestArchiveExistence();
    std::cout << "bbb" << std::endl;
    arc2.error_checker_.TestArchiveExistence();
    error_checker_.TestArchiveName();
    std::cout << "ccc" << std::endl;
    error_checker_.TestArchiveExistence(false);
    error_checker_.TestArchiveCreation();

    arc1.archive_manipulator_.Start();
    arc2.archive_manipulator_.Start();
    Header arc1_header(arc1);
    Header arc2_header(arc2);
    Header header(arc1_header.files_num + arc2_header.files_num);
    archive_manipulator_.Start(header.files[0].name_offset, true);
    RewriteToNewArchive(arc1, arc1_header, header);
    RewriteToNewArchive(arc2, arc2_header, header, arc1_header.files_num);
    FillInfoFromHeader(header);
    arc1.archive_manipulator_.End();
    arc2.archive_manipulator_.End();
    archive_manipulator_.End();
}
void HamArc::FillInfoFromHeader(const HamArc::Header& header) {
    archive_manipulator_.Seekp(0);
    archive_manipulator_.Write(header.files_num);
    for (size_t i = 0; i < header.files_num; i++) {
        archive_manipulator_.Write(header.files[i].name_offset);
        archive_manipulator_.Write(header.files[i].content_offset);
    }
}
void HamArc::RewriteToNewArchive(HamArc& other,
                                 const HamArc::Header& other_header,
                                 HamArc::Header& header,
                                 size_t index_from) {
    for (size_t i = 0; i < other_header.files_num; i++, index_from++) {
        header.files[index_from].name_offset = archive_manipulator_.Tellp();
        other.archive_manipulator_.Seekp(other_header.files[i].name_offset);
        archive_manipulator_.Write(other.archive_manipulator_.Get(
                other_header.files[i].content_offset - other_header.files[i].name_offset));
        header.files[index_from].content_offset = archive_manipulator_.Tellp();
        other.archive_manipulator_.Seekp(other_header.files[i].content_offset);
        while (!(other.archive_manipulator_.Eof() || (i + 1 < other_header.files_num
                && other.archive_manipulator_.Tellp() == other_header.files[i + 1].name_offset))) {
            archive_manipulator_.Write(static_cast<char>(other.archive_manipulator_.Get()));
        }
    }
}

void HamArc::WriteFromFilesToArchive(const std::vector<std::filesystem::path>& files,
                                     HamArc::Header& header,
                                     size_t index_from) {
    for (size_t i = index_from; i - index_from < files.size(); i++) {
        header.files[i].name_offset = archive_manipulator_.Tellp();
        archive_manipulator_.Write(files[i - index_from].filename().string());
        header.files[i].content_offset = archive_manipulator_.Tellp();
        std::ifstream file_stream(files[i - index_from], std::ios::binary);
        while (true) {
            int32_t x = file_stream.get();
            if (x == -1) {
                break;
            }
            archive_manipulator_.Write(static_cast<char>(x));
        }
    }
}
std::string HamArc::GetFileName(const FileInfo& info, HamArc* arc) const {

    arc->archive_manipulator_.Seekp(info.name_offset);
    return arc->archive_manipulator_.Get(info.content_offset
                                                 - info.name_offset);

}
void HamArc::Test() {
//    archive_manipulator_.Test();
}
HamArc::HamArc(const std::string& archive_name) : archive_(std::filesystem::path(archive_name)),
                                                  error_checker_(std::filesystem::path(archive_name)),
                                                  archive_manipulator_(archive_) {

}
bool HamArc::CheckCorrectness() {
    return archive_manipulator_.CheckCorrectness();
}
void HamArc::Damage(const std::vector<std::filesystem::path>& files) {
    error_checker_.TestArchiveExistence();

    archive_manipulator_.Start();
    Header header(*this);
    for (size_t i = 0; i < header.files.size(); i++) {
        archive_manipulator_.Seekp(header.files[i].name_offset);
        std::string file_name;
        for (; archive_manipulator_.Tellp() < header.files[i].content_offset; ) {
            file_name += archive_manipulator_.Get();
        }
        if (!ExistFileInFileList(files, file_name)) {
            continue;
        }
        archive_manipulator_.Damage();
    }
}

HamArc::FileInfo::FileInfo(int64_t name_offset, int64_t content_offset)
        : name_offset(name_offset), content_offset(content_offset) {}
HamArc::FileInfo::FileInfo() {
    name_offset = 0;
    content_offset = 0;
}

HamArc::Header::Header(int64_t files_num) : files_num(files_num) {
    InitiateHeader();
}
void HamArc::Header::InitiateHeader() {
    files = std::vector<FileInfo>(files_num);
    files[0].name_offset = std::floor((files_num * kHelpInfoCount + 1) * (sizeof(int64_t)) / 16) + (files_num * kHelpInfoCount + 1) * (sizeof(int64_t));
}
HamArc::Header::Header(HamArc& ham_arc) {
    files_num = ham_arc.archive_manipulator_.GetInt64_t();
    files = std::vector<FileInfo>(files_num);
    for (size_t i = 0; i < files_num; i++) {
        files[i].name_offset = ham_arc.archive_manipulator_.GetInt64_t();
        files[i].content_offset = ham_arc.archive_manipulator_.GetInt64_t();
    }
}
