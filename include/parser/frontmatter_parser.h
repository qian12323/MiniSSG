#pragma once
#include "core/document.h"
#include <string>

namespace ssg {

class FrontMatterParser {
public:
  FrontMatterParser();
  FrontMatter parse(const std::string& raw);
};

} // namespace ssg
