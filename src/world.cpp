#include "world/world.hpp"
#include "iostream"


void MCRFT::World::createnew()
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

    unsigned int chunk_x = x / CHUNK_LENGTH;
    unsigned int coord_x_within_chunk = x % CHUNK_LENGTH;
    unsigned int segment = y / CHUNK_HEIGHT;
    unsigned int coord_y_within_chunk = y % CHUNK_HEIGHT;
    unsigned int chunk_z = z / CHUNK_WIDTH;
    unsigned int coord_z_within_chunk = z % CHUNK_WIDTH;

    if (chunk_x >= CHUNK_LENGTH || chunk_z >= CHUNK_WIDTH)
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

    if (coord_x_within_chunk >= CHUNK_LENGTH || coord_y_within_chunk >= CHUNK_HEIGHT || coord_z_within_chunk >= CHUNK_WIDTH)
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
        const unsigned int section_height = CHUNK_HEIGHT;

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
        for (unsigned int i = 0; i < CHUNK_WIDTH; i++)
        {
            for (unsigned int j = 0; j < CHUNK_LENGTH; j++)
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
                            world->addblockface(&m_mesh_vertices, x_coord, y_coord, z_coord, dir, 1);
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
void MCRFT::World::addblockface(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction, int blockid)
{
    auto blocktype = m_blockmanager.getblockinfo(blockid);
    if(blocktype == nullptr){
        std::cout << "addblockface: blocktype is nullptr" << std::endl;
        throw std::runtime_error("blocktype is invalid");
    }

    const float SIDE_LEFT = (blocktype->m_sidebounds.first * BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    const float SIDE_RIGHT = (blocktype->m_sidebounds.second * BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    const float TOP_LEFT = (blocktype->m_topbounds.first * BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    const float TOP_RIGHT = (blocktype->m_topbounds.second* BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    const float BOTTOM_LEFT = (blocktype->m_bottombounds.first * BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    const float BOTTOM_RIGHT = (blocktype->m_bottombounds.second * BLOCK_SIZE_PIXEL_DIM) / PIXEL_PIC_WIDTH;
    float VOXEL_VERTICES[] = {
        // front
        -0.5f, -0.5f, -0.5f, SIDE_RIGHT, 1.0f,
        0.5f, -0.5f, -0.5f, SIDE_LEFT, 1.0f,
        0.5f, 0.5f, -0.5f, SIDE_LEFT, 0.0f,
        0.5f, 0.5f, -0.5f, SIDE_LEFT, 0.0f,
        -0.5f, 0.5f, -0.5f, SIDE_RIGHT, 0.0f,
        -0.5f, -0.5f, -0.5f, SIDE_RIGHT, 1.0f,
        // back
        -0.5f, -0.5f, 0.5f, SIDE_RIGHT, 1.0f,
        0.5f, -0.5f, 0.5f, SIDE_LEFT, 1.0f,
        0.5f, 0.5f, 0.5f, SIDE_LEFT, 0.0f,
        0.5f, 0.5f, 0.5f, SIDE_LEFT, 0.0f,
        -0.5f, 0.5f, 0.5f, SIDE_RIGHT, 0.0f,
        -0.5f, -0.5f, 0.5f, SIDE_RIGHT, 1.0f,
        // right
        -0.5f, 0.5f, 0.5f, SIDE_RIGHT, 0.0f,
        -0.5f, 0.5f, -0.5f, SIDE_LEFT, 0.0f,
        -0.5f, -0.5f, -0.5f, SIDE_LEFT, 1.0f,
        -0.5f, -0.5f, -0.5f, SIDE_LEFT, 1.0f,
        -0.5f, -0.5f, 0.5f, SIDE_RIGHT, 1.0f,
        -0.5f, 0.5f, 0.5f, SIDE_RIGHT, 0.0f,
        // left
        0.5f, 0.5f, 0.5f, SIDE_RIGHT, 0.0f,
        0.5f, 0.5f, -0.5f, SIDE_LEFT, 0.0f,
        0.5f, -0.5f, -0.5f, SIDE_LEFT, 1.0f,
        0.5f, -0.5f, -0.5f, SIDE_LEFT, 1.0f,
        0.5f, -0.5f, 0.5f, SIDE_RIGHT, 1.0f,
        0.5f, 0.5f, 0.5f, SIDE_RIGHT, 0.0f,
        // bottom
        -0.5f, -0.5f, -0.5f, BOTTOM_LEFT, 1.0f,
        0.5f, -0.5f, -0.5f, BOTTOM_RIGHT, 1.0f,
        0.5f, -0.5f, 0.5f, BOTTOM_RIGHT, 0.0f,
        0.5f, -0.5f, 0.5f, BOTTOM_RIGHT, 0.0f,
        -0.5f, -0.5f, 0.5f, BOTTOM_LEFT, 0.0f,
        -0.5f, -0.5f, -0.5f, BOTTOM_LEFT, 1.0f,
        // top
        -0.5f, 0.5f, -0.5f, TOP_LEFT, 1.0f,
        0.5f, 0.5f, -0.5f, TOP_RIGHT, 1.0f,
        0.5f, 0.5f, 0.5f, TOP_RIGHT, 0.0f,
        0.5f, 0.5f, 0.5f, TOP_RIGHT, 0.0f,
        -0.5f, 0.5f, 0.5f, TOP_LEFT, 0.0f,
        -0.5f, 0.5f, -0.5f, TOP_LEFT, 1.0f};
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
std::shared_ptr<MCRFT::Chunk> MCRFT::World::getchunk(int x, int z)
{
    try
    {
        if (x < 0 || x >= CHUNK_LENGTH)
        {
            return nullptr;
        }
        if (z < 0 || z >= CHUNK_WIDTH)
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
        int chunk_segment = floor(y / CHUNK_HEIGHT);
        int block_x = x - (CHUNK_LENGTH * m_chunk_x);
        int block_y = y % CHUNK_HEIGHT;
        int block_z = z - (CHUNK_WIDTH * m_chunk_z);

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
        std::shared_ptr<MCRFT::Chunk> curr_chunk = this->getchunk(floor(x / CHUNK_LENGTH), floor(z / CHUNK_HEIGHT));
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
void MCRFT::World::findhitblock(Camera *camera, glm::vec3 position)
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
        std::cout << "Chunk findhitblock(): Exception: " << e.what() << std::endl;
    }
}