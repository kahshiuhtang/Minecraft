#include "world.hpp"
#include "iostream"
void MCRFT::World::init()
{
    try
    {
        const siv::PerlinNoise::seed_type seed = 123456u;

        const siv::PerlinNoise perlin{seed};

        for (unsigned int i = 0; i < 16; i++)
        {
            for (unsigned int j = 0; j < 16; j++)
            {
                Chunk *current_chunk = new Chunk();
                current_chunk->initialize_chunk(&perlin, i, j, CHUNK_SIZE_X, CHUNK_SIZE_Z);
                m_chunks[i][j] = current_chunk;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
}

void MCRFT::Chunk::initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_y, const int chunk_size_x, const int chunk_size_y)
{
    try
    {
        int chunk_heights[16][16];
        const unsigned int starting_x_coord = chunk_x * chunk_size_x;
        const unsigned int starting_y_coord = chunk_y * chunk_size_y;
        for (unsigned int i = 0; i < 16; i++)
        {
            for (unsigned int j = 0; j < 16; j++)
            {
                double noise = perlin->octave2D_01(((starting_x_coord + i) * 0.01), ((starting_y_coord + j) * 0.01), 4);
                noise *= 100;
                const int coordinate_height = static_cast<int>(noise);
                chunk_heights[i][j] = coordinate_height;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
}