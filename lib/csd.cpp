#include <fstream>
#include <future>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <set>

#include <boost/algorithm/string/predicate.hpp>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "csd.hpp"
#include "html5parser.cpp"
#include "uri.cpp"
#include "adler.cpp"

using namespace std;
using namespace CryptoPP;


File::File(const string url)
: url(url)
, data(File::download_url(url))
, adler32hex(adler32hex(data))
, name(url.substr(url.find_last_of('/')+1))
{}

bool File::size_cmp(File x, File y) {
    return x.data.size() < y.data.size();
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

vector<File> OriginUrl::fetch_files() {
    vector<future<File>> futures;

    // TODO: maybe some controll on how many threads is spawned?
    // download will be IO bound so that's ok but the hash calculation
    // should probably be at ncores.
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

        this_thread::sleep_for(chrono::seconds(0));
    }

    return files;
}

set<string> OriginUrl::parse_urls() const {
    return Html5Parser(data, url).urls;
}
