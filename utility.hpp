#pragma once

#include "core/api.h"
#include "core/paramset.h"
#include <filesystem>
#include <ostream>

#include "constants.hpp"

using namespace std::string_literals;

template<class T>
std::unique_ptr<T[]> makeSingle(const T& val) {
    auto ptr = std::make_unique<T[]>(1);
    ptr[0]   = val;
    return std::move(ptr);
}

template<class T>
std::unique_ptr<T[]> makeMulti(std::vector<T> vals) {
    auto ptr = std::make_unique<T[]>(vals.size());
    int i    = 0;
    for (const auto& v : vals) {
        ptr[i++] = v;
    }
    return std::move(ptr);
}

struct Transformation {
    pbrt::Vector3f Translate{0, 0, 0};
    pbrt::Vector3f Scale{1, 1, 1};

    pbrt::Float Deg = 0.0;
    pbrt::Vector3f Rotate{0, 0, 1};

    std::string to_string() const {
        std::stringstream ss;
        ss << "Translate " << Translate.x << " " << Translate.y << " " << Translate.z << std::endl;
        ss << "Scale " << Scale.x << " " << Scale.y << " " << Scale.z << std::endl;
        ss << "Rotate " << Deg << " " << Rotate.x << " " << Rotate.y << " " << Rotate.z << std::endl;
        return ss.str();
    }

    void print() const { std::cout << to_string(); }
};

struct ParamExperiment {
    pbrt::Options opt;
    pbrt::Float fov        = pbrt::Float(60);
    int width              = XRES;
    int height             = YRES;
    int rays               = RAYS;
    int ray_depth          = RAY_DEPTH;
    pbrt::Float glassetaF  = 0;
    pbrt::Float visoretaF  = 0;
    pbrt::Float kd[3]      = {0.f, 0.f, 0.f};
    pbrt::Float kr[3]      = {1.0f, 1.0f, 1.0f};
    pbrt::Float ks[3]      = {0.6f, 0.6f, 0.6f};
    pbrt::Float lookAtE[3] = {60.f, 0.f, 0.f};
    // pbrt::Float lookAtE[3]         = {30.f, 10.f, 10.f};
    pbrt::Float light_intensity[3] = {0.1f, 0.1f, 0.1f};
    // std::string helmet_texture     = "images/2.png";
    std::string background_image;
    Transformation bg_transform{};
    std::string output_name;
};

void add_material(const std::string& name, const std::string& filename) {
    // Autumn Leaves Materials
    pbrt::ParamSet TextureParam;
    TextureParam.AddString("filename", makeSingle(filename), 1);
    TextureParam.AddFloat("uscale", makeSingle(pbrt::Float(1)));
    TextureParam.AddFloat("vscale", makeSingle(pbrt::Float(1)));
    pbrt::pbrtTexture(name, "spectrum", "imagemap", TextureParam);
    pbrt::ParamSet MaterialParam;
    MaterialParam.AddString("type", makeSingle("matte"s), 1);
    MaterialParam.AddTexture("Kd", name);
    pbrt::pbrtMakeNamedMaterial(name, MaterialParam);
}

void doTransformation(const Transformation& transformation) {
    pbrt::pbrtTranslate(transformation.Translate.x, transformation.Translate.y, transformation.Translate.z);
    pbrt::pbrtScale(transformation.Scale.x, transformation.Scale.y, transformation.Scale.z);
    pbrt::pbrtRotate(transformation.Deg, transformation.Rotate.x, transformation.Rotate.y, transformation.Rotate.z);
}

bool hasEnding(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

void add_attribute(const Transformation& transformation,
                   const std::string& material,
                   const std::string& include = "",
                   const bool named_mat       = false) {
    pbrt::pbrtAttributeBegin();

    doTransformation(transformation);
    if (hasEnding(material, ".pbrt")) {
        pbrt::pbrtParseFile(material);
    }
    else if (named_mat) {
        pbrt::pbrtNamedMaterial(material);
    }
    else {
        pbrt::ParamSet materialParam;
        materialParam.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{1.f, 1.f, 1.f}), 3);
        pbrt::pbrtMaterial(material, materialParam);
    }

    // Hack around a stupid around
    auto path     = std::filesystem::current_path();
    auto inc_path = std::filesystem::path(include);
    std::filesystem::current_path(inc_path.parent_path());

    pbrt::pbrtParseFile(inc_path.filename().string());

    std::filesystem::current_path(path);
    pbrt::pbrtAttributeEnd();
}

void add_attribute2(const Transformation& transformation, const std::string& include = "") {
    pbrt::pbrtAttributeBegin();

    doTransformation(transformation);

    // Hack around a stupid around
    auto path     = std::filesystem::current_path();
    auto inc_path = std::filesystem::path(include);
    std::filesystem::current_path(inc_path.parent_path());

    pbrt::pbrtParseFile(inc_path.filename().string());

    std::filesystem::current_path(path);
    pbrt::pbrtAttributeEnd();
}

void create_object(const std::string& name,
                   std::vector<std::string> materials,
                   std::vector<Transformation> transformations,
                   std::vector<std::string> includes) {
    pbrt::pbrtObjectBegin(name);
    if (materials.size() == transformations.size() && materials.size() == includes.size()) {
        for (unsigned int i = 0; i < materials.size(); i++) {
            add_attribute(transformations[i], materials[i], includes[i]);
        }
    }
    else {
        std::cout << "ERROR: " << materials.size() << "!= " << transformations.size() << "!= " << includes.size() << std::endl;
    }

    pbrt::pbrtObjectEnd();
}

void add_object(const std::string& name, const Transformation& transformation) {
    pbrt::pbrtAttributeBegin();
    pbrt::pbrtTranslate(transformation.Translate.x, transformation.Translate.y, transformation.Translate.z);
    pbrt::pbrtScale(transformation.Scale.x, transformation.Scale.y, transformation.Scale.z);
    pbrt::pbrtRotate(transformation.Deg, transformation.Rotate.x, transformation.Rotate.y, transformation.Rotate.z);
    pbrt::pbrtObjectInstance(name);
    pbrt::pbrtAttributeEnd();
}

auto matrixToAxisAngle(const float m[][4]) {
    struct ret {
        float deg{};
        pbrt::Vector3f vec{};
    };
    // To quaternion
    float qw, qx, qy, qz;
    qw = sqrt(std::max(0.f, 1 + m[0][0] + m[1][1] + m[2][2])) / 2;
    qx = sqrt(std::max(0.f, 1 + m[0][0] - m[1][1] - m[2][2])) / 2;
    qy = sqrt(std::max(0.f, 1 - m[0][0] + m[1][1] - m[2][2])) / 2;
    qz = sqrt(std::max(0.f, 1 - m[0][0] - m[1][1] + m[2][2])) / 2;

    // To angle axis
    float deg = (2 * acos(qw) * (180.f / pbrt::Pi));
    float x   = qx / sqrt(1 - qw * qw);
    float y   = qy / sqrt(1 - qw * qw);
    float z   = qz / sqrt(1 - qw * qw);

    return ret{deg, {x, y, z}};
}