#include <fstream>
#include <future>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>

#include <boost/algorithm/string/predicate.hpp>

#include <cryptopp/adler32.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <gumbo.h>

#include <uriparser/Uri.h>

#include "csd.hpp"

using namespace std;
using namespace CryptoPP;

class UriParseFailure : public runtime_error {
    public:

    UriParseFailure(int error_code)
        : runtime_error("uri parsing error code: " + to_string(error_code)) {
    }
};

class Uri {
    // FIXME: pick a sane lib here and package it where needed
    // sane looking libs:
    // - https://github.com/datacratic/google-url
    // - https://github.com/chmike/CxxUrl
    // FIXME: this implemenation with uriparser probably doesn't support
    // anything other than most common formats.
    // Unsupported includes: explicit ports, file access, probably
    // localized uris
    public:
    const string str;
    Uri(string url) : str(url) {
        state.uri = &uri;

        if (uriParseUriA(&state, str.c_str()) != URI_SUCCESS) {
            throw new UriParseFailure(state.errorCode);
        }

        scheme = _mkstr(uri.scheme);
        port   = _mkstr(uri.portText);
        host   = _mkstr(uri.hostText);
        is_abs = uri.absolutePath;
        is_prl = boost::starts_with(str, "//");

        uriFreeUriMembersA(&uri);
    }

    string get_absolute(string base) {
        return get_absolute(Uri(base));
    }
    /**
     * \param[in] base uri of the page to resolve relative links against.
     *   eg.: Uri("foo").get_absolute("http://example.com") == "http://example.com/foo"
     */
    string get_absolute(const Uri base) const {
        // TODO: cover by unit tests
        if (!scheme.empty()) {
            return str;
        }

        if (!is_abs) {
            return base.str + "/" + str;
        }

        if (is_prl) {
            // FIXME: scheme-relative-file-URL not implemented.
            // The path-absolute-non-Windows-file-URL doesn't make sense
            // to me.
            return base.scheme + ":" + str;
        }

        if (host.empty()) {
            return base.scheme + "://" + base.host + str;
        }

        throw runtime_error("Failed to absolutize " + str + " with base "
            + base.str);
    }

    private:
    UriParserStateA state;
    UriUriA uri;
    string host;
    string port;
    string scheme;
    /**
     * True if the URL starts with a '/', false otherwise (including
     * starting with scheme)
     */
    bool is_abs;

    /**
     * True if uri is Protocol Relative Link
     * https://url.spec.whatwg.org/#syntax-url-scheme-relative
     */
    bool is_prl;

    string _mkstr(UriTextRangeA x) const {
        if (x.first == NULL || x.afterLast == NULL) {
            return string();
            // TODO: maybe check if only one of those is NULL?
        }

        return string(x.first, x.afterLast);
    }
};

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
            relative_urls.push_back(Uri(href->value));
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

File::File(const string url) : url(url), data(File::download_url(url)),
    adler32hex(File::mk_hash())
{}

string File::mk_hash() const {
    Adler32 hash;
    string hex;
    StringSource ss(data, true,
        new HashFilter(hash,
            new HexEncoder(new StringSink( hex ))
        )
    );

    return hex;
}

string File::download_url(const string url) const {
    stringstream out;

    curlpp::Cleanup cleanup;
    curlpp::Easy request;

    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(new curlpp::options::WriteStream(&out));
    request.setOpt(new curlpp::options::FollowLocation(true));

    request.perform();

    return out.str();
}

OriginUrl::OriginUrl(const string url) : File(url), urls(parse_urls()) {}

vector<File> OriginUrl::fetch_files() {
    vector<future<File>> futures;

    for (const auto &x : urls) {
        auto f(std::async([] (string url) -> File
            { return File(url); }, x));
        futures.push_back(move(f));
    }

    vector<File> files;

    while (futures.size() > 0 ) {
        for(unsigned int i = 0; i < futures.size(); i++) {
            if (futures[i].valid()) {
                files.push_back(futures[i].get());
                futures.erase(futures.begin() + i);
            }
        }
    }

    return files;
}

set<string> OriginUrl::parse_urls() const {
    return Html5Parser(data, url).urls;
}
