#include <iostream>
#include <string>
#include <cmark.h>

namespace minissg
{
    
    void test(){
        std::string md = "# Test Title\n- item1\n- item2";
        auto doc = cmark_parse_document(md.c_str(), md.size(), CMARK_OPT_DEFAULT);
        char* html_buf = cmark_render_html(doc, CMARK_OPT_UNSAFE);

        std::cout << html_buf << std::endl;

        free(html_buf);
        cmark_node_free(doc);
    }
    
} // namespace minissg


