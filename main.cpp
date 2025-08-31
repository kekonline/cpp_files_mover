#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main() {
    fs::path musicFolder = "music";  // relative to container working dir

    if (!fs::exists(musicFolder) || !fs::is_directory(musicFolder)) {
        std::cerr << "❌ 'music' folder not found.\n";
        return 1;
    }

    // Iterate over artist folders
    for (const auto& artistDir : fs::directory_iterator(musicFolder)) {
        if (!fs::is_directory(artistDir)) continue;

        std::cout << "Processing artist: " << artistDir.path().filename().string() << "\n";

        std::vector<fs::path> filesToMove;
        std::vector<fs::path> dirsToRemove;

        // 1️⃣ Collect files and dirs separately
        for (auto& entry : fs::recursive_directory_iterator(artistDir.path())) {
            if (fs::is_regular_file(entry)) {
                filesToMove.push_back(entry.path());
            } else if (fs::is_directory(entry)) {
                dirsToRemove.push_back(entry.path());
            }
        }

        // 2️⃣ Move files to artist root
        for (const auto& file : filesToMove) {
            fs::path targetPath = artistDir.path() / file.filename();
            try {
                if (fs::exists(targetPath)) {
                    fs::path newTarget = artistDir.path() / ("copy_" + file.filename().string());
                    std::cout << "⚠️ File exists, renaming: " << newTarget << "\n";
                    fs::rename(file, newTarget);
                } else {
                    fs::rename(file, targetPath);
                }
            } catch (fs::filesystem_error& e) {
                std::cerr << "Error moving file: " << e.what() << "\n";
            }
        }

        // 3️⃣ Remove empty subfolders (deepest first)
        std::sort(dirsToRemove.begin(), dirsToRemove.end(),
                  [](const fs::path& a, const fs::path& b) {
                      return a.string().size() > b.string().size(); // deeper first
                  });

        for (const auto& dir : dirsToRemove) {
            if (fs::exists(dir) && fs::is_empty(dir)) {
                try {
                    fs::remove(dir);
                    std::cout << "🗑️ Removed empty folder: " << dir << "\n";
                } catch (fs::filesystem_error& e) {
                    std::cerr << "Error removing folder: " << e.what() << "\n";
                }
            }
        }
    }

    std::cout << "✅ Done flattening music folders!\n";
    return 0;
}
