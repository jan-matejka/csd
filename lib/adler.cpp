#ifndef __CSD_ADLER
#define __CSD_ADLER

#include <string>

#include <cryptopp/adler32.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

using namespace std;
using namespace CryptoPP;

string adler32hex(const string xs) {
    Adler32 hash;
    string  hex;
    StringSource ss(
      data
    , true
    , new HashFilter(hash, new HexEncoder(new StringSink(hex)))
    );

    return hex;
}

#endif
