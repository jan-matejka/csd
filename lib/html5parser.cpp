#ifndef __CSD_HTML5PARSER
#define __CSD_HTML5PARSER

using namespace std;

#include <gumbo.h>

/**
 * Parses file references from HTML document.
 * TODO: Probably can handle only HTML5 now.
 */
class Html5Parser {
    public:

    /** URLs parsed from the given document
     */
    set<string> urls;

    /**
     * \param[in] page HTML document
     * \param[in] url  URL of the document. Used for resolving relative
     *    links.
     */
    Html5Parser (const string page, string url)
    : base_url(url) {
        output = gumbo_parse(page.c_str());
        parse(output->root);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        munge();
    }

    private:

    GumboOutput* output;
    string base_url;
    vector<Uri> relative_urls;

    /**
     * Parses raw urls from the given HTML document
     */
    void parse(GumboNode *node) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }

        parse_attr(node, GUMBO_TAG_A, "href");
        parse_attr(node, GUMBO_TAG_IMG, "src");
        parse_attr(node, GUMBO_TAG_LINK, "href");
        parse_attr(node, GUMBO_TAG_SCRIPT, "src");
        // TODO: probably missed some, check with html5 spec
        // FIXME: handle base tag

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            parse(static_cast<GumboNode*>(children->data[i]));
        }
    }

    void parse_attr(GumboNode *node, const GumboTag t, const char* attr) {
        if (node->v.element.tag != t) {
            return;
        }

        GumboAttribute* gattr;
        if ((gattr = gumbo_get_attribute(&node->v.element.attributes, attr))) {
            relative_urls.push_back(Uri(gattr->value));
        }
    }

    /**
     * Resolves the relative links to absolute.
     *
     */
    void munge() {
        for (const auto &x : relative_urls) {
            urls.insert(x.get_absolute(Uri(base_url)));
        }
    }
};

#endif
