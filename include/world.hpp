#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "external/PerlinNoise.hpp"
namespace MCRFT
{
    class World;
    enum Direction
    {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
        FRONT,
        BACK
    };
    struct Block
    {
        uint8_t id;
        uint8_t metadata;
    };
    struct ChunkSection
    {
        Block *blocks[16][16][16];
        uint8_t blockLight[16][16][16]; // Block light levels
        uint8_t skyLight[16][16][16];
    };
    class Chunk
    {
    public:
        ChunkSection *m_sections[24]; // 24 sections to cover 384 blocks in height
        int m_max_height[16][16];
        int m_chunk_x;
        int m_chunk_z;
        std::vector<float> m_mesh_vertices;
        void initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_y, const int chunk_size_x, const int chunk_size_y);
        void generate_mesh(World *world);

    private:
    };
    class World
    {
    public:
        void init();
        bool is_block_occupied(int x, int y, int z);
        bool generate_all_chunk_meshes();
        Chunk *get_chunk(int x, int z);

    private:
        const int CHUNK_SIZE_X = 16;
        const int CHUNK_SIZE_Y = 384;
        const int CHUNK_SIZE_Z = 16;
        Chunk *m_chunks[16][16];
    };
};

#endif