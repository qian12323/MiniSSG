#include "builder/builder.h"

int main()
{
    auto config = minissg::loadConfig("config.yaml");
    minissg::build(config);
    return 0;
}
