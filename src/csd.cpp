#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <csd.hpp>

using namespace std;

vector<string> argv2args(int argc, char* argv[]) {
    vector<string> vec;

    for(int i=0; i<argc; i++) {
        vec.push_back(string(argv[i]));
    }

    return vec;
}

void usage(ostream &out, string prg) {
    out << "Usage: " << prg << " [--print-urls] <url>" << endl;
}

int main(int argc, char* argv[]) {
    auto args = argv2args(argc, argv);
    auto prg = args.at(0);

    if (argc < 2) {
        usage(cerr, prg);
        return 1;
    }

    bool print_urls = false;
    string url;

    for (int i=1; i<argc; i++) {
        auto s = args.at(i);

        if (s == "--print-urls") {
            print_urls = true;
        } else {
            if (i+1 < argc) {
                cerr << "Unrecognized option: " << s << endl;
                usage(cerr, prg);
                return 1;
            }

            url = s;
        }
    }

    try {
        auto origin = OriginUrl(url);
        auto files = origin.fetch_files();
        for(auto &file : files) {
            cout
                << setw(8) << file.adler32hex << " "
                << setw(60) << file.name;

            if (print_urls) {
                cout << " " << file.url;
            }

            cout << endl;
        }

        auto minf = *min_element(files.begin(), files.end(),
            File::size_cmp);
        auto maxf = *max_element(files.begin(), files.end(),
            File::size_cmp);

        cout << endl;
        cout
            << "Min: " << setw(6) << minf.data.size()
            << " " << minf.name;
        if(print_urls) {
            cout << " " << minf.url;
        }
        cout << endl;

        cout
            << "Max: " << setw(6) << maxf.data.size()
            << " " << maxf.name;
        if(print_urls) {
            cout << " " << maxf.url;
        }

        cout << endl;
    }catch ( const exception& e ) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
