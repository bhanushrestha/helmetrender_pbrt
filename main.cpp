#include <filesystem>
#include <ostream>
#include <string>
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
#include "create_render.hpp"
#include "experiments.hpp"
#include "utility.hpp"

void different_backgrounds(const ParamExperiment& param) {
    auto _param = param;
    for (const auto& dirEntry : std::filesystem::directory_iterator("images/pngs")) {
        auto bg = dirEntry.path();
        if (bg.extension() == ".png") {
            std::string folder_name = "experiments/backgrounds/";
            std::filesystem::create_directories(folder_name);
            _param.background_image = bg.string();
            _param.output_name      = folder_name + bg.filename().string();
            create_render(_param);
            std::cout << "Created: " << _param.output_name << std::endl;
        }
    }
}

void create_rotation(const ParamExperiment& param, const std::string& folder, const int PHOTOS = 30) {
    auto _param             = param;
    std::string folder_name = "experiments/" + folder;
    std::filesystem::create_directories(folder_name);
    std::cout << "Created: " << folder_name << std::endl;
    for (int i = 0; i < PHOTOS; i++) {
        auto angle         = (1.0 / PHOTOS) * 3.14 * 2;
        auto x             = 58 * cos(i * angle);
        auto z             = 58 * sin(i * angle);
        _param.lookAtE[0]  = x;
        _param.lookAtE[1]  = z;
        _param.output_name = folder_name + "/rotation_" + std::to_string(i) + ".png";
        create_render(_param);
        std::cout << "Created: " << _param.output_name << std::endl;
    }
}

void create_kr(const ParamExperiment& param) {
    ParamExperiment _param = param;
    // kr_Experiment
    // for (int i = 0; i <= 10; i++) {
    //    auto kr_val       = i / pbrt::Float(10);
    //    _param.kr[0]       = kr_val;
    //    _param.kr[1]       = kr_val;
    //    _param.kr[2]       = kr_val;
    //    _param.output_name = "experiments/kr/" + std::to_string(i) + ".png";
    //    //{opt, 0, 0, kr, kr, kr, file_name};
    //    create_background(_param);
    //    std::cout << "Created: " << _param.output_name << std::endl;
    //}
    _param                  = param;
    std::string folder_name = "experiments/ks_kr_r";
    std::filesystem::create_directories(folder_name);
    for (int i = 0; i <= 10; i++) {
        auto ks_val  = i / pbrt::Float(10);
        _param.ks[0] = ks_val;
        _param.ks[1] = ks_val;
        _param.ks[2] = ks_val;

        //{opt, 0, 0, kr, kr, kr, file_name};
        for (int j = 0; j <= 8; j++) {
            _param.output_name     = folder_name + "/" + std::to_string(i) + std::to_string(j) + ".png";
            _param.visor_roughness = j / pbrt::Float(100); // 0.01
            create_background(_param);
            std::cout << "Created: " << _param.output_name << std::endl;
        }
    }
    /*
    _param = param;
    for (int i = 0; i <= 10; i++) {
        auto kd_val             = i / pbrt::Float(10);
        _param.kd[0]             = kd_val;
        _param.kd[1]             = kd_val;
        _param.kd[2]             = kd_val;
        std::string folder_name = "experiments/kd_kr";
        std::filesystem::create_directories(folder_name);
        _param.output_name = folder_name + "/" + std::to_string(i) + ".png";
        //{opt, 0, 0, kr, kr, kr, file_name};
        create_background(_param);
        std::cout << "Created: " << _param.output_name << std::endl;
    }
    */
}

void create_glass(const ParamExperiment& param) {
    auto _param = param;
    for (int i = 0; i <= 30; i++) {
        std::string folder_name = "experiments/glass2/";
        std::filesystem::create_directories(folder_name);
        _param.output_name = folder_name + std::to_string(i) + ".png";
        _param.glassetaF   = i / pbrt::Float(10);
        create_background(_param);
        std::cout << "Created: " << _param.output_name << std::endl;
    }
}

void create_visor(const ParamExperiment& param) {
    auto _param = param;
    for (int i = 0; i <= 30; i++) {
        _param.output_name = "experiments/visor/" + std::to_string(i) + ".png";
        _param.visoretaF   = i / pbrt::Float(10);
        create_background(_param);
        std::cout << "Created: " << _param.output_name << std::endl;
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
            i++;
            param.width = stoi(i[0]);
            std::cout << "Setting width: " << param.width << std::endl;
        }
        else if (i[0] == "y" || i[0] == "height") {
            i++;
            param.height = stoi(i[0]);
            std::cout << "Setting height: " << param.height << std::endl;
        }

        else if (i[0] == "fov") {
            i++;
            param.fov = stoi(i[0]);
            std::cout << "Setting fov: " << param.fov << std::endl;
        }
        else if (i[0] == "ray") {
            i++;
            param.rays = stoi(i[0]);
            std::cout << "Setting ray: " << param.rays << std::endl;
        }
        else if (i[0] == "ray_depth") {
            i++;
            param.ray_depth = stoi(i[0]);
            std::cout << "Setting ray depth: " << param.ray_depth << std::endl;
        }
    }
    // Run the code

    const ParamExperiment _param     = param;
    param.background_image           = "images/pngs/R0010096_high.png";
    std::string simple_render_suffix = "_ultra.png";
    for (auto i = args.begin(); i != args.end(); ++i) {
        // Reset configurations
        param = _param;
        if (i[0] == "single") {
            param.lookAtE[0]  = 50;
            param.output_name = "test2.png";

            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }

        else if (i[0] == "helmet") {
            auto bg_images = {"images/pngs/R0010078_high.png", "images/pngs/R0010096_high.png", "images/pngs/R0010102_high.png"};
            for (const auto& bg : bg_images) {
                param.background_image  = bg;
                param.lookAtE[0]        = 58;
                std::string folder_name = "helmet_texture_high/" + std::filesystem::path(bg).filename().string() + "/";
                for (int i = 3; i <= 7; i++) {
                    param.helmet_texture = "images/" + std::to_string(i) + ".png";
                    create_rotation(param, folder_name + std::to_string(i));
                }
            }
        }

        else if (i[0] == "day") {
            param.background_image   = "images/pngs/R0010078_high.png";
            param.lookAtE[0]         = 58;
            param.bg_transform.Deg   = 180;
            param.light_intensity[0] = 0;
            param.light_intensity[1] = 0;
            param.light_intensity[2] = 0;
            std::string folder_name  = "experiments/final/";
            std::filesystem::create_directories(folder_name);
            param.output_name = folder_name + "day" + simple_render_suffix;
            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }

        else if (i[0] == "snow") {
            param.background_image  = "images/pngs/R0010102_high.png";
            param.lookAtE[0]        = 58;
            param.bg_transform.Deg  = 180;
            std::string folder_name = "experiments/final/";
            std::filesystem::create_directories(folder_name);
            param.output_name = folder_name + "snow" + simple_render_suffix;
            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }

        else if (i[0] == "field") {
            param.background_image   = "images/pngs/R0010064_high.png";
            param.lookAtE[0]         = 58;
            param.bg_transform.Deg   = 180;
            param.light_intensity[0] = 0;
            param.light_intensity[1] = 0;
            param.light_intensity[2] = 0;
            std::string folder_name  = "experiments/final/";
            std::filesystem::create_directories(folder_name);
            param.output_name = folder_name + "field" + simple_render_suffix;
            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }

        else if (i[0] == "night") {
            param.background_image  = "images/pngs/R0010096_high.png";
            param.lookAtE[0]        = 58;
            std::string folder_name = "experiments/final/";
            std::filesystem::create_directories(folder_name);
            param.output_name = folder_name + "night" + simple_render_suffix;
            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }
        else if (i[0] == "day360") {
            param.background_image = "images/pngs/R0010064_high.png";
            i++;
            create_rotation(param, i[0]);
            std::cout << "Created: " << param.output_name << std::endl;
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
            param.lookAtE[0] = 58;
            different_backgrounds(param);
        }
        else if (i[0] == "glass") {
            param.background_image = "images/pngs/R0010078_high.png";
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
