#include <filesystem>
#include <ostream>
#include <thread>

// #include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "background_table_sepc_day.hpp"
#include "background_table_sepc_night_no_snow.hpp"
#include "background_table_spectral.hpp"
#include "bump_experiment.hpp"
#include "constants.hpp"
#include "core/pbrt.h"
#include "experiments.hpp"
#include "utility.hpp"

void different_backgrounds(ParamExperiment& param) {
    for (const auto& dirEntry : std::filesystem::directory_iterator("images/pngs")) {
        auto bg = dirEntry.path();
        if (bg.extension() == ".png") {
            param.background_image = bg.string();
            param.output_name      = "experiments/backgrounds/" + bg.filename().string();
            create_background(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }
    }
}

void create_rotation(ParamExperiment& param, const std::string& folder, const int PHOTOS = 30) {
    for (int i = 0; i < PHOTOS; i++) {
        auto angle              = (1.0 / PHOTOS) * 3.14 * 2;
        auto x                  = 55 * cos(i * angle);
        auto z                  = 55 * sin(i * angle);
        param.lookAtE[0]        = x;
        param.lookAtE[1]        = z;
        std::string folder_name = "experiments/" + folder;
        std::filesystem::create_directories(folder_name);
        param.output_name = folder_name + "/rotation_" + std::to_string(i) + ".png";
        create_day(param);
    }
}

void create_kr(ParamExperiment& param) {
    ParamExperiment tmp = param;
    // kr_Experiment
    // for (int i = 0; i <= 10; i++) {
    //    auto kr_val       = i / pbrt::Float(10);
    //    param.kr[0]       = kr_val;
    //    param.kr[1]       = kr_val;
    //    param.kr[2]       = kr_val;
    //    param.output_name = "experiments/kr/" + std::to_string(i) + ".png";
    //    //{opt, 0, 0, kr, kr, kr, file_name};
    //    create_background(param);
    //    std::cout << "Created: " << param.output_name << std::endl;
    //}
    param                   = tmp;
    std::string folder_name = "experiments/ks_kr_r";
    std::filesystem::create_directories(folder_name);
    for (int i = 0; i <= 10; i++) {
        auto ks_val = i / pbrt::Float(10);
        param.ks[0] = ks_val;
        param.ks[1] = ks_val;
        param.ks[2] = ks_val;

        //{opt, 0, 0, kr, kr, kr, file_name};
        for (int j = 0; j <= 8; j++) {
            param.output_name     = folder_name + "/" + std::to_string(i) + std::to_string(j) + ".png";
            param.visor_roughness = j / pbrt::Float(100); // 0.01
            create_background(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }
    }
    /*
    param = tmp;
    for (int i = 0; i <= 10; i++) {
        auto kd_val             = i / pbrt::Float(10);
        param.kd[0]             = kd_val;
        param.kd[1]             = kd_val;
        param.kd[2]             = kd_val;
        std::string folder_name = "experiments/kd_kr";
        std::filesystem::create_directories(folder_name);
        param.output_name = folder_name + "/" + std::to_string(i) + ".png";
        //{opt, 0, 0, kr, kr, kr, file_name};
        create_background(param);
        std::cout << "Created: " << param.output_name << std::endl;
    }
    */
}

void create_glass(ParamExperiment& param) {
    for (int i = 0; i <= 30; i++) {
        param.output_name = "experiments/glass/" + std::to_string(i) + ".png";
        param.glassetaF   = i / pbrt::Float(10);
        create_background(param);
        std::cout << "Created: " << param.output_name << std::endl;
    }
}

void create_visor(ParamExperiment& param) {
    for (int i = 0; i <= 30; i++) {
        param.output_name = "experiments/visor/" + std::to_string(i) + ".png";
        param.visoretaF   = i / pbrt::Float(10);
        create_background(param);
        std::cout << "Created: " << param.output_name << std::endl;
    }
}

int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }

    pbrt::Options opt;
    opt.nThreads = std::thread::hardware_concurrency();
    opt.quiet    = true;

    // Get the settings:
    ParamExperiment param;
    for (auto i = args.begin(); i != args.end(); i++) {
        if (i[0] == "x" || i[0] == "width") {
            param.width = stoi(i[1]);
            i++;
            std::cout << "Setting width: " << param.width << std::endl;
        }
        else if (i[0] == "y" || i[0] == "height") {
            param.height = stoi(i[1]);
            i++;
            std::cout << "Setting height: " << param.height << std::endl;
        }

        else if (i[0] == "fov") {
            param.height = stoi(i[1]);
            i++;
        }
        else if (i[0] == "ray") {
            param.height = stoi(i[1]);
            i++;
        }
        else if (i[0] == "ray_depth") {
            param.height = stoi(i[1]);
            i++;
        }
    }

    // Run the code

    param.background_image = "images/pngs/R0010096_high.png";
    for (auto i = args.begin(); i != args.end(); ++i) {
        if (i[0] == "single") {
            param.output_name = "test_night_no_snow.png";
            create_no_snow_night(param);
        }

        else if (i[0] == "day") {
            param.background_image = "images/pngs/R0010078_high.png";
            param.output_name      = "test_day.png";
            create_day(param);
        }

        else if (i[0] == "day360") {
            param.background_image = "images/pngs/R0010064_high.png";
            i++;
            create_rotation(param, i[0]);
        }

        else if (i[0] == "field") {
            param.background_image = "images/pngs/R0010064_high.png";
            param.output_name      = "test_field.png";
            create_day(param);
        }

        else if (i[0] == "night") {
            param.background_image = "images/pngs/R0010096_high.png";
            param.output_name      = "test_night_no_snow.png";
            create_no_snow_night(param);
        }

        else if (i[0] == "light_intensity") {
            for (int i = 0; i <= 10; i++) {
                auto val                 = pbrt::Float(i);
                param.light_intensity[0] = val;
                param.light_intensity[1] = val;
                param.light_intensity[2] = val;
                std::string folder_name  = "experiments/light_intensity";
                std::filesystem::create_directories(folder_name);
                param.output_name = folder_name + "/" + std::to_string(i) + ".png";
                create_background(param);
                std::cout << "created " << param.output_name << std::endl;
            }
        }
        else if (i[0] == "background") {
            different_backgrounds(param);
        }
        else if (i[0] == "glass") {
            create_glass(param);
        }

        else if (i[0] == "visor") {
            create_visor(param);
        }
        else if (i[0] == "kr") {
            create_kr(param);
        }
        else if (i[0] == "rotation") {
            i++;
            create_rotation(param, i[0]);
        }
    }

    return 0;
}
