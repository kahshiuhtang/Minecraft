#ifndef MCRFT_WORLD_HPP
#define MCRFT_WORLD_HPP

#include <vector>
#include <memory>
#include <algorithm> // For std::fill

#include "PerlinNoise.hpp"
#include "camera.hpp"

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
            : blocks(16, std::vector<std::vector<std::shared_ptr<Block>>>(16, std::vector<std::shared_ptr<Block>>(16))),
              blockLight(16, std::vector<std::vector<uint8_t>>(16, std::vector<uint8_t>(16))),
              skyLight(16, std::vector<std::vector<uint8_t>>(16, std::vector<uint8_t>(16)))
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
            : m_sections(24),
              m_max_height(16, std::vector<int>(16, 0))
        {
            // Initialize all shared pointers to nullptr
            std::fill(m_sections.begin(), m_sections.end(), nullptr);
        }

        void initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_z, const int chunk_size_x, const int chunk_size_z);
        void generate_mesh(World *world);
        bool remove_block(int x, int y, int z);

    private:
    };

    class World
    {
    public:
        void init();
        bool is_block_occupied(int x, int y, int z);
        bool generate_all_chunk_meshes();
        void cast_ray(Camera *camera, glm::vec3 position);
        bool remove_block(int x, int y, int z);
        bool isInsideBlock(int x, int y, int z);
        std::shared_ptr<MCRFT::Chunk> get_chunk(int x, int z);

        World()
            : m_chunks(16, std::vector<std::shared_ptr<Chunk>>(16))
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
