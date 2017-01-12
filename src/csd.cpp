#include <fstream>
#include <sstream>
#include <string>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;

/**
 * Downloaded file
 */
class File {
    public:
    string url;
    string data;

    File(const string url, const string data) : url(url), data(data) {}
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
        cout << origin.data;
    }catch ( const exception& e ) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
