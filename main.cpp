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

std::string getExtension(const fs::path &path) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::vector<fs::path> renameToTemp(const std::vector<fs::path> &filePaths, const fs::path &dirPath) {
    std::vector<fs::path> tempFilePaths;
    int counter = 1;
    for (auto const &file: filePaths) {
        std::string newName = "tmp_" + std::to_string(counter) + getExtension(file);
        fs::path newPath = dirPath / newName;

        if (fs::exists(newPath) && newPath != file) {
            std::cout << "[SKIP] " << file.filename().string() << " -> " << newName << " (already exists)\n";
            counter++;
            continue;
        }

        if (newPath == file) {
            std::cout << "[OK]   " << file.filename().string() << " (already correct)\n";
            counter++;
            continue;
        }

        try {
            tempFilePaths.push_back(newPath);
            fs::rename(file, newPath);
            std::cout << "[OK]   " << file.filename().string()
                    << " -> " << newName << "\n";
        } catch (const fs::filesystem_error &e) {
            std::cerr << "[ERR]  " << file.filename().string()
                    << ": " << e.what() << "\n";
        }
        counter++;
    }
    return tempFilePaths;
}

void renameToFinal(const std::vector<fs::path> &filePaths, const fs::path &dirPath) {
    int counter = 1;
    for (auto const &file: filePaths) {
        std::string newName = "img_" + std::to_string(counter) + getExtension(file);
        fs::path newPath = dirPath / newName;

        fs::rename(file, newPath);
        counter++;
    }
}

void processDirectory(const fs::path &dirPath) {
    std::vector<fs::path> filePaths;
    for (auto const &entry: fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file() && MEDIA_EXTENSIONS.count(getExtension(entry)) > 0) {
            filePaths.push_back(entry.path());
        }
    }

    std::sort(filePaths.begin(), filePaths.end(), [](const fs::path &a, const fs::path &b) {
        return fs::last_write_time(a) < fs::last_write_time(b);
    });

    const std::vector<fs::path> tempFilePaths = renameToTemp(filePaths, dirPath);
    renameToFinal(tempFilePaths, dirPath);

    for (auto const &entry : fs::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            processDirectory(entry.path());
        }
    }
}

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: file_renamer <directory path>\n";
        return 1;
    }
    const fs::path directoryPath = argv[1]; // Directory path

    processDirectory(directoryPath);

    return 0;
}
