#ifndef WORLD_HPP
#define WORLD_HPP

#include "external/PerlinNoise.hpp"
namespace MCRFT
{
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
        void initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_y, const int chunk_size_x, const int chunk_size_y);

    private:
    };
    class World
    {
    public:
        void init();
        bool is_block_occupied(int x, int y, int z);

    private:
        const int CHUNK_SIZE_X = 16;
        const int CHUNK_SIZE_Y = 384;
        const int CHUNK_SIZE_Z = 16;
        Chunk *m_chunks[16][16];
    };
};

#endif