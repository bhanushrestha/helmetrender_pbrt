#pragma once

#include <iterator>
#include <vector>

#include "core/api.h"
#include "core/geometry.h"
#include "core/paramset.h"

#include "core/pbrt.h"
#include "utility.hpp"

void create_no_snow_night(const ParamExperiment& param) {
    pbrt::pbrtInit(param.opt);
    pbrt::pbrtLookAt(/*E*/ param.lookAtE[0], param.lookAtE[1], param.lookAtE[2], /*L*/ 0, 0, 0, /*U*/ 0, 0, 1);

    // ***** Camera *****
    pbrt::ParamSet cameraParams;
    cameraParams.AddFloat("fov", makeSingle(pbrt::Float(param.fov)), 1);
    cameraParams.AddFloat("lensradius", makeSingle(pbrt::Float(0.5f)), 1);
    cameraParams.AddFloat("focaldistance", makeSingle(pbrt::Float(40.f)), 1);

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

    // Main Light

    // pbrt::pbrtAttributeBegin();
    //{
    //     pbrt::ParamSet arealightParam;
    //     arealightParam.AddRGBSpectrum("L", makeMulti(std::vector<pbrt::Float>{45.f, 0, 0}), 3);
    //     pbrt::pbrtAreaLightSource("area", arealightParam);
    //
    //    pbrt::pbrtCoordSysTransform("camera");
    //    doTransformation(Transformation{{50, 50, 0}});
    //    pbrt::ParamSet radius;
    //    radius.AddFloat("radius", makeSingle(pbrt::Float(26)));
    //    pbrt::pbrtShape("disk", radius);
    //}
    // pbrt::pbrtAttributeEnd();
    /*

 #LightSource "distant" "color L" [50 50 50] "point from" [100 10 50] "point to" [0 0 1]
 #LightSource "distant" "color I" [30 30 30] "point from" [50 10 10]

 {
    #LightSource "point" "color I" [30 30 30] "point from" [50 10 10]
    pbrt::ParamSet lightSrc;
    lightSrc.AddRGBSpectrum("I", makeMulti(std::vector<pbrt::Float>{30.0f, 30.0f, 30.0f}), 3);
    lightSrc.AddPoint3f("from", makeSingle(pbrt::Point3f(50.f, 10.0, 10.0)), 1);
    pbrt::pbrtLightSource("distant", lightSrc);

 }
*/

    pbrt::pbrtAttributeBegin();
    {
        pbrt::ParamSet lightSrc;
        // TODO: The image?
        // lightSrc.AddString("", makeSingle(param.background_image), 1);
        lightSrc.AddRGBSpectrum(
            "L",
            makeMulti(std::vector<pbrt::Float>{param.light_intensity[0], param.light_intensity[1], param.light_intensity[2]}),
            3);
        lightSrc.AddPoint3f("from", makeSingle(pbrt::Point3f(200.f, 0.0, 170.0)), 1);
        lightSrc.AddPoint3f("to", makeSingle(pbrt::Point3f(0.f, .0, 1.0)), 1);
        pbrt::pbrtLightSource("distant", lightSrc);
    }
    pbrt::pbrtAttributeEnd();
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
    purpleMat.AddFloat("roughness", makeSingle(param.visor_roughness));
    pbrt::pbrtMakeNamedMaterial("mattePurple", purpleMat);

    // Substrate Texture and Material
    pbrt::ParamSet tmap;
    tmap.AddString("filename", makeSingle("textures/bump.png"s), 1);
    pbrt::pbrtTexture("tmap", "color", "imagemap", tmap);

    pbrt::ParamSet substrateMat;
    substrateMat.AddString("type", makeSingle("substrate"s), 1);
    substrateMat.AddTexture("Kd", "tmap");
    // This actually worked before
    //  substrateMat.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{0.04f, 0.04f, 0.04f}), 3);
    //  substrateMat.AddBool("remaproughness", makeSingle(false), 1);
    substrateMat.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{0.1f, 0.1f, 0.1f}), 3);
    // substrateMat.AddFloat("uroughness", makeSingle(pbrt::Float(0.07f))); // roughness from 0 to 1, 0.05
    // substrateMat.AddFloat("vroughness", makeSingle(pbrt::Float(0.07f))); // roughness from 0 to 1, 0.05
    substrateMat.AddBool("remaproughness", makeSingle(false), 1);
    pbrt::pbrtMakeNamedMaterial("substrateMat", substrateMat);

    pbrt::ParamSet substrateMat2;
    substrateMat2.AddString("type", makeSingle("substrate"s), 1);
    // substrateMat2.AddTexture("Kd", "tmap");
    substrateMat2.AddFloat("uroughness", makeSingle(pbrt::Float(0.0f))); // roughness from 0 to 1, 0.05
    substrateMat2.AddFloat("vroughness", makeSingle(pbrt::Float(0.0f))); // roughness from 0 to 1, 0.05
    substrateMat2.AddBool("remaproughness", makeSingle(false), 1);       // try with bot false and true for ^
    substrateMat2.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{0.7f, 0.7f, 0.7f}), 3); // 0 to 1, 0.05
    pbrt::pbrtMakeNamedMaterial("substrateMat2", substrateMat2);

    pbrt::pbrtAttributeBegin();
    {
        Transformation transformations;
        transformations.Rotate = pbrt::Vector3f{0, 0, 1};
        // transformations.Deg       = 180;
        transformations.Translate = pbrt::Vector3f{0, 0, -50};
        doTransformation(transformations);

        pbrt::ParamSet lightSrc;
        // TODO: The image?
        lightSrc.AddString("mapname", makeSingle(param.background_image), 1);
        pbrt::pbrtLightSource("infinite", lightSrc);
    }
    pbrt::pbrtAttributeEnd();
    // Add mirror visor
    Transformation mirrorVisorTrans{{60, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mirrorVisorTrans, "mattePurple", "geometry/visorOnlyV2.pbrt");
    // add_attribute(mirrorVisorTrans, "substrateMat2", "geometry/visorOnlyV2.pbrt");
    //  Helmet Object 2
    Transformation mattePurpleVisorTrans{{60, 0, -18}, {1, 1, 1}, 180, {0, 0, 1}};
    add_attribute(mattePurpleVisorTrans, "substrateMat", "geometry/bikeHelmetOnlyV2.pbrt");

    // Disk
    pbrt::pbrtAttributeBegin();
    {
        Transformation Trans{{42, 0, -15}};
        doTransformation(Trans);
        pbrt::ParamSet diskMatPara;
        diskMatPara.AddFloat("eta", makeSingle(param.glassetaF)); // This
        // Adding the glossiness to the glass
        diskMatPara.AddFloat("Kr", makeSingle(pbrt::Float(10.0f)));
        diskMatPara.AddFloat("Kt", makeSingle(pbrt::Float(10.0f)));

        // Selecting material type for the disk
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