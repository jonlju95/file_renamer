#include <algorithm>
#include <iostream>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const std::set<std::string> MEDIA_EXTENSIONS = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".webp", ".heic", ".heif", ".raw",
    ".mp4", ".mov", ".avi", ".mkv", ".wmv", ".flv", ".webm", ".m4v", ".mpeg", ".mpg"
};

void processDirectory(const fs::path &dirPath) {
    std::vector<fs::path> filePaths;
    for (auto const &entry: fs::directory_iterator(dirPath)) {
        std::string extension = entry.path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (entry.is_regular_file() && MEDIA_EXTENSIONS.count(extension) > 0) {
            filePaths.push_back(entry.path());
        }
    }

    std::sort(filePaths.begin(), filePaths.end(), [](const fs::path &a, const fs::path &b) {
        return fs::last_write_time(a) < fs::last_write_time(b);
    });

    std::vector<fs::path> tempFilePaths;
    int counter = 1;
    for (auto const &sortedFile: filePaths) {
        std::string ext = sortedFile.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        std::string newName = "tmp_" + std::to_string(counter) + ext;
        fs::path newPath = dirPath / newName;


        if (fs::exists(newPath) && newPath != sortedFile) {
            std::cout << "[SKIP] " << sortedFile.filename().string() << " -> " << newName << " (already exists)\n";
            counter++;
            continue;
        }

        if (newPath == sortedFile) {
            std::cout << "[OK]   " << sortedFile.filename().string() << " (already correct)\n";
            counter++;
            continue;
        }


        try {
            tempFilePaths.push_back(newPath);
            fs::rename(sortedFile, newPath);
            std::cout << "[OK]   " << sortedFile.filename().string()
                    << " -> " << newName << "\n";
        } catch (const fs::filesystem_error &e) {
            std::cerr << "[ERR]  " << sortedFile.filename().string()
                    << ": " << e.what() << "\n";
        }

        counter++;
    }

    counter = 1;
    for (auto const &tempFile: tempFilePaths) {
        std::string ext = tempFile.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        std::string newName = "img_" + std::to_string(counter) + ext;
        fs::path newPath = dirPath / newName;

        fs::rename(tempFile, newPath);
        counter++;
    }
}


int main() {
    const fs::path directoryPath = ""; // Directory path
    processDirectory(directoryPath);

    return 0;
}