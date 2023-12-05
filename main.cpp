#include <filesystem>
#include <ostream>
#include <string>
#include <thread>

// #include <vector>

#include "core/api.h"
#include "core/paramset.h"

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
        auto x             = param.lookAtE[0] * cos(i * angle);
        auto z             = param.lookAtE[0] * sin(i * angle);
        _param.lookAtE[0]  = x;
        _param.lookAtE[1]  = z;
        _param.output_name = folder_name + "/rotation_" + std::to_string(i) + ".png";
        try {
            create_render(_param);
        }
        catch (...) {
        }
        std::cout << "Created: " << _param.output_name << std::endl;
    }
}

void create_kr(const ParamExperiment& param) {
    ParamExperiment _param = param;
    // kr_Experiment
    for (int i = 0; i <= 10; i++) {
        auto kr_val  = i / pbrt::Float(10);
        _param.kr[0] = kr_val;
        _param.kr[1] = kr_val;
        _param.kr[2] = kr_val;
        for (int j = 0; j <= 10; j++) {
            auto ks_val        = j / pbrt::Float(10);
            _param.ks[0]       = ks_val;
            _param.ks[1]       = ks_val;
            _param.ks[2]       = ks_val;
            _param.output_name = "experiments/kr_ks/kr_" + std::to_string(i) + "_ks_" + std::to_string(j) + ".png";
            //{opt, 0, 0, kr, kr, kr, file_name};
            create_render(_param);
            std::cout << "Created: " << _param.output_name << std::endl;
        }
    }
}

std::string get_output_name(const std::filesystem::path& path) {
    const auto stem = path.stem();
    const auto dir  = path.parent_path();
    // if (dir.parent_path())
    int highest = 0;
    for (const auto& file : std::filesystem::directory_iterator(dir)) {
        auto f = file.path().filename().string();
        if (f.rfind(stem.string(), 0) == 0) {
            auto nr_ext = f.substr(stem.string().length());
            auto nr     = nr_ext.substr(0, nr_ext.length() - path.extension().string().length());
            int nr_int  = stoi(nr);
            if (nr_int > highest) {
                highest = nr_int;
            }
        }
    }
    highest++;
    return dir.string() + "/" + stem.string() + std::to_string(highest) + path.extension().string();
}

ParamExperiment parse_settings(const std::vector<std::string>& args) {
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
        else if (i[0] == "bg") {
            i++;
            param.background_image = i[0];
            std::cout << "Setting background: " << param.background_image << std::endl;
        }
        else if (i[0] == "camera") {
            i++;
            param.lookAtE[0] = stoi(i[0]);
            i++;
            param.lookAtE[1] = stoi(i[0]);
            i++;
            param.lookAtE[2] = stoi(i[0]);
            std::cout << "Setting starting camera position: " << param.lookAtE[0] << ", " << param.lookAtE[1] << ", "
                      << param.lookAtE[2] << std::endl;
        }
    }
    return param;
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
    ParamExperiment param = parse_settings(args);
    // Run the code

    param.background_image           = "images/background1.png";
    std::string simple_render_suffix = "_ultra.png";
    const ParamExperiment _param     = param;
    for (auto i = args.begin(); i != args.end(); ++i) {
        // Reset configurations
        param = _param;
        if (i[0] == "single") {
            auto path       = std::filesystem::path("./test.png");
            auto final_name = get_output_name(path);
            std::cout << "The Final Name: " << final_name << std::endl;
            param.output_name = final_name;

            create_render(param);
            std::cout << "Created: " << param.output_name << std::endl;
        }

        else if (i[0] == "far") {
            auto path       = std::filesystem::path("./experiments/material/test_far.png");
            auto final_name = get_output_name(path);
            std::cout << "The Final Name: " << final_name << std::endl;
            param.output_name = final_name;
            // create_render(param);
            param.lookAtE[0] = 30.f;
            param.lookAtE[1] = 10.f;
            param.lookAtE[2] = 10.f;

            path       = std::filesystem::path("./experiments/material/test_close.png");
            final_name = get_output_name(path);
            std::cout << "The Final Name: " << final_name << std::endl;
            param.output_name = final_name;
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
                    // param.helmet_texture = "images/" + std::to_string(i) + ".png";
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

        else if (i[0] == "background") {
            param.lookAtE[0] = 58;
            different_backgrounds(param);
        }
        else if (i[0] == "kr") {
            create_kr(param);
        }
        else if (i[0] == "rotation") {
            i++;
            create_rotation(param, i[0], 60);
        }
    }

    return 0;
}
