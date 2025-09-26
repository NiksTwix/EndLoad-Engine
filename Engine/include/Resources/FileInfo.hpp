#pragma once
#include <string>
#include <filesystem>
#include <optional>

enum class FIStatus {
    FAILED,
    SUCCESS
};

class FileInfo {
public:
    explicit FileInfo(const std::string& path) noexcept {  // явное создание
        try {
            file_path_ = std::filesystem::path(path);

            if (!std::filesystem::exists(file_path_)) {
                status_ = FIStatus::FAILED;
                return;
            }

            size_bytes_ = std::filesystem::file_size(file_path_);
            status_ = FIStatus::SUCCESS;
        }
        catch (...) {
            status_ = FIStatus::FAILED;
        }
    }


    FIStatus GetStatus() const noexcept { return status_; }
    std::string GetFullPath() const { return file_path_.string(); }
    std::string GetExtension() const { return file_path_.extension().string(); }
    std::string GetStem() const { return file_path_.stem().string(); }
    uintmax_t GetSizeBytes() const noexcept { return size_bytes_; }


    bool IsValid() const noexcept { return status_ == FIStatus::SUCCESS; }

private:
    std::filesystem::path file_path_;
    uintmax_t size_bytes_ = 0;
    FIStatus status_ = FIStatus::FAILED;
};