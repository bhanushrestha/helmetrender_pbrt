#pragma once

#include <filesystem>
#include <iterator>
#include <vector>

#include "core/api.h"
#include "core/paramset.h"

#include "constants.hpp"
#include "utility.hpp"

void create_bump(const ParamExperiment& param) {
    pbrt::pbrtInit(param.opt);
    std::cout << std::filesystem::current_path() << "\n";

    pbrt::pbrtLookAt(/*E*/ param.lookAtE[0], param.lookAtE[1], param.lookAtE[2], /*L*/ 0, 0, 0, /*U*/ 0, 0, 1);
    // ***** Camera *****
    pbrt::ParamSet cameraParams;
    cameraParams.AddFloat("fov", makeSingle(pbrt::Float(param.fov)), 1);
    // cameraParams.AddFloat("lensradius", makeSingle(pbrt::Float(0.5f)), 1);
    // cameraParams.AddFloat("focaldistance", makeSingle(pbrt::Float(40.f)), 1);

    pbrt::pbrtCamera("perspective", cameraParams);

    // ***** Film *****
    pbrt::ParamSet filmParams;
    // xresolution
    filmParams.AddInt("xresolution", makeSingle(param.width), 1);
    // yresolution
    filmParams.AddInt("yresolution", makeSingle(param.height), 1);
    // filename
    filmParams.AddString("filename", makeSingle(std::string(param.output_name)), 1);

    pbrt::pbrtFilm("image", filmParams);

    // ***** Sampler *****
    pbrt::ParamSet samplerParams;
    samplerParams.AddInt("pixelsamples", makeSingle(param.rays), 1);
    pbrt::pbrtSampler("halton", samplerParams);

    // ***** Integrator *****
    pbrt::ParamSet integratorParams;
    integratorParams.AddInt("maxdepth", makeSingle(param.ray_depth), 1);

    pbrt::pbrtIntegrator("path", integratorParams);
    // ***** World Begins ******
    pbrt::pbrtWorldBegin();

    pbrt::ParamSet mirrorMat;
    mirrorMat.AddRGBSpectrum("Kr", makeMulti(std::vector<pbrt::Float>{0.73565960f, 0.73565960f, 0.73565960f}), 3);
    mirrorMat.AddString("type", makeSingle("mirror"s), 1);
    pbrt::pbrtMakeNamedMaterial("Mirror1", mirrorMat);

    pbrt::ParamSet mirror2Mat;
    mirror2Mat.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{0.7f, 0.7f, 0.7f}), 3);
    mirror2Mat.AddFloat("eta", makeSingle(param.visoretaF));
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

    pbrt::ParamSet tmap;
    tmap.AddString("filename", makeSingle("textures/bump.png"s), 1);
    pbrt::pbrtTexture("tmap", "color", "imagemap", tmap);

    pbrt::ParamSet substrateMat;
    substrateMat.AddString("type", makeSingle("substrate"s), 1);
    substrateMat.AddTexture("Kd", "tmap");
    substrateMat.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{0.04f, 0.04f, 0.04f}), 3);
    substrateMat.AddBool("remaproughness", makeSingle(false), 1);
    pbrt::pbrtMakeNamedMaterial("substrateMat", substrateMat);

    pbrt::ParamSet bumpy_kd_tex;
    bumpy_kd_tex.AddString("filename", makeSingle("./textures/bump.png"s), 1);
    pbrt::pbrtTexture("bumpy_inlay-kd-tex", "color", "imagemap", bumpy_kd_tex);

    pbrt::ParamSet bumpy_kd;
    bumpy_kd.AddTexture("tex1", "bumpy_inlay-kd-tex");
    bumpy_kd.AddRGBSpectrum("tex2", makeMulti(std::vector<pbrt::Float>{1.5f, 1.5f, 1.5f}), 3);
    pbrt::pbrtTexture("bumpy_inlay-kd", "color", "scale", bumpy_kd);

    pbrt::ParamSet bumpy_bump;
    bumpy_bump.AddString("filename", makeSingle("./textures/paper_bump.png"s), 1);
    pbrt::pbrtTexture("bumpy_inlay-bump", "float", "imagemap", bumpy_bump);

    pbrt::ParamSet bumpy_sc;
    bumpy_sc.AddTexture("tex1", "bumpy_inlay-bump");
    bumpy_sc.AddFloat("tex2", makeSingle(pbrt::Float(0.005)));
    bumpy_sc.AddRGBSpectrum("tex2", makeMulti(std::vector<pbrt::Float>{1.5f, 1.5f, 1.5f}), 3);
    pbrt::pbrtTexture("bumpy_inlay-bump-sc", "float", "scale", bumpy_sc);

    pbrt::ParamSet bumpy_param;
    bumpy_param.AddFloat("roughness", makeSingle(pbrt::Float(0.0104080001)));
    bumpy_param.AddString("type", makeSingle("substrate"s), 1);
    bumpy_param.AddTexture("Kd", "bumpy_inlay-kd");
    bumpy_param.AddTexture("bumpmap", "bumpy_inlay-bump-sc");

    pbrt::pbrtMakeNamedMaterial("bumpy_inlay", bumpy_param);
    pbrt::pbrtAttributeBegin();
    {
        Transformation transformations;
        transformations.Rotate    = pbrt::Vector3f{0, 0, 1};
        transformations.Deg       = 180;
        transformations.Translate = pbrt::Vector3f{0, 0, -50};
        doTransformation(transformations);

        pbrt::ParamSet lightSrc;
        // TODO: The image?
        lightSrc.AddString("mapname", makeSingle(param.background_image), 1);
        pbrt::pbrtLightSource("infinite", lightSrc);
    }
    pbrt::pbrtAttributeEnd();
    // Add mirror visor
    Transformation mirrorVisorTrans{{10, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    // add_attribute(mirrorVisorTrans, "mattePurple", "./geometry/visorOnlyV2.pbrt");

    // Helmet Object 2
    Transformation mattePurpleVisorTrans{{10, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    // add_attribute(mattePurpleVisorTrans, "substrateMat", "./geometry/bikeHelmetOnlyV2.pbrt");

    create_object("helmet",
                  std::vector<std::string>{"mattePurple", "substrateMat"},
                  std::vector<Transformation>{mirrorVisorTrans, mattePurpleVisorTrans},
                  std::vector<std::string>{"./geometry/visorOnlyV2.pbrt", "./geometry/bikeHelmetOnlyV2.pbrt"});
    // Disk
    pbrt::pbrtAttributeBegin();
    {
        Transformation Trans{{0, 0, -15}};
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
        Transformation Trans{{0, 0, -52}};
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