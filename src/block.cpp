#include "world/block.hpp"

bool MCRFT::BlockManager::loadblocks(const char* blockspath)
{
    std::ifstream file(blockspath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file in loadblocks()");
    }
    std::string line;
    // name,id,numcoords,...
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string key;
        std::string token;
        int id;
        int count;

        if (!std::getline(lineStream, key, ',')) {
            throw std::runtime_error("Error reading key from line: " + line);
        }
        if (!std::getline(lineStream, token, ',')) {
            throw std::runtime_error("Error reading count from line: " + line);
        }
        id = std::stoi(token);
        if (!std::getline(lineStream, token, ',')) {
            throw std::runtime_error("Error reading count from line: " + line);
        }
        count = std::stoi(token);
        std::vector<float> values;
        for (int i = 0; i < count; ++i) {
            if (!std::getline(lineStream, token, ',')) {
                throw std::runtime_error("Error reading value from line: " + line);
            }
            values.push_back(std::stoi(token));
        }
        auto newblock = std::make_shared<BlockType>();
        // count can either be 1 or 3, this is the number of possible unique sides to a block
        // always goes bottom, side, top
        if(count == 1){
            newblock->m_bottombounds = {values[0], values[0] + 1};
            newblock->m_sidebounds = {values[0], values[0] + 1};
            newblock->m_topbounds = {values[0], values[0] + 1};
        }else if(count == 3){
            newblock->m_bottombounds = {values[0], values[0] + 1};
            newblock->m_sidebounds = {values[1], values[1] + 1};
            newblock->m_topbounds = {values[2], values[2] + 1};
        }else{
            throw std::runtime_error("Invalid count");
        }
        newblock->m_blocktype = key;
        m_strblocksmap[key] = newblock;
        m_intblocksmap[id] = newblock;
    }
    file.close();
    return false;
}

std::shared_ptr<MCRFT::BlockType> MCRFT::BlockManager::getblockinfo(std::string blockname)
{
    if(m_strblocksmap.find(blockname) != m_strblocksmap.end()){
        return m_strblocksmap[blockname];
    }
    return nullptr;
}
std::shared_ptr<MCRFT::BlockType> MCRFT::BlockManager::getblockinfo(int blockid)
{
    if(m_intblocksmap.find(blockid) != m_intblocksmap.end()){
        return m_intblocksmap[blockid];
    }
    return nullptr;
}