#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <cstdint>

struct HuffmanNode {
    uint8_t byte;
    uint32_t freq;
    HuffmanNode *left, *right;
    HuffmanNode(uint8_t b, uint32_t f) : byte(b), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(uint32_t f) : byte(0), freq(f), left(nullptr), right(nullptr) {}
};

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

std::unordered_map<uint8_t, uint32_t> buildFrequencyTable(const std::vector<uint8_t>& data);
HuffmanNode* buildHuffmanTree(const std::unordered_map<uint8_t, uint32_t>& freq);
void generateCodes(HuffmanNode* node, const std::string& code, std::unordered_map<uint8_t, std::string>& codes);
void writeHeader(std::ofstream& out, const std::unordered_map<uint8_t, uint32_t>& freq);
void compressData(const std::vector<uint8_t>& data, const std::unordered_map<uint8_t, std::string>& codes, std::ofstream& out);
std::unordered_map<uint8_t, uint32_t> readHeader(std::ifstream& in);
void decompressData(std::ifstream& in, HuffmanNode* root, uint32_t data_size, std::ofstream& out);
void deleteTree(HuffmanNode* node);

#endif
