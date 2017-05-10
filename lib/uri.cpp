#ifndef __CSD_URI
#define __CSD_URI

#include <uriparser/Uri.h>

using namespace std;

class UriParseFailure : public runtime_error {
    public:

    UriParseFailure(int error_code)
        : runtime_error("uri parsing error code: " + to_string(error_code)) {
    }
};


/**
 * A convenience wrapper over uriparser C library
 */
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

    void print_dbg() {
        cout
            << scheme << " " << is_prl << " " << is_abs << " " << host.empty()
            << endl
            << str << endl << endl;
    }

    /**
     * \param[in] base uri of the page to resolve relative links against.
     *   eg.: Uri("foo").get_absolute("http://example.com") == "http://example.com/foo"
     */
    string get_absolute(const Uri base) const {
        // FIXME: drop fragments
        // TODO: cover by unit tests
        if (!scheme.empty()) {
            return str;
        }

        if (is_prl) {
            // FIXME: scheme-relative-file-URL not implemented.
            // The path-absolute-non-Windows-file-URL doesn't make sense
            // to me.
            return base.scheme + ":" + str;
        }

        if (!is_abs) {
            return base.str + "/" + str;
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

#endif
