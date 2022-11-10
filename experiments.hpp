#pragma once

#include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "constants.hpp"
#include "utility.hpp"

void create_experiment(const pbrt::Options& opt, const std::string& output_name) {
    pbrt::pbrtInit(opt);
    pbrt::pbrtLookAt(/*E*/ 100, 0, 0, /*L*/ 0, 0, 0, /*U*/ 0, 0, 1);

    // ***** Camera *****
    pbrt::ParamSet cameraParams;
    cameraParams.AddFloat("fov", makeSingle(pbrt::Float(FOV)), 1);
    cameraParams.AddFloat("lensradius", makeSingle(pbrt::Float(0.5f)), 1);
    cameraParams.AddFloat("focaldistance", makeSingle(pbrt::Float(40.f)), 1);

    pbrt::pbrtCamera("perspective", cameraParams);

    // ***** Film *****
    pbrt::ParamSet filmParams;
    // xresolution
    filmParams.AddInt("xresolution", makeSingle(XRES), 1);
    // yresolution
    filmParams.AddInt("yresolution", makeSingle(YRES), 1);
    // filename
    filmParams.AddString("filename", makeSingle(std::string(output_name)), 1);

    pbrt::pbrtFilm("image", filmParams);

    // ***** Sampler *****
    pbrt::ParamSet samplerParams;
    samplerParams.AddInt("pixelsamples", makeSingle(RAYS), 1);
    pbrt::pbrtSampler("halton", samplerParams);

    // ***** Integrator *****
    pbrt::ParamSet integratorParams;
    integratorParams.AddInt("maxdepth", makeSingle(RAY_DEPTH), 1);

    pbrt::pbrtIntegrator("path", integratorParams);

    // ***** World Begins ******
    pbrt::pbrtWorldBegin();

    pbrt::ParamSet mirrorMat;
    mirrorMat.AddRGBSpectrum("Kr", makeMulti(std::vector<pbrt::Float>{0.73565960f, 0.73565960f, 0.73565960f}), 3);
    mirrorMat.AddString("type", makeSingle("mirror"s), 1);
    pbrt::pbrtMakeNamedMaterial("Mirror1", mirrorMat);

    pbrt::ParamSet mattePurpleMat;
    mattePurpleMat.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{0.7f, 0.5f, 0.7f}), 3);
    mattePurpleMat.AddString("type", makeSingle("matte"s), 1);
    pbrt::pbrtMakeNamedMaterial("mattePurple", mattePurpleMat);

    pbrt::pbrtAttributeBegin();
    {
        Transformation transformations;
        transformations.Rotate    = pbrt::Vector3f{0, 0, 1};
        transformations.Deg       = 180;
        transformations.Translate = pbrt::Vector3f{0, 0, -50};
        doTransformation(transformations);

        pbrt::ParamSet lightSrc;
        lightSrc.AddString("mapname", makeSingle("images/background1.exr"s), 1);
        pbrt::pbrtLightSource("infinite", lightSrc);
    }
    pbrt::pbrtAttributeEnd();

    // Add mirror visor
    Transformation mirrorVisorTrans{{60, 0, -15}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mirrorVisorTrans, "Mirror1", "geometry/visorOnlyV2.pbrt");

    Transformation mattePurpleVisorTrans{{60, 0, -15}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mattePurpleVisorTrans, "mattePurple", "geometry/bikeHelmetOnlyV2.pbrt");

    pbrt::pbrtWorldEnd();
    pbrt::pbrtCleanup();
}