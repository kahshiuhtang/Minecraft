#include "renderer/renderer.hpp"
#include "world/world.hpp"
int main()
{

    MCRFT::Renderer *renderer = new MCRFT::Renderer();
    renderer->init();
    renderer->loop();
    renderer->destroy_renderer();
}