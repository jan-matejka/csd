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
     * Downloads and hashes the file from `url`
     */
    File(const string url);

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
     * File name
     */
    const string name;

    /**
     * \returns true if data of x < data of y
     */
    bool static size_cmp(File x, File y);

    /**
     * \returns Files found in this file/url. fetches the files in
     *    parallel, blocks until are files are received.
     */
    vector<File> fetch_files();

    private:
    string download_url(const string url) const;
    string mk_hash() const;
    string mk_fname() const;
    set<string> parse_urls() const;
    const set<string> urls;
};

#endif
