#include <algorithm>
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
        auto files = origin.fetch_files();
        for(auto &file : files) {
            cout
                << setw(80) << file.url
                << setw(10) << file.adler32hex
                << endl;
        }

        auto minf = *min_element(files.begin(), files.end(),
            File::size_cmp);
        auto maxf = *max_element(files.begin(), files.end(),
            File::size_cmp);

        cout
            << "Min: " << setw(6) << minf.data.size()
                << " " << minf.url << endl
            << "Max: " << setw(6) << maxf.data.size()
                << " " << maxf.url << endl;
    }catch ( const exception& e ) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
