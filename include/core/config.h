#pragma once
#include <string>
#include <filesystem>

namespace ssg {

struct Config {
  std::filesystem::path content_dir = "content";
  std::filesystem::path output_dir = "output";
  std::filesystem::path theme_dir = "themes/default";
};

} // namespace ssg
