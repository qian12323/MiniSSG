#include <iostream>
#include "parser/parser.h"

int main()
{
    auto art = minissg::parseArticle("posts/test.md");

    std::cout << "=== Article Info ===" << std::endl;
    std::cout << "title: " << art.title << std::endl;
    std::cout << "date:  " << art.date  << std::endl;
    std::cout << "slug:  " << art.slug  << std::endl;
    std::cout << "tags:  ";
    for (auto& t : art.tags)
        std::cout << t << " ";
    std::cout << std::endl;
    std::cout << "=== HTML ===" << std::endl;
    std::cout << art.htmlContent << std::endl;

    return 0;
}
