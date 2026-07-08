#pragma once

#include <string>

namespace minissg
{

struct SiteConfig
{
    std::string title;
    std::string description;
    std::string baseUrl;
    std::string sourceDir;
    std::string outputDir;
    std::string themeDir;

    bool fixHeadings = true;
    bool autoNumber = true;
};

} // namespace minissg
