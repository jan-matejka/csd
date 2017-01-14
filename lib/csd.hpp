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
    const string url;
    const string data;
    const string adler32hex;

    File(const string url);
    bool static size_cmp(File, File);

    private:
    string download_url(const string url) const;
    string mk_hash() const;
};

class OriginUrl : public File {
    public:
    const set<string> urls;

    OriginUrl(const string url);

    vector<File> fetch_files();

    private:
    set<string> parse_urls() const;
};

#endif
