#ifndef MCRFT_WORLD_HPP
#define MCRFT_WORLD_HPP

#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "PerlinNoise.hpp"

#include "camera.hpp"

#define CHUNK_WIDTH         16
#define CHUNK_LENGTH        16
#define CHUNK_HEIGHT        16
#define SECTIONS_PER_CHUNK  24

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
        std::vector<std::vector<std::vector<std::shared_ptr<Block>>>> blocks;
        std::vector<std::vector<std::vector<uint8_t>>> blockLight;
        std::vector<std::vector<std::vector<uint8_t>>> skyLight;

        ChunkSection()
            : blocks(CHUNK_LENGTH, std::vector<std::vector<std::shared_ptr<Block>>>(CHUNK_WIDTH, std::vector<std::shared_ptr<Block>>(CHUNK_HEIGHT))),
              blockLight(CHUNK_LENGTH, std::vector<std::vector<uint8_t>>(CHUNK_WIDTH, std::vector<uint8_t>(CHUNK_HEIGHT))),
              skyLight(CHUNK_LENGTH, std::vector<std::vector<uint8_t>>(CHUNK_WIDTH, std::vector<uint8_t>(CHUNK_HEIGHT)))
        {
            // Initialize all shared pointers to nullptr
            for (auto &plane : blocks)
                for (auto &row : plane)
                    std::fill(row.begin(), row.end(), nullptr);
        }
    };

    class Chunk
    {
    public:
        std::vector<std::shared_ptr<ChunkSection>> m_sections; // 24 sections to cover 384 blocks in height
        std::vector<std::vector<int>> m_max_height;
        unsigned int m_chunk_x;
        unsigned int m_chunk_z;
        std::vector<float> m_mesh_vertices;

        Chunk()
            : m_sections(SECTIONS_PER_CHUNK),
              m_max_height(CHUNK_LENGTH, std::vector<int>(CHUNK_WIDTH, 0))
        {
            // Initialize all shared pointers to nullptr
            std::fill(m_sections.begin(), m_sections.end(), nullptr);
        }

        void setup(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_z, const int chunk_size_x, const int chunk_size_z);
        void generatesurfacemesh(World *world);
        bool eraseblock(int x, int y, int z);

    private:
    };

    class World
    {
    public:
        void createnew();
        bool isblockoccupied(int x, int y, int z);
        bool generateallchunkmeshes();
        void castray(Camera *camera, glm::vec3 position);
        bool eraseblock(int x, int y, int z);
        bool isinsideblock(int x, int y, int z);
        std::shared_ptr<MCRFT::Chunk> get_chunk(int x, int z);

        World()
            : m_chunks(CHUNK_LENGTH, std::vector<std::shared_ptr<Chunk>>(CHUNK_WIDTH))
        {
        }

    private:
        const int CHUNK_SIZE_X = 16;
        const int CHUNK_SIZE_Y = 384;
        const int CHUNK_SIZE_Z = 16;
        std::vector<std::vector<std::shared_ptr<Chunk>>> m_chunks;
    };
};

#endif
