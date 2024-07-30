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

bool MCRFT::World::is_block_occupied(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0)
    {
        return false;
    }
    unsigned int chunk_x = x / 16;
    unsigned int coord_x_within_chunk = x % 16;
    unsigned int segment = y / 16;
    unsigned int coord_y_within_chunk = y % 16;
    unsigned int chunk_z = z / 16;
    unsigned int coord_z_within_chunk = z % 16;
    Chunk *curr_chunk = m_chunks[chunk_x][chunk_z];
    if (curr_chunk == nullptr || curr_chunk->m_sections == nullptr)
    {
        return false;
    }
    ChunkSection *curr_section = curr_chunk->m_sections[segment];
    if (curr_section == nullptr || curr_section->blocks == nullptr)
    {
        return false;
    }
    if (coord_x_within_chunk < 0 || coord_x_within_chunk >= 16)
    {
        return false;
    }
    if (coord_y_within_chunk < 0 || coord_y_within_chunk >= 16)
    {
        return false;
    }
    if (coord_z_within_chunk < 0 || coord_z_within_chunk >= 16)
    {
        return false;
    }
    return !(curr_section->blocks[coord_x_within_chunk][coord_y_within_chunk][coord_z_within_chunk] == nullptr);
}
void MCRFT::Chunk::initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_y, const int chunk_size_x, const int chunk_size_y)
{
    try
    {
        // for every section, we want to make a new section (should be 24)
        for (unsigned int i = 0; i < sizeof(m_sections) / sizeof(m_sections[0]); i++)
        {
            ChunkSection *new_section = new ChunkSection();
            m_sections[i] = new_section;
        }
        int block_heights[16][16];
        const unsigned int starting_x_coord = chunk_x * chunk_size_x;
        const unsigned int starting_y_coord = chunk_y * chunk_size_y;
        // for every x, z (16 x 16), we want to get the height at THAT specific x, z
        for (unsigned int i = 0; i < chunk_size_x; i++)
        {
            for (unsigned int j = 0; j < chunk_size_y; j++)
            {
                double noise = perlin->octave2D_01(((starting_x_coord + i) * 0.01), ((starting_y_coord + j) * 0.01), 4);
                noise *= 100;
                const int coordinate_height = static_cast<int>(noise);
                block_heights[i][j] = coordinate_height;
            }
        }
        for (unsigned int i = 0; i < sizeof(m_sections) / sizeof(m_sections[0]); i++)
        {
            unsigned int min_height = (i) * 16; // should probably make this 16 a constant
            ChunkSection *current_section = m_sections[i];
            for (unsigned int u = 0; u < 16; u++) // x
            {
                for (unsigned int v = 0; v < 16; v++) // y
                {
                    for (unsigned int w = 0; w < 16; w++) // z
                    {
                        if (min_height + v <= block_heights[u][w])
                        {
                            current_section->blocks[u][v][w] = new Block();
                        }
                        else
                        {
                            current_section->blocks[u][v][w] = nullptr;
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught an exception: " << e.what() << std::endl;
    }
}