#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <gumbo.h>

using namespace std;

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
    vector<string> relative_urls;

    /**
     * Parses raw urls from the given HTML document
     */
    void parse(GumboNode *node) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }

        parse_a(node);
        // TODO: Add more tag parsers

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            parse(static_cast<GumboNode*>(children->data[i]));
        }
    }

    void parse_a(GumboNode *node) {
        if (node->v.element.tag != GUMBO_TAG_A) {
            return;
        }

        GumboAttribute* href;
        if ((href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
            relative_urls.push_back(href->value);
        }
    }

    /**
     * Resolves the relative links to absolute.
     *
     */
    void munge() {
        for (const auto &x : relative_urls) {
            urls.insert(x);
        }
    }
};

/**
 * Downloaded file
 */
class File {
    public:
    string url;
    string data;

    File(const string url, const string data) : url(url), data(data) {}
    set<string> parse_urls() {
        return Html5Parser(data, url).urls;
    }
};

File download_url(const string url) {
        stringstream out;

        curlpp::Cleanup cleanup;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(url));
        request.setOpt(new curlpp::options::WriteStream(&out));
        request.setOpt(new curlpp::options::FollowLocation(true));

        request.perform();

        return File(url, out.str());
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <url>" << endl;
        return 1;
    }

    try {
        auto origin = download_url(argv[1]);
        auto urls = origin.parse_urls();

        for (const auto &x : urls) {
            cout << x << endl;
        }
    }catch ( const exception& e ) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
