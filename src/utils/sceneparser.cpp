#include "sceneparser.h"
#include "scenefilereader.h"
#include "imagereader.h";
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    appendLists(glm::mat4(1), root, renderData);

    std::cout << renderData.lights.size();
    std::cout << renderData.shapes.size();

    return true;
}

void SceneParser::appendLists(glm::mat4 ctmToThisPoint, SceneNode* current, RenderData &renderData){
    std::vector<SceneTransformation*> trfms = current->transformations;
    glm::mat4 newMatrix = ctmToThisPoint;
    for(int i = 0; i <  trfms.size(); i++){
        newMatrix *= getTransformation(trfms[i]);
    }

    std::vector<ScenePrimitive*> pmtvs = current->primitives;
    for(int i =0; i < pmtvs.size(); i++){
        RenderShapeData shapeData;
        shapeData.primitive = *pmtvs[i];
        shapeData.ctm = newMatrix;
        shapeData.inverse = glm::inverse(newMatrix);

        if (!pmtvs[i]->material.textureMap.filename.empty()){
            shapeData.image = loadImageFromFile(pmtvs[i]->material.textureMap.filename);
        }
        if (pmtvs[i]->type == PrimitiveType::PRIMITIVE_MESH) {
            // shapeData.primitive.meshfile = pmtvs[i]->meshfile;
            shapeData.primitive.meshfile = "bean_meshfile.obj";
        }
        renderData.shapes.push_back(shapeData);
    }

    // std::vector<ScenePrimitive*> pmtvs = current->primitives;
    // for(int i =0; i < pmtvs.size(); i++){
    //     if(pmtvs[i]->material.textureMap.filename.empty()){
    //         renderData.shapes.push_back(RenderShapeData{*pmtvs[i], newMatrix, glm::inverse(newMatrix), std::nullopt});
    //     }
    //     else{
    //         renderData.shapes.push_back(RenderShapeData{*pmtvs[i], newMatrix, glm::inverse(newMatrix), loadImageFromFile(pmtvs[i]->material.textureMap.filename)});
    //     }
    // }

    std::vector<SceneLight*>lghts = current->lights;
    for(int i = 0; i < lghts.size(); i++){
        addLight(newMatrix, lghts[i], renderData);
    }

    std::vector<SceneNode*> chldn = current->children;
    for(int i = 0; i < chldn.size(); i++){
        appendLists(newMatrix, chldn[i], renderData);
    }

}

void SceneParser::addLight(glm::mat4 ctmToThisPoint, SceneLight* sl, RenderData &renderData){
    if(sl->type == LightType::LIGHT_POINT){
        glm::vec4 posMult = {0, 0, 0, 1};
        renderData.lights.push_back(SceneLightData{
                                                   .id = sl->id,
                                                   .type = sl->type,
                                                   .color = sl->color,
                                                   .function = sl->function,
                                                   .pos = ctmToThisPoint * posMult,

                                                   .dir = {0, 0, 0, 0},
                                                   .penumbra = 0,
                                                   .angle = 0,
                                                   });
    }
    else if (sl->type == LightType::LIGHT_SPOT){
        glm::vec4 posMult = {0, 0, 0, 1};
        renderData.lights.push_back(SceneLightData{
            .id = sl->id,
            .type = sl->type,
            .color = sl->color,
            .function = sl->function,
            .pos = ctmToThisPoint * posMult,
            .dir = ctmToThisPoint * sl->dir,
            .penumbra = sl->penumbra,
            .angle = sl->angle
        });
    }
    else {
        renderData.lights.push_back(SceneLightData{
            .id = sl->id,
            .type = sl->type,
            .color = sl->color,
            .function = sl->function,
            .pos = {0, 0, 0, 0},
            .dir = ctmToThisPoint * sl->dir,
            .penumbra = 0,
            .angle = 0

        });
    }

}

glm::mat4 SceneParser::getTransformation(SceneTransformation* current){
    if(current->type == TransformationType::TRANSFORMATION_TRANSLATE){
        glm::vec3 tslt = current->translate;
        return glm::translate(glm::mat4(1), tslt);
    }
    else if (current->type == TransformationType::TRANSFORMATION_SCALE){
        glm::vec3 scl = current->scale;
        return glm::scale(scl);
    }
    else if (current->type == TransformationType::TRANSFORMATION_ROTATE) {
        glm::vec3 rtt = current->rotate;
        return glm::rotate(current->angle, rtt);
    }
    else if (current->type == TransformationType::TRANSFORMATION_MATRIX){
        glm::mat4 mtx = current->matrix;
        return mtx;
    }
    else {
        return glm::mat4(1);
    }

}
