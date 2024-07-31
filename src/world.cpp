#include "world.hpp"
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
                Chunk *current_chunk = new Chunk();
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
void MCRFT::Chunk::initialize_chunk(const siv::PerlinNoise *perlin, unsigned int chunk_x, unsigned int chunk_z, const int chunk_size_x, const int chunk_size_z)
{
    try
    {
        // for every section, we want to make a new section (should be 24)
        for (unsigned int i = 0; i < sizeof(m_sections) / sizeof(m_sections[0]); i++)
        {
            ChunkSection *new_section = new ChunkSection();
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
                        if (min_height + v <= m_max_height[u][w])
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
                int y_coord = m_max_height[i][j];
                int x_coord = m_chunk_x * 16 + i;
                int z_coord = m_chunk_z * 16 + j;
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
    catch (const std::exception &e)
    {
        std::cout << "Chunk generate_mesh(): Exception: " << e.what() << std::endl;
    }
}
void AddFace(std::vector<float> *vertices, int x, int y, int z, MCRFT::Direction direction)
{
    float VOXEL_VERTICES[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
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
MCRFT::Chunk *MCRFT::World::get_chunk(int x, int z)
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