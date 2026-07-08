#pragma once

#include <string>
#include <vector>
#include "core/article.h"

namespace minissg
{

std::string loadTemplate(const std::string& path);

std::string renderPost(const Article& article, const std::string& tpl);

std::string renderIndex(const std::vector<Article>& articles, const std::string& tpl);

} // namespace minissg
