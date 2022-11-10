#pragma once

#include "core/api.h"
#include "core/paramset.h"

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

void add_material(const std::string name, const std::string& filename) {
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

struct Transformation {
    pbrt::Vector3f Translate{0, 0, 0};
    pbrt::Vector3f Scale{1, 1, 1};

    pbrt::Float Deg = 0.0;
    pbrt::Vector3f Rotate{1, 0, 0};

    std::string to_string() const {
        std::stringstream ss;
        ss << "Translate " << Translate.x << " " << Translate.y << " " << Translate.z << std::endl;
        ss << "Scale " << Scale.x << " " << Scale.y << " " << Scale.z << std::endl;
        ss << "Rotate " << Deg << " " << Rotate.x << " " << Rotate.y << " " << Rotate.z << std::endl;
        return ss.str();
    }

    void print() const { std::cout << to_string(); }
};

void doTransformation(const Transformation& transformation) {
    pbrt::pbrtTranslate(transformation.Translate.x, transformation.Translate.y, transformation.Translate.z);
    pbrt::pbrtScale(transformation.Scale.x, transformation.Scale.y, transformation.Scale.z);
    pbrt::pbrtRotate(transformation.Deg, transformation.Rotate.x, transformation.Rotate.y, transformation.Rotate.z);
}

void add_attribute(const Transformation& transformation, const std::string& material, const std::string& include = "") {
    pbrt::pbrtAttributeBegin();

    doTransformation(transformation);
    if (material == ".pbrt") {
        pbrt::pbrtParseFile(material);
    }
    else {
        pbrt::pbrtNamedMaterial(material);
    }

    pbrt::pbrtParseFile(include);

    pbrt::pbrtAttributeEnd();
}

void create_object(const std::string& name,
                   std::vector<std::string> materials,
                   std::vector<Transformation> transformations,
                   std::vector<std::string> includes) {
    pbrt::pbrtObjectBegin(name);
    if (materials.size() != transformations.size() && materials.size() != includes.size()) {
        return;
    }
    for (int i = 0; i < materials.size(); i++) {
        add_attribute(transformations[i], materials[i], includes[i]);
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