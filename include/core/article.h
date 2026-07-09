#pragma once

#include <string>
#include <vector>

namespace minissg
{

struct Article
{
    std::string title;
    std::string date;
    std::string slug;
    std::string category;
    std::vector<std::string> tags;

    std::string rawContent;
    std::string htmlContent;

    std::string excerpt;
    std::string coverImage;
};

} // namespace minissg
