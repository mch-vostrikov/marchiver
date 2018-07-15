#include <getopt.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    static const char* optstr = "i:o:b:c:h";
    static const option opts[] = {
        {"in", 1, nullptr, 'i'},
        {"out", 1, nullptr, 'o'},
        {"block-size", 1, nullptr, 'b'},
        {"chunk-size", 1, nullptr, 'c'},
        {"help", 0, nullptr, 'h'},
    };

    std::string infile, outfile;
    // Default block size is 1 byte, chunk size is 1Kb.
    unsigned long block_size = 1, chunk_size = 1024;
    int opt;
    while ((opt = getopt_long(argc, argv, optstr, opts, nullptr)) != -1) {
        switch (opt) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'b':
                // Returns 0 on fail.
                block_size = strtoul(optarg, nullptr, 10);
                break;
            case 'c':
                // Returns 0 on fail.
                chunk_size = strtoul(optarg, nullptr, 10);
                break;
            case 'h':
                std::cout << "Usage:\n" << argv[0] << "[--in infile] [--out outfile] [--block-size sz] [--chunk-size sz]\n"
                    << "    -i, --in          input filename, stdin if none\n"
                    << "    -o, --out         output filename, stdout if none\n"
                    << "    -b, --block-size  block size in bytes, default is 1, maximum is 8\n"
                    << "    -c, --chunk-size  chunk size in bytes, default is 1024, must be power of two\n"
                    << "    -h, --help        show this message\n";
                return 0;
        }
    }

    if (block_size < 1 || block_size > 8) {
        std::cout << "block-size must be between 1 and 8\n";
        return 1;
    }
    if (__builtin_popcountl(chunk_size) != 1 || chunk_size < 2) {
        std::cout << "chunk-size must be power of two\n";
        return 1;
    }

    return 0;
}
