#ifndef __CSD_HPP
#define __CSD_HPP

#include <set>
#include <string>
#include <vector>

using namespace std;

/**
 * Downloaded file
 */
class File {
    public:
    /**
     * Source url
     */
    const string url;
    /**
     * Data received from url
     */
    const string data;
    /**
     * Adler 32 hash of `data` in hex format
     */
    const string adler32hex;

    /**
     * Downloads and hashes the file from `url`
     */
    File(const string url);

    /**
     * \returns true if data of x < data of y
     */
    bool static size_cmp(File x, File y);

    private:
    string download_url(const string url) const;
    string mk_hash() const;
};

class OriginUrl : public File {
    public:
    const set<string> urls;

    OriginUrl(const string url);

    /**
     * \returns Files found in this file/url. fetches the files in
     *    parallel, blocks until are files are received.
     */
    vector<File> fetch_files();

    private:
    set<string> parse_urls() const;
};

#endif
