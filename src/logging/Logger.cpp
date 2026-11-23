#include "logging/Logger.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <utility>

namespace mss {
namespace {

std::mutex &loggerMutex() {
    static std::mutex mutex;
    return mutex;
}

bool reportFailure(const std::string &message) {
    std::cerr << "Erreur du logger : " << message << '\n';
    return false;
}

} // namespace

Logger::Logger(std::filesystem::path filePath) : filePath(std::move(filePath)) {}

bool Logger::info(const std::string &message) const {
    return write("INFO", message);
}

bool Logger::error(const std::string &message) const {
    return write("ERROR", message);
}

bool Logger::write(const char *level, const std::string &message) const {
    std::lock_guard<std::mutex> lock(loggerMutex());

    const std::filesystem::path directory = filePath.parent_path();
    if (!directory.empty()) {
        std::error_code error;
        std::filesystem::create_directories(directory, error);
        if (error) {
            return reportFailure("impossible de creer " + directory.string() + ": " +
                                 error.message());
        }
    }

    std::ofstream file(filePath, std::ios::out | std::ios::app);
    if (!file) {
        return reportFailure("impossible d'ouvrir " + filePath.string());
    }

    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    if (localtime_s(&localTime, &time) != 0) {
        return reportFailure("impossible de convertir l'heure courante");
    }
#else
    if (localtime_r(&time, &localTime) == nullptr) {
        return reportFailure("impossible de convertir l'heure courante");
    }
#endif

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() %
        1000;
    file << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S") << '.'
         << std::setw(3) << std::setfill('0') << milliseconds << " [" << level << "] "
         << message << '\n';
    file.flush();
    if (!file) {
        return reportFailure("echec d'ecriture dans " + filePath.string());
    }
    file.close();
    if (file.fail()) {
        return reportFailure("echec de fermeture de " + filePath.string());
    }
    return true;
}

} // namespace mss
