#pragma once

#include "core/config.h"

namespace minissg
{

SiteConfig loadConfig(const std::string& path);
void build(const SiteConfig& config, bool fixHeadings = true, bool autoNumber = true);

} // namespace minissg
