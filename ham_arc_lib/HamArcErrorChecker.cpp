//
// Created by SPY_LY on 22.11.2022.
//

#include "HamArcErrorChecker.h"

#include <utility>
void HamArcErrorChecker::TestFilesExistence(const std::vector<std::filesystem::path>& files) {
    if (!std::all_of(files.begin(), files.end(), [](const std::filesystem::path& file) {
        return exists(file);
    })) {
        std::cerr << "files existence error";
        throw HamArcException("wrong files arguments");
    }
    if (files.empty()) {
        std::cerr << "files existencece error";
        throw HamArcException("There must me at least 1 file");
    }
}
void HamArcErrorChecker::TestArchiveName()  const{
    std::cout << archive_ << std::endl;
    auto file_name = archive_.filename().string();
    if (!(file_name.size() > 4 && file_name.substr(file_name.size() - 4) == ".ham")){
        throw HamArcException("wrong archive name");
    }
}
void HamArcErrorChecker::TestArchiveExistence(bool need)  const{
    std::cerr << archive_ << std::endl;
     if ((need && !exists(archive_))) {
         std::cerr << archive_.string() + " archive already exist";
        throw HamArcException(archive_.string() + "archive does not exist");
    }
    if ((!need && exists(archive_))) {
        std::cerr << archive_.string() + " archive already exist";
        throw HamArcException(archive_.string() + "archive already exist");
    }
}
void HamArcErrorChecker::TestArchiveCreation() const {
    create_directories(archive_.parent_path());
}
void HamArcErrorChecker::TestFilesCorrectness(const std::vector<std::filesystem::path>& files) {
    if (!std::all_of(files.begin(), files.end(), [](const std::filesystem::path& file) {
        return is_regular_file(file);
    })) {
        throw HamArcException("Wrong file names");
    }
}

void HamArcErrorChecker::TestExistenceFileInList(const std::vector<std::filesystem::path>& files,
                                                 const std::string& file_name) {
    if (!std::any_of(files.begin(), files.end(), [file_name](const std::filesystem::path& file) {
        return file.filename().string() == file_name;
    })) {
        throw HamArcException("remove does not ended well. File name are not correct");
    }
}
HamArcErrorChecker::HamArcErrorChecker(const std::filesystem::path& archive) {
    std::cout << "!" << std::endl;
    std::cout << archive.string() << std::endl;
    archive_ = archive;
    std::cout << archive_.string() << std::endl;
}
HamArcErrorChecker::HamArcErrorChecker() = default;


