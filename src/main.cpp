#include "renderer.hpp"
#include "world.hpp"
int main()
{

    MCRFT::Renderer *renderer = new MCRFT::Renderer();
    renderer->init();
    renderer->loop();
    renderer->destroy_renderer();
}