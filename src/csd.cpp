#include <iomanip>
#include <iostream>

#include <csd.hpp>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <url>" << endl;
        return 1;
    }

    try {
        auto origin = OriginUrl(argv[1]);
        for(auto &file : origin.fetch_files()) {
            cout
                << setw(80) << file.url
                << setw(10) << file.adler32hex
                << endl;
        }
    }catch ( const exception& e ) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
