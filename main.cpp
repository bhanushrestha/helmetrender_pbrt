#include <ostream>
#include <thread>
// #include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "background_table_spectral.hpp"
#include "constants.hpp"
#include "core/pbrt.h"
#include "experiments.hpp"
#include "utility.hpp"

int main(int argc, char** argv) {
    std::cout << "Hello world!" << std::endl;
    pbrt::Options opt;
    opt.nThreads = std::thread::hardware_concurrency();
    opt.quiet    = true;
    // create_experiment(opt, "experiment.png");
    // Visor_Experiment
    for (int i = 0; i <= 30; i++) {
        Param Param{};
        Param.opt         = opt;
        Param.output_name = "experiments/visor/" + std::to_string(i) + ".png";
        Param.visoretaF   = i / pbrt::Float(10);
        create_background(Param);
        std::cout << "Created: " << Param.output_name << std::endl;
    }

    // glass_Experiment
    for (int i = 0; i <= 30; i++) {
        Param Param{};
        Param.opt         = opt;
        Param.output_name = "experiments/glass/" + std::to_string(i) + ".png";
        Param.glassetaF   = i / pbrt::Float(10);
        create_background(Param);
        std::cout << "Created: " << Param.output_name << std::endl;
    }

    /**
    // kr_Experiment
    for (int i = 0; i <= 10; i++) {
        auto kr_val = i / pbrt::Float(10);
        Param Param{};
        Param.opt         = opt;
        Param.kr[0]       = kr_val;
        Param.kr[1]       = kr_val;
        Param.kr[2]       = kr_val;
        Param.output_name = "experiment/kr/" + std::to_string(i) + ".png";
        //{opt, 0, 0, kr, kr, kr, file_name};
        create_background(Param);
        std::cout << "Created: " << Param.output_name << std::endl;
    }
    for (int i = 0; i <= 10; i++) {
        auto ks_val = i / pbrt::Float(10);
        Param Param{};
        Param.opt         = opt;
        Param.ks[0]       = ks_val;
        Param.ks[1]       = ks_val;
        Param.ks[2]       = ks_val;
        Param.output_name = "experiment/ks/" + std::to_string(i) + ".png";
        //{opt, 0, 0, kr, kr, kr, file_name};
        create_background(Param);
        std::cout << "Created: " << Param.output_name << std::endl;
    }
    for (int i = 0; i <= 10; i++) {
        auto kd_val = i / pbrt::Float(10);
        Param Param{};
        Param.opt         = opt;
        Param.kd[0]       = kd_val;
        Param.kd[1]       = kd_val;
        Param.kd[2]       = kd_val;
        Param.output_name = "experiment/kd/" + std::to_string(i) + ".png";
        //{opt, 0, 0, kr, kr, kr, file_name};
        create_background(Param);
        std::cout << "Created: " << Param.output_name << std::endl;
    }
    */
    return 0;
}
