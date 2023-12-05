#pragma once

#include <iterator>
#include <vector>

#include "core/api.h"
#include "core/geometry.h"
#include "core/paramset.h"

#include "core/pbrt.h"
#include "utility.hpp"

void create_render(const ParamExperiment& param) {
    pbrt::pbrtInit(param.opt);
    pbrt::pbrtLookAt(/*E*/ param.lookAtE[0], param.lookAtE[1], param.lookAtE[2], /*L*/ 0, 0, 0, /*U*/ 0, 0, 1);

    // ***** Camera *****
    pbrt::ParamSet cameraParams;
    cameraParams.AddFloat("fov", makeSingle(pbrt::Float(param.fov)), 1);
    // Focus
    // cameraParams.AddFloat("lensradius", makeSingle(pbrt::Float(0.2f)), 1);
    // cameraParams.AddFloat("focaldistance", makeSingle(pbrt::Float(0.f)), 1);

    pbrt::pbrtCamera("perspective", cameraParams);

    // ***** Film *****
    pbrt::ParamSet filmParams;
    // xresolution
    filmParams.AddInt("xresolution", makeSingle(param.width), 1);
    // yresolution
    filmParams.AddInt("yresolution", makeSingle(param.height), 1);
    // filename
    filmParams.AddString("filename", makeSingle(param.output_name), 1);

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

    pbrt::pbrtAttributeBegin();
    {
        pbrt::ParamSet lightSrc;
        lightSrc.AddRGBSpectrum(
            "L",
            makeMulti(std::vector<pbrt::Float>{param.light_intensity[0], param.light_intensity[1], param.light_intensity[2]}),
            3);
        lightSrc.AddPoint3f("from", makeSingle(pbrt::Point3f(200.f, 0.0, 170.0)), 1);
        lightSrc.AddPoint3f("to", makeSingle(pbrt::Point3f(0.f, .0, 1.0)), 1);
        pbrt::pbrtLightSource("distant", lightSrc);
    }
    pbrt::pbrtAttributeEnd();
    /*





    */
    {
        // The helmet material
        pbrt::ParamSet helmetMaterial;

        pbrt::ParamSet coreMap;
        coreMap.AddString("filename", makeSingle("textures/viking/1_Base_Color_resize.png"s), 1);
        pbrt::pbrtTexture("1-coremap", "color", "imagemap", coreMap);
        helmetMaterial.AddTexture("color", "1-coremap");

        // Specular Reflection
        // float kr = 0.8f;
        // helmetMaterial.AddRGBSpectrum("Kr", makeMulti(std::vector<pbrt::Float>{kr, kr, kr}), 3);

        // Glossyness
        // float ks = 0.8f;
        // helmetMaterial.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{ks, ks, ks}), 3);

        helmetMaterial.AddString("type", makeSingle("disney"s), 1);

        // helmetMaterial.AddFloat("roughness", makeSingle(pbrt::Float(0.6f)));
        helmetMaterial.AddFloat("eta", makeSingle(pbrt::Float(2.9304f)));

        // pbrt::ParamSet bumpMap;
        // bumpMap.AddString("filename", makeSingle("textures/viking/9425-bump.png"s), 1);
        // pbrt::pbrtTexture("1-bmap", "float", "imagemap", bumpMap);
        // helmetMaterial.AddTexture("bumpmap", "1-bmap");
        //
        //  pbrt::ParamSet roughMap;
        //  roughMap.AddString("filename", makeSingle("textures/viking/1_Roughness_resize.png"s), 1);
        //  pbrt::pbrtTexture("1-rough", "float", "imagemap", roughMap);
        //  helmetMaterial.AddTexture("roughness", "1-rough");

        helmetMaterial.AddFloat("metallic", makeSingle(pbrt::Float(1.f)));
        // helmetMaterial.AddFloat("spectrans", makeSingle(pbrt::Float(0.8f)));
        // helmetMaterial.AddFloat("speculartint", makeSingle(pbrt::Float(0.1f)));
        helmetMaterial.AddFloat("roughness", makeSingle(pbrt::Float(0.1f)));

        // float kd = 0.5f;
        // float ks = 0.2f;
        // float kr = 0.0331047662f;
        // helmetMaterial.AddRGBSpectrum("Kd", makeMulti(std::vector<pbrt::Float>{kd, kd, kd}), 3);
        // helmetMaterial.AddRGBSpectrum("Ks", makeMulti(std::vector<pbrt::Float>{ks, ks, ks}), 3);
        // helmetMaterial.AddRGBSpectrum("Kr", makeMulti(std::vector<pbrt::Float>{kr, kr, kr}), 3);

        // helmetMaterial.AddFloat("index", makeSingle(pbrt::Float(1.45f)), 1);
        pbrt::pbrtMakeNamedMaterial("helmetMaterial", helmetMaterial);
    }
    {
        // The chain material
        pbrt::ParamSet chainMaterial;

        pbrt::ParamSet coreMap2;
        coreMap2.AddString("filename", makeSingle("textures/viking/2_Base_Color_resize.png"s), 1);
        pbrt::pbrtTexture("2-coremap", "color", "imagemap", coreMap2);
        chainMaterial.AddTexture("color", "2-coremap");

        // pbrt::ParamSet roughMap2;
        // roughMap2.AddString("filename", makeSingle("textures/viking/2_Roughness_resize.png"s),
        // 1); pbrt::pbrtTexture("2-rough", "float", "imagemap", roughMap2);
        // chainMaterial.AddTexture("roughness", "2-rough");

        chainMaterial.AddString("type", makeSingle("disney"s), 1);
        chainMaterial.AddFloat("metallic", makeSingle(pbrt::Float(1.f)));
        chainMaterial.AddFloat("roughness", makeSingle(pbrt::Float(0.1f)));
        chainMaterial.AddFloat("eta", makeSingle(pbrt::Float(2.9304f)), 1);

        pbrt::pbrtMakeNamedMaterial("chainMaterial", chainMaterial);
    }

    {
        // The mandy helmet material
        pbrt::ParamSet mandyMaterial;

        pbrt::ParamSet coreMap;
        coreMap.AddString("filename", makeSingle("textures/mandy/lambert1_albedo_resize.png"s), 1);
        pbrt::pbrtTexture("lambert1-kd", "color", "imagemap", coreMap);
        mandyMaterial.AddTexture("color", "lambert1-kd");

        mandyMaterial.AddString("type", makeSingle("disney"s), 1);

        mandyMaterial.AddFloat("metallic", makeSingle(pbrt::Float(1.f)));
        mandyMaterial.AddFloat("roughness", makeSingle(pbrt::Float(0.0f)));
        mandyMaterial.AddFloat("speculartint", makeSingle(pbrt::Float(0.2f)));
        mandyMaterial.AddFloat("eta", makeSingle(pbrt::Float(2.9304f)), 1);

        pbrt::pbrtMakeNamedMaterial("lambert1", mandyMaterial);
    }

    pbrt::pbrtAttributeBegin();
    {
        // doTransformation(param.bg_transform);
        pbrt::ParamSet lightSrc;
        std::cout << "background image " << param.background_image << "!!!!!" << std::endl;
        lightSrc.AddString("mapname", makeSingle(param.background_image), 1);
        pbrt::pbrtLightSource("infinite", lightSrc);
    }
    pbrt::pbrtAttributeEnd();
    Transformation HelemtTransformation{{10, 0, -5}, {15, 15, 15}, 120, {0.5773503f, 0.5773503f, 0.5773503f}};
    add_attribute2(HelemtTransformation, "geometry/viking_helmet.pbrt");

    // Uncomment to use the Mandelorian Helmet
    // Transformation MandyTransformation{{10, 0, 3}, {110, 110, 110}, 120, {0.5773503f, 0.5773503f, 0.5773503f}};
    // add_attribute2(MandyTransformation, "geometry/mandy_helmet.pbrt");

    // Disk

    pbrt::pbrtAttributeBegin();
    {
        Transformation Trans{{0, 0, -15}};
        doTransformation(Trans);
        pbrt::ParamSet diskMatPara;
        diskMatPara.AddFloat("eta", makeSingle(param.glassetaF)); // This

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
        Transformation Trans{{0, 0, -57}};
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