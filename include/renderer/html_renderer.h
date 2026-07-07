#pragma once
#include "core/document.h"
#include <string>

namespace ssg {

class HtmlRenderer {
public:
  HtmlRenderer();
  std::string render(const Document& doc);
};

} // namespace ssg
