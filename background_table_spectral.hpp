#pragma once

#include <iterator>
#include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "constants.hpp"
#include "utility.hpp"

struct Param {
    pbrt::Options opt;
    pbrt::Float glassetaF = 0;
    pbrt::Float visoretaF = 0;
    pbrt::Float kd[3]     = {0.f, 0.f, 0.f};
    pbrt::Float kr[3]     = {0, 0, 0};
    pbrt::Float ks[3]     = {0.f, 0.f, 0.f};
    std::string output_name;
};

void create_background(const Param& param) {
    pbrt::pbrtInit(param.opt);
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
    filmParams.AddString("filename", makeSingle(std::string(param.output_name)), 1);

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

    pbrt::ParamSet mirror2Mat;
    mirror2Mat.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{0.7f, 0.7f, 0.7f}), 3);
    mirror2Mat.AddFloat("eta", makeSingle(param.visoretaF)); // THIS one
    mirror2Mat.AddString("type", makeSingle("glass"s), 1);
    pbrt::pbrtMakeNamedMaterial("Mirror2", mirror2Mat);

    // Uber material
    pbrt::ParamSet purpleMat;
    purpleMat.AddString("type", makeSingle("uber"s), 1);
    mirror2Mat.AddFloat("eta", makeSingle(pbrt::Float(1.5)));
    purpleMat.AddRGBSpectrum("Kd",
                             makeMulti(std::vector<pbrt::Float>(param.kd, param.kd + sizeof(param.kd) / sizeof(param.kd[0]))),
                             3);
    purpleMat.AddRGBSpectrum("Ks",
                             makeMulti(std::vector<pbrt::Float>(param.ks, param.ks + sizeof(param.ks) / sizeof(param.ks[0]))),
                             3);
    purpleMat.AddRGBSpectrum("Kr",
                             makeMulti(std::vector<pbrt::Float>(param.kr, param.kr + sizeof(param.kr) / sizeof(param.kr[0]))),
                             3);
    purpleMat.AddFloat("roughness", makeSingle(pbrt::Float(0.9)));
    pbrt::pbrtMakeNamedMaterial("mattePurple", purpleMat);

    // Substrate Texture and Material
    pbrt::ParamSet tmap;
    tmap.AddString("filename", makeSingle("textures/bump.png"s), 1);
    pbrt::pbrtTexture("tmap", "color", "imagemap", tmap);

    pbrt::ParamSet substrateMat;
    substrateMat.AddString("type", makeSingle("substrate"s), 1);
    substrateMat.AddTexture("Kd", "tmap");
    substrateMat.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{0.04f, 0.04f, 0.04f}), 3);
    substrateMat.AddBool("remaproughness", makeSingle(false), 1);
    pbrt::pbrtMakeNamedMaterial("substrateMat", substrateMat);

    pbrt::pbrtAttributeBegin();
    {
        Transformation transformations;
        transformations.Rotate    = pbrt::Vector3f{0, 0, 1};
        transformations.Deg       = 180;
        transformations.Translate = pbrt::Vector3f{0, 0, -50};
        doTransformation(transformations);

        pbrt::ParamSet lightSrc;
        // TODO: The image?
        lightSrc.AddString("mapname", makeSingle("images/R0010072.png"s), 1);
        pbrt::pbrtLightSource("infinite", lightSrc);
    }
    pbrt::pbrtAttributeEnd();

    // Add mirror visor
    Transformation mirrorVisorTrans{{60, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mirrorVisorTrans, "Mirror2", "geometry/visorOnlyV2.pbrt");

    // Helmet Object 2
    Transformation mattePurpleVisorTrans{{60, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mattePurpleVisorTrans, "substrateMat", "geometry/bikeHelmetOnlyV2.pbrt");

    // Disk
    pbrt::pbrtAttributeBegin();
    {
        Transformation Trans{{42, 0, -15}};
        doTransformation(Trans);
        pbrt::ParamSet diskMatPara;
        diskMatPara.AddFloat("eta", makeSingle(param.glassetaF)); // This
        pbrt::pbrtMaterial("glass", diskMatPara);

        pbrt::ParamSet diskShapePara;
        diskShapePara.AddFloat("radius", makeSingle(pbrt::Float(42.5f)));
        diskShapePara.AddFloat("innerradius", makeSingle(pbrt::Float(2.9f)));
        diskShapePara.AddFloat("height", makeSingle(pbrt::Float(0.6f)));
        diskShapePara.AddFloat("phimax", makeSingle(pbrt::Float(360.f)));
        pbrt::pbrtShape("disk", diskShapePara);

        pbrt::ParamSet cylShapePara;
        cylShapePara.AddFloat("radius", makeSingle(pbrt::Float(42.5f)));
        cylShapePara.AddFloat("zmin", makeSingle(pbrt::Float(-0.1f)));
        cylShapePara.AddFloat("phimax", makeSingle(pbrt::Float(360.f)));
        pbrt::pbrtShape("cylinder", cylShapePara);
    }
    pbrt::pbrtAttributeEnd();

    // Cylinder
    pbrt::pbrtAttributeBegin();
    {
        Transformation Trans{{42, 0, -52}};
        doTransformation(Trans);

        pbrt::ParamSet cylMatPara;
        cylMatPara.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{0.06f, 0.06f, 0.06f}), 3);
        pbrt::pbrtMaterial("matte", cylMatPara);

        pbrt::ParamSet cylShapePara;
        cylShapePara.AddFloat("radius", makeSingle(pbrt::Float(9.5f)));
        cylShapePara.AddFloat("zmin", makeSingle(pbrt::Float(-1.0f)));
        cylShapePara.AddFloat("zmax", makeSingle(pbrt::Float(42.0f)));
        cylShapePara.AddFloat("phimax", makeSingle(pbrt::Float(360.f)));
        pbrt::pbrtShape("cylinder", cylShapePara);
    }
    pbrt::pbrtAttributeEnd();
    pbrt::pbrtWorldEnd();
    pbrt::pbrtCleanup();
}