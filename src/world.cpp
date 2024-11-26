#include "world/world.hpp"
#include "iostream"

void addblockface(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction);

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
                auto current_chunk = std::make_shared<Chunk>();
                current_chunk->setup(&perlin, i, j, CHUNK_SIZE_X, CHUNK_SIZE_Z);
                m_chunks[i][j] = current_chunk;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "World init(): Exception: " << e.what() << std::endl;
    }
}

bool MCRFT::World::isblockoccupied(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || y >= 384)
    {
        return false;
    }

    unsigned int chunk_x = x / 16;
    unsigned int coord_x_within_chunk = x % 16;
    unsigned int segment = y / 16;
    unsigned int coord_y_within_chunk = y % 16;
    unsigned int chunk_z = z / 16;
    unsigned int coord_z_within_chunk = z % 16;

    if (chunk_x >= 16 || chunk_z >= 16)
    {
        return false;
    }

    auto curr_chunk = m_chunks[chunk_x][chunk_z];
    if (!curr_chunk)
    {
        return false;
    }

    auto curr_section = curr_chunk->m_sections[segment];
    if (!curr_section)
    {
        return false;
    }

    if (coord_x_within_chunk >= 16 || coord_y_within_chunk >= 16 || coord_z_within_chunk >= 16)
    {
        return false;
    }

    return curr_section->blocks[coord_x_within_chunk][coord_y_within_chunk][coord_z_within_chunk] != nullptr;
}

void MCRFT::Chunk::setup(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_z, 
                                    const int chunk_size_x, const int chunk_size_z)
{
    try
    {
        for (auto &section : m_sections)
        {
            section = std::make_shared<ChunkSection>();
        }

        m_chunk_x = chunk_x;
        m_chunk_z = chunk_z;

        const unsigned int starting_x_coord = chunk_x * chunk_size_x;
        const unsigned int starting_z_coord = chunk_z * chunk_size_z;
        const double noise_scale = 0.01;
        const double height_multiplier = 100.0;
        const unsigned int section_height = 16;

        // Generate height map for the chunk
        for (unsigned int x = 0; x < chunk_size_x; x++)
        {
            for (unsigned int z = 0; z < chunk_size_z; z++)
            {
                double noise = perlin->octave2D_01((starting_x_coord + x) * noise_scale, 
                                                   (starting_z_coord + z) * noise_scale, 4);
                m_max_height[x][z] = static_cast<int>(noise * height_multiplier);
            }
        }

        for (unsigned int section_idx = 0; section_idx < m_sections.size(); section_idx++)
        {
            auto &current_section = m_sections[section_idx];
            unsigned int min_height = section_idx * section_height;

            for (unsigned int x = 0; x < section_height; x++)
            {
                for (unsigned int y = 0; y < section_height; y++)
                {
                    for (unsigned int z = 0; z < section_height; z++)
                    {
                        if (min_height + y <= m_max_height[x][z])
                        {
                            current_section->blocks[x][y][z] = std::make_shared<Block>();
                        }
                        else
                        {
                            current_section->blocks[x][y][z] = nullptr;
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in setup(): " << e.what() << std::endl;
    }
}

bool MCRFT::World::generateallchunkmeshes()
{
    bool err = false;
    try
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                m_chunks[i][j]->generatesurfacemesh(this);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "World generate_all_chunk_meshes(): Exception: " << e.what() << std::endl;
    }
    return err;
}

void MCRFT::Chunk::generatesurfacemesh(MCRFT::World *world)
{
    try
    {
        if (!world)
        {
            std::cerr << "Chunk generatesurfacemesh(): world is a nullptr" << std::endl;
            return;
        }
        m_mesh_vertices.clear();
        for (unsigned int i = 0; i < 16; i++)
        {
            for (unsigned int j = 0; j < 16; j++)
            {
                int max_height = m_max_height[i][j];
                for (int k = 0; k <= max_height; k++)
                {
                    int x_coord = m_chunk_x * 16 + i;
                    int y_coord = k;
                    int z_coord = m_chunk_z * 16 + j;
                    if (!world->isblockoccupied(x_coord, y_coord, z_coord))
                    {
                        continue;
                    }
                    const std::vector<MCRFT::Direction> directions = {
                        MCRFT::Direction::LEFT,
                        MCRFT::Direction::RIGHT,
                        MCRFT::Direction::TOP,
                        MCRFT::Direction::BOTTOM,
                        MCRFT::Direction::BACK,
                        MCRFT::Direction::FRONT
                    };
                    // Check and add faces for each direction
                    for (const auto& dir : directions)
                    {
                        int dx = 0, dy = 0, dz = 0;
                        switch (dir)
                        {
                            case MCRFT::Direction::LEFT:   dx = 1; break;
                            case MCRFT::Direction::RIGHT:  dx = -1; break;
                            case MCRFT::Direction::TOP:    dy = 1; break;
                            case MCRFT::Direction::BOTTOM: dy = -1; break;
                            case MCRFT::Direction::BACK:   dz = 1; break;
                            case MCRFT::Direction::FRONT:  dz = -1; break;
                        }
                        // If the adjacent block is not occupied, add face
                        if (!world->isblockoccupied(x_coord + dx, y_coord + dy, z_coord + dz))
                        {
                            addblockface(&m_mesh_vertices, x_coord, y_coord, z_coord, dir);
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Chunk generatesurfacemesh(): Exception: " << e.what() << std::endl;
    }
}
void addblockface(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction)
{
    float sides_left_x = (0.0 * 16.0) / 256.0;
    float sides_right_x = (1.0 * 16.0) / 256.0;
    float sides_top_y = (3.0 * 16.0) / 256.0;
    float sides_bottom_y = (2.0 * 16.0) / 256.0;
    const float GRASS_SIDE_LEFT = (2.0 * 64.0) / 3048.0;
    const float GRASS_SIDE_RIGHT = (3.0 * 64.0) / 3048.0;
    const float GRASS_TOP_LEFT = (0.0 * 64.0) / 3048.0;
    const float GRASS_TOP_RIGHT = (1.0 * 64.0) / 3048.0;
    const float GRASS_BOTTOM_LEFT = (1.0 * 64.0) / 3048.0;
    const float GRASS_BOTTOM_RIGHT = (2.0 * 64.0) / 3048.0;
    float VOXEL_VERTICES[] = {
        // front
        -0.5f, -0.5f, -0.5f, GRASS_SIDE_RIGHT, 1.0f,
        0.5f, -0.5f, -0.5f, GRASS_SIDE_LEFT, 1.0f,
        0.5f, 0.5f, -0.5f, GRASS_SIDE_LEFT, 0.0f,
        0.5f, 0.5f, -0.5f, GRASS_SIDE_LEFT, 0.0f,
        -0.5f, 0.5f, -0.5f, GRASS_SIDE_RIGHT, 0.0f,
        -0.5f, -0.5f, -0.5f, GRASS_SIDE_RIGHT, 1.0f,
        // back
        -0.5f, -0.5f, 0.5f, GRASS_SIDE_RIGHT, 1.0f,
        0.5f, -0.5f, 0.5f, GRASS_SIDE_LEFT, 1.0f,
        0.5f, 0.5f, 0.5f, GRASS_SIDE_LEFT, 0.0f,
        0.5f, 0.5f, 0.5f, GRASS_SIDE_LEFT, 0.0f,
        -0.5f, 0.5f, 0.5f, GRASS_SIDE_RIGHT, 0.0f,
        -0.5f, -0.5f, 0.5f, GRASS_SIDE_RIGHT, 1.0f,
        // right
        -0.5f, 0.5f, 0.5f, GRASS_SIDE_RIGHT, 0.0f,
        -0.5f, 0.5f, -0.5f, GRASS_SIDE_LEFT, 0.0f,
        -0.5f, -0.5f, -0.5f, GRASS_SIDE_LEFT, 1.0f,
        -0.5f, -0.5f, -0.5f, GRASS_SIDE_LEFT, 1.0f,
        -0.5f, -0.5f, 0.5f, GRASS_SIDE_RIGHT, 1.0f,
        -0.5f, 0.5f, 0.5f, GRASS_SIDE_RIGHT, 0.0f,
        // left
        0.5f, 0.5f, 0.5f, GRASS_SIDE_RIGHT, 0.0f,
        0.5f, 0.5f, -0.5f, GRASS_SIDE_LEFT, 0.0f,
        0.5f, -0.5f, -0.5f, GRASS_SIDE_LEFT, 1.0f,
        0.5f, -0.5f, -0.5f, GRASS_SIDE_LEFT, 1.0f,
        0.5f, -0.5f, 0.5f, GRASS_SIDE_RIGHT, 1.0f,
        0.5f, 0.5f, 0.5f, GRASS_SIDE_RIGHT, 0.0f,
        // bottom
        -0.5f, -0.5f, -0.5f, GRASS_BOTTOM_LEFT, 1.0f,
        0.5f, -0.5f, -0.5f, GRASS_BOTTOM_RIGHT, 1.0f,
        0.5f, -0.5f, 0.5f, GRASS_BOTTOM_RIGHT, 0.0f,
        0.5f, -0.5f, 0.5f, GRASS_BOTTOM_RIGHT, 0.0f,
        -0.5f, -0.5f, 0.5f, GRASS_BOTTOM_LEFT, 0.0f,
        -0.5f, -0.5f, -0.5f, GRASS_BOTTOM_LEFT, 1.0f,
        // top
        -0.5f, 0.5f, -0.5f, GRASS_TOP_LEFT, 1.0f,
        0.5f, 0.5f, -0.5f, GRASS_TOP_RIGHT, 1.0f,
        0.5f, 0.5f, 0.5f, GRASS_TOP_RIGHT, 0.0f,
        0.5f, 0.5f, 0.5f, GRASS_TOP_RIGHT, 0.0f,
        -0.5f, 0.5f, 0.5f, GRASS_TOP_LEFT, 0.0f,
        -0.5f, 0.5f, -0.5f, GRASS_TOP_LEFT, 1.0f};
    int index = 0;
    switch (direction)
    {
    case MCRFT::Direction::TOP:
        index = 30 * 5;
        break;
    case MCRFT::Direction::BOTTOM:
        index = 24 * 5;
        break;
    case MCRFT::Direction::LEFT:
        index = 18 * 5;
        break;
    case MCRFT::Direction::RIGHT:
        index = 12 * 5;
        break;
    case MCRFT::Direction::FRONT:
        index = 0 * 5;
        break;
    case MCRFT::Direction::BACK:
        index = 6 * 5;
        break;
    }
    for (unsigned int i = 0; i < 6; i++)
    {
        for (unsigned int j = 0; j < 5; j++)
        {
            vertices->push_back(VOXEL_VERTICES[index]);
            index += 1;
        }
        vertices->push_back(x);
        vertices->push_back(y);
        vertices->push_back(z);
    }
}
bool MCRFT::World::isinsideblock(int x, int y, int z)
{
    bool res = false;
    res = res || this->isblockoccupied(x + 1, y, z);
    res = res || this->isblockoccupied(x, y, z + 1);
    res = res || this->isblockoccupied(x + 1, y, z + 1);
    res = res || this->isblockoccupied(x + 1, y + 1, z);
    res = res || this->isblockoccupied(x, y + 1, z + 1);
    res = res || this->isblockoccupied(x + 1, y + 1, z + 1);
    return res;
}
std::shared_ptr<MCRFT::Chunk> MCRFT::World::get_chunk(int x, int z)
{
    try
    {
        if (x < 0 || x >= 16)
        {
            return nullptr;
        }
        if (z < 0 || z >= 16)
        {
            return nullptr;
        }
        return m_chunks[x][z];
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk getchunk(): Exception: " << e.what() << std::endl;
    }
}
bool MCRFT::Chunk::eraseblock(int x, int y, int z)
{
    bool result = false;
    try
    {
        int chunk_segment = floor(y / 16);
        int block_x = x - (16 * m_chunk_x);
        int block_y = y % 16;
        int block_z = z - (16 * m_chunk_z);

        if (m_sections[chunk_segment] != nullptr &&
            m_sections[chunk_segment]->blocks[block_x][block_y][block_z] != nullptr)
        {
            // Reset the smart pointer to release ownership of the block
            m_sections[chunk_segment]->blocks[block_x][block_y][block_z].reset();
            result = true;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk eraseblock(): Exception: " << e.what() << std::endl;
    }
    return result;
}
bool MCRFT::World::eraseblock(int x, int y, int z)
{
    bool result = false;
    try
    {
        std::shared_ptr<MCRFT::Chunk> curr_chunk = this->get_chunk(floor(x / 16), floor(z / 16));
        if (curr_chunk == nullptr)
        {
            return result;
        }
        result = curr_chunk->eraseblock(x, y, z);
        if (result)
        {
            curr_chunk->generatesurfacemesh(this);
        }
        else
        {
            std::cout << "ERROR, didnt generate mesh" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk eraseblock(): Exception: " << e.what() << std::endl;
    }
    return result;
}
void MCRFT::World::castray(Camera *camera, glm::vec3 position)
{
    try
    {
        if (camera == nullptr)
        {
            return;
        }

        glm::vec3 rayOrigin = camera->m_camera_pos;
        glm::vec3 rayDirection = camera->getraydirection();

        for (float t = 0; t < 5; t += 0.1f)
        {
            glm::vec3 currentPos = rayOrigin + t * rayDirection;
            if (this->isblockoccupied(floor(currentPos.x), floor(currentPos.y), floor(currentPos.z)))
            {
                this->eraseblock(floor(currentPos.x), floor(currentPos.y), floor(currentPos.z));
                return;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk castray(): Exception: " << e.what() << std::endl;
    }
}