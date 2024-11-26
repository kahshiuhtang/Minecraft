#include "world/world.hpp"
#include "iostream"

void AddFace(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction);

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
                current_chunk->initialize_chunk(&perlin, i, j, CHUNK_SIZE_X, CHUNK_SIZE_Z);
                m_chunks[i][j] = current_chunk;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "World init(): Exception: " << e.what() << std::endl;
    }
}

bool MCRFT::World::is_block_occupied(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0)
    {
        return false;
    }
    if (y >= 384)
    {
        return false;
    }
    unsigned int chunk_x = x / 16;
    unsigned int coord_x_within_chunk = x % 16;
    unsigned int segment = y / 16;
    unsigned int coord_y_within_chunk = y % 16;
    unsigned int chunk_z = z / 16;
    unsigned int coord_z_within_chunk = z % 16;
    if (chunk_x >= 16 || chunk_x < 0)
    {
        return false;
    }
    if (chunk_z >= 16 || chunk_z < 0)
    {
        return false;
    }
    std::shared_ptr<MCRFT::Chunk> curr_chunk = m_chunks[chunk_x][chunk_z];
    if (curr_chunk == nullptr)
    {
        return false;
    }
    std::shared_ptr<MCRFT::ChunkSection> curr_section = curr_chunk->m_sections[segment];
    if (curr_section == nullptr)
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
    bool ans = !(curr_section->blocks[coord_x_within_chunk][coord_y_within_chunk][coord_z_within_chunk] == nullptr);
    //if(!ans) std::cout << y << std::endl;
    return ans;
}
void MCRFT::Chunk::initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_z, const int chunk_size_x, const int chunk_size_z)
{
    try
    {
        // for every section, we want to make a new section (should be 24)
        for (unsigned int i = 0; i < m_sections.size(); i++)
        {
            auto new_section = std::make_shared<ChunkSection>();
            m_sections[i] = new_section;
        }
        m_chunk_x = chunk_x;
        m_chunk_z = chunk_z;
        const unsigned int starting_x_coord = chunk_x * chunk_size_x;
        const unsigned int starting_z_coord = chunk_z * chunk_size_z;
        // for every x, z (16 x 16), we want to get the height at THAT specific x, z
        for (unsigned int i = 0; i < chunk_size_x; i++)
        {
            for (unsigned int j = 0; j < chunk_size_z; j++)
            {
                double noise = perlin->octave2D_01(((starting_x_coord + i) * 0.01), ((starting_z_coord + j) * 0.01), 4);
                noise *= 100;
                const int coordinate_height = static_cast<int>(noise);
                m_max_height[i][j] = coordinate_height;
            }
        }
        for (unsigned int i = 0; i < m_sections.size(); i++)
        {
            unsigned int min_height = (i) * 16; // should probably make this 16 a constant
            auto current_section = m_sections[i];
            for (unsigned int u = 0; u < 16; u++) // x
            {
                for (unsigned int v = 0; v < 16; v++) // y
                {
                    for (unsigned int w = 0; w < 16; w++) // z
                    {
                        if (min_height + v <= m_max_height[u][w])
                        {
                            current_section->blocks[u][v][w] = std::make_shared<Block>();
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
        std::cout << "World initialize_chunk(): Exception: " << e.what() << std::endl;
    }
}
bool MCRFT::World::generate_all_chunk_meshes()
{
    bool err = false;
    try
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                m_chunks[i][j]->generate_mesh(this);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "World generate_all_chunk_meshes(): Exception: " << e.what() << std::endl;
    }
    return err;
}
void MCRFT::Chunk::generate_mesh(MCRFT::World *world)
{
    try
    {
        if (world == nullptr)
        {
            std::cout << "Chunk generate_mesh(): world is a nullptr" << std::endl;
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
                    int y_coord = k;
                    int x_coord = m_chunk_x * 16 + i;
                    int z_coord = m_chunk_z * 16 + j;
                    if (world->is_block_occupied(x_coord, y_coord, z_coord) == false)
                    {
                        continue;
                    }
                    if (world->is_block_occupied(x_coord + 1, y_coord, z_coord) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::LEFT);
                    }
                    if (world->is_block_occupied(x_coord - 1, y_coord, z_coord) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::RIGHT);
                    }
                    if (world->is_block_occupied(x_coord, y_coord + 1, z_coord) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::TOP);
                    }
                    if (world->is_block_occupied(x_coord, y_coord - 1, z_coord) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::BOTTOM);
                    }
                    if (world->is_block_occupied(x_coord, y_coord, z_coord + 1) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::BACK);
                    }
                    if (world->is_block_occupied(x_coord, y_coord, z_coord - 1) == false)
                    {
                        AddFace(&m_mesh_vertices, x_coord, y_coord, z_coord, MCRFT::Direction::FRONT);
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk generate_mesh(): Exception: " << e.what() << std::endl;
    }
}
void AddFace(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction)
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
bool MCRFT::World::isInsideBlock(int x, int y, int z)
{
    bool res = false;
    res = res || this->is_block_occupied(x + 1, y, z);
    res = res || this->is_block_occupied(x, y, z + 1);
    res = res || this->is_block_occupied(x + 1, y, z + 1);
    res = res || this->is_block_occupied(x + 1, y + 1, z);
    res = res || this->is_block_occupied(x, y + 1, z + 1);
    res = res || this->is_block_occupied(x + 1, y + 1, z + 1);
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
        std::cout << "Chunk generate_mesh(): Exception: " << e.what() << std::endl;
    }
}
bool MCRFT::Chunk::remove_block(int x, int y, int z)
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
        std::cout << "Chunk remove_block(): Exception: " << e.what() << std::endl;
    }
    return result;
}
bool MCRFT::World::remove_block(int x, int y, int z)
{
    bool result = false;
    try
    {
        std::shared_ptr<MCRFT::Chunk> curr_chunk = this->get_chunk(floor(x / 16), floor(z / 16));
        if (curr_chunk == nullptr)
        {
            return result;
        }
        result = curr_chunk->remove_block(x, y, z);
        if (result)
        {
            curr_chunk->generate_mesh(this);
        }
        else
        {
            std::cout << "ERROR, didnt generate mesh" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk remove_block(): Exception: " << e.what() << std::endl;
    }
    return result;
}
void MCRFT::World::cast_ray(Camera *camera, glm::vec3 position)
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

            // Check if currentPos is inside a block
            if (this->is_block_occupied(floor(currentPos.x), floor(currentPos.y), floor(currentPos.z)))
            {
                this->remove_block(floor(currentPos.x), floor(currentPos.y), floor(currentPos.z));
                return;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Chunk cast_ray(): Exception: " << e.what() << std::endl;
    }
}