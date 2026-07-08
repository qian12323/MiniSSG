#pragma once

#include <string>

namespace minissg
{
namespace cli
{

int run(int argc, char* argv[]);

void cmdBuild(const std::string& configPath);
void cmdNew(const std::string& title, const std::string& configPath);
void cmdServe(int port, const std::string& configPath);

} // namespace cli
} // namespace minissg
