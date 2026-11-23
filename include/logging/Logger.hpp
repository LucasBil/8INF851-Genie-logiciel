#pragma once

#include <filesystem>
#include <string>

namespace mss {

class Logger final {
  public:
    explicit Logger(std::filesystem::path filePath =
                        std::filesystem::path("logs") / "mss.log");

    bool info(const std::string &message) const;
    bool error(const std::string &message) const;

  private:
    bool write(const char *level, const std::string &message) const;

    std::filesystem::path filePath;
};

} // namespace mss
