#pragma once
#include "core/document.h"
#include <string>

namespace ssg {

class MarkdownParser {
public:
  MarkdownParser();
  std::string render(const std::string& markdown);
};

} // namespace ssg
