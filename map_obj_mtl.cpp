#include <chrono>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    std::string source = std::stod(argv[1]);
    std::string target = std::stdo(argv[2]);
    std::string material = "";

    static struct option long_options[] = {
        {"material", required_argument, 0, 'm'}, {0, 0, 0, 0}};
    optind = 3;
    int option_index = 0;
    for (int i = 0; i < 1000; ++i) {
        int opt = getopt_long(argc, argv, "m:", long_options, &option_index);
        if (opt == -1) {
            break;
        }
        switch (opt) {
        case 'm': {
            material = optarg;
        }
        default: {
            std::cerr << "?? getopt returned character code " << opt << "??"
                      << std::endl;
        }
        }
    }
}
