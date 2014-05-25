#include "common.h"
#include "miniEngine.h"
#include "shaders.h"

struct cubo
{
    cubo(uint w, uint h)
    {
        auto m = std::make_shared<material>(shaders::sphere);
        int width  = w / 100;
        int height = h / 100;
        for(auto x = - width;  x < width;  x+=3)
            for(auto y = - height; y < height; y+=3)
            {
                auto c = object::createSphere(1.7,64,m);
                c->translate(vec(x,y,0));
                cubes.push_back(c);
            }    
    }
    void rotate(vec::ref angle) {for(auto &c : cubes) c->rotate(angle);}
    void render() {for(auto &c : cubes) c->render();}

private:
    std::vector<std::shared_ptr<object>> cubes;
};

int main()
{
    miniEngine e;

    e.getX11Info();
    e.getOGLInfo();
    
    cubo *cubes;
    
    logger::info("create cubes in %f sec",trace([&](){ cubes = new cubo(e.getWidth(), e.getHeight());}));

    auto background = std::make_shared<object>(object(
         {1,1,0,1,0,1,-1,0,1,1,-1,-1,0,0,1,-1,1,0,0,0},
         {2,1,0,3,2,0},
         std::make_shared<material>(shaders::back)));
    background->translate({0,0,-50});
    background->scale({e.getWidth()/35.0f,e.getHeight()/35.f,0});
   
    float  workTime {};
    long   frames   {};
    
    e.setUpdateHandler([&](float dt)
    {
        workTime +=dt;
        ++frames;
        background->render();
        cubes->render();
        cubes->rotate(vec(dt * 30,dt * 30,dt * 45));
    });

    while(e.update()) {}
    delete cubes;    
    logger::info("Work time: %0.2f sec. Frames: %d, Average FPS: %0.2f", workTime, frames, (float)frames/workTime);

    return 0;
}
