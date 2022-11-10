//#include <algorithm>
//#include <fstream>
//#include <iostream>
//#include <memory>
//#include <sstream>
//#include <string>
//#include <string_view>
#include <thread>
//#include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "constants.hpp"
#include "experiments.hpp"
#include "utility.hpp"

int main(int argc, char** argv) {
    std::cout << "Hello world!" << std::endl;
    pbrt::Options opt;
    opt.nThreads = std::thread::hardware_concurrency();
    create_experiment(opt, "experiment.png");
    return 0;
}
