#ifndef MCRFT_BLOCK_HPP
#define MCRFT_BLOCK_HPP

#include <string>
#include <memory>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace MCRFT
{
    class Block
    {
    public:
        int type;
        unsigned int m_metadata;
    private:
    };
    class BlockType
    {
        public:
            std::string m_blocktype;
            std::pair<float, float> m_sidebounds;
            std::pair<float, float> m_topbounds;
            std::pair<float, float> m_bottombounds;
    };
    class BlockManager
    {
        public:
            bool loadblocks(const char* blockspath);
            std::shared_ptr<MCRFT::BlockType> getblockinfo(std::string blockname);
            std::shared_ptr<MCRFT::BlockType> getblockinfo(int blockid);
        private:
            std::unordered_map<std::string, std::shared_ptr<MCRFT::BlockType>> m_strblocksmap;
            std::unordered_map<int, std::shared_ptr<MCRFT::BlockType>> m_intblocksmap;
    };
}
#endif