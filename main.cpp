#include <filesystem>

#include "console_parser_lib/ConsoleParser.h"
#include "ham_arc_lib/HamArcErrorChecker.h"
#include "ham_arc_lib/HamArc.h"
#include "ham_arc_lib/HammingEncoder.h"
#include <bitset>
//
// Created by SPY_LY on 20.11.2022.
//
std::vector<std::filesystem::path> GetFiles(ConsoleParser& console_parser) {
    auto files = console_parser.GetFreeArguments();
    std::vector<std::filesystem::path> files_paths;
    std::transform(files.cbegin(), files.cend(), std::back_inserter(files_paths), [](std::string str) {
        return std::filesystem::path(str);
    });
    return files_paths;
}
void Test() {
    HammingEncoder encoder;
    std::string abc = std::string(15, '\0');
    abc[0] = 'a';"a\0\nb01234";
    abc[2] = '\n';
    abc[3] = 'b';
    abc[4] = '0';
    abc[5] = '1';
    abc[6] = '2';
    abc[7] = '3';
    abc[8] = '4';
    abc[14] = '5';
    abc[15] = '6';
    int64_t size_abc = 15;
    for (int i = 0; i < size_abc; i++) {
        std::cout << std::bitset<8>(abc[i]) << " ";
    }
    std::cout << std::endl;
    std::string abc_after_encode = encoder.encode(abc, size_abc);
    for (int i = 0; i < size_abc; i++) {
        std::cout << std::bitset<8>(abc_after_encode[i]) << " ";
    }
    std::cout << std::endl;
    std::string abc_after_decode = encoder.decode(abc_after_encode, size_abc);
    for (int i = 0; i < size_abc; i++) {
        std::cout << std::bitset<8>(abc_after_decode[i]) << " ";
    }
    std::cout << std::endl;
}
int main(int argc, char** argv) {
    std::vector<Flag> expected_flags = {{"--create",      "-c", Behavior::kCheckout, false},
                                        {"--file",        "-f", Behavior::kWaitForFileInsideOrSepareted},
                                        {"--list",        "-l", Behavior::kCheckout, false},
                                        {"--extract",     "-x", Behavior::kCheckout, false},
                                        {"--append",      "-a", Behavior::kCheckout, false},
                                        {"--delete",      "-d", Behavior::kCheckout, false},
                                        {"--concatenate", "-A", Behavior::kWaitForPair, std::monostate()},
                                        {"--correctness", "-C", Behavior::kCheckout, false}};

    ConsoleParser console_parser = ConsoleParser(argc, argv, expected_flags);
    bool succeed = console_parser.Init();
    if (!succeed) {
        std::cout << "Ooops, wrong console arguments";
        return 0;
    }
    auto archive_name = std::get<std::string>(console_parser.Extract("--file"));

    HamArc archive((archive_name));
    std::vector<std::filesystem::path> files_paths = GetFiles(console_parser);
    try {
        auto create_data = console_parser.Extract("--create");
        if (std::get<bool>(create_data)) {
            archive.Create(files_paths);
        }

        if (std::get<bool>(console_parser.Extract("--correctness"))) {
//            archive.Damage(files_paths);
            bool correct = archive.CheckCorrectness();
            if (!correct) {
                std::cout << std::endl << "archive is corrupted but was repaired. There's still chance that your files are too corrupted that this archive can not restore them";
            }
            else {
                std::cout << std::endl << "archive is correct";
            }
        }

        if (std::get<bool>(console_parser.Extract("--list"))) {
            auto list = archive.GetFileList();
            std::cout << "archive files:" << list.size() << std::endl;
            for (auto & i : list) {
                std::cout << " " << i << std::endl;
            }
        }

        if (std::get<bool>(console_parser.Extract("--extract"))) {
            archive.ExtractFiles(files_paths);
        }

        if (std::get<bool>(console_parser.Extract("--append"))) {
            archive.Append(files_paths);

        }

        if (std::get<bool>(console_parser.Extract("--delete"))) {
            archive.Delete(files_paths);
        }

        auto x = console_parser.Extract("--concatenate");
        if (x.index() != 0) {
            auto first_arc = HamArc(std::get<std::pair<std::string, std::string>>(x).first);
            auto second_arc = HamArc(std::get<std::pair<std::string, std::string>>(x).second);
            archive.Concatenate(first_arc, second_arc);
        }

    }
    catch (HamArcErrorChecker::HamArcException& e){
        std::cout << "error occured:" <<std::endl << "  ";
        std::cout << e.what();
    }

}

// --file=my_archive --append "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\ebebev2.txt"
// --file=my_archive --create "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\aboba.txt"
// --file=my_archive --list
// --file=my_archive --extract "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\aboba.txt"
// --file=my_archive --delete "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\aboba.txt"

// --file=my_archivev2 --create "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\farc2.txt" "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\farc.txt"
// --file=my_concatenated_archive -A "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\my_archive.haf" "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\my_archivev2.haf"
// -f "D:\University stuff\labs\labwork-4-SPY-LY\cmake-build-debug\archive_folder\my_concatenated_archive.haf" --extract

// --create --file="D:\University stuff\labs\labwork-4-SPY-LY\test\arch.ham" "D:\University stuff\labs\labwork-4-SPY-LY\test\cfg.cfg" "D:\University stuff\labs\labwork-4-SPY-LY\test\coolest_meme.jpg" --list --extract

// --create --file="D:\University stuff\labs\labwork-4-SPY-LY\testv2\arc.ham" "D:\University stuff\labs\labwork-4-SPY-LY\testv2\dearc\cfg.cfg" --list
// --file="D:\University stuff\labs\labwork-4-SPY-LY\testv2\arc.ham" "D:\University stuff\labs\labwork-4-SPY-LY\testv2\dearc\aaa.txt" --correctness --extract
