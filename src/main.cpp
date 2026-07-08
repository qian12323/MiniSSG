#include <iostream>
#include <fstream>
#include "parser/parser.h"
#include "renderer/renderer.h"

int main()
{
    auto art = minissg::parseArticle("posts/test.md");

    auto tpl = minissg::loadTemplate("themes/default/post.html");
    auto page = minissg::renderPost(art, tpl);

    std::ofstream out("output/test.html");
    out << page;

    std::cout << "Generated: output/test.html" << std::endl;
    return 0;
}
