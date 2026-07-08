#pragma once

namespace minissg
{
namespace cli
{

struct Command
{
    const char* name;
    const char* desc;
    const char* usage;
    void (*run)(int argc, char* argv[]);
};

} // namespace cli
} // namespace minissg
