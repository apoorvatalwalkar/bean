#include <stdexcept>
#include "raytracescene.h"
#include "utils/sceneparser.h"

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData) {

    data = metaData;
    globalData = metaData.globalData;
    cameraData = metaData.cameraData;

    w = width;
    h = height;
}

const int& RayTraceScene::width() const {
    return w;
}

const int& RayTraceScene::height() const {
    return h;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return globalData;
}

const std::vector<SceneLightData>& RayTraceScene::getLights() const{
    return data.lights;
}

const std::vector<RenderShapeData>& RayTraceScene::getShapes() const{
    return data.shapes;
}

const Camera& RayTraceScene::getCamera() const {
    Camera camera;
    camera.height = h;
    camera.width = w;
    camera.cameraData = cameraData;

    return camera;
}
