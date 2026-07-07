#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

namespace ssg {

struct FrontMatter {
  std::string title;
  std::string date;
  std::vector<std::string> tags;
};

struct Document {
  FrontMatter frontmatter;
  std::string body;
};

} // namespace ssg
