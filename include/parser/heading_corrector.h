#pragma once

#include <string>

namespace minissg
{

std::string correctHeadings(const std::string& html, const std::string& filePath,
                            bool autoNumber = true);

} // namespace minissg
