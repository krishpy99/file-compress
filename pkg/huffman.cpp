#include "huffman.h"
#include <fstream>
#include <queue>

using namespace std;

unordered_map<uint8_t, uint32_t> buildFrequencyTable(const vector<uint8_t>& data) {
    unordered_map<uint8_t, uint32_t> freq;
    for (uint8_t b : data) freq[b]++;
    return freq;
}

HuffmanNode* buildHuffmanTree(const unordered_map<uint8_t, uint32_t>& freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    for (const auto& pair : freq)
        pq.push(new HuffmanNode(pair.first, pair.second));
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* internal = new HuffmanNode(left->freq + right->freq);
        internal->left = left;
        internal->right = right;
        pq.push(internal);
    }
    return pq.empty() ? nullptr : pq.top();
}

void generateCodes(HuffmanNode* node, const string& code, unordered_map<uint8_t, string>& codes) {
    if (!node) return;
    if (!node->left && !node->right) codes[node->byte] = code;
    generateCodes(node->left, code + "0", codes);
    generateCodes(node->right, code + "1", codes);
}

void writeHeader(ofstream& out, const unordered_map<uint8_t, uint32_t>& freq) {
    uint32_t num_symbols = freq.size();
    out.write(reinterpret_cast<const char*>(&num_symbols), sizeof(num_symbols));
    for (const auto& pair : freq) {
        uint8_t b = pair.first;
        uint32_t f = pair.second;
        out.write(reinterpret_cast<const char*>(&b), sizeof(b));
        out.write(reinterpret_cast<const char*>(&f), sizeof(f));
    }
}

void compressData(const vector<uint8_t>& data, const unordered_map<uint8_t, string>& codes, ofstream& out) {
    uint8_t current_byte = 0;
    int bit_position = 0;
    for (uint8_t b : data) {
        for (char c : codes.at(b)) {
            if (c == '1')
                current_byte |= 1 << (7 - bit_position);
            if (++bit_position == 8) {
                out.put(current_byte);
                current_byte = bit_position = 0;
            }
        }
    }
    if (bit_position > 0) out.put(current_byte);
}

unordered_map<uint8_t, uint32_t> readHeader(ifstream& in) {
    unordered_map<uint8_t, uint32_t> freq;
    uint32_t num_symbols;
    in.read(reinterpret_cast<char*>(&num_symbols), sizeof(num_symbols));
    for (uint32_t i = 0; i < num_symbols; ++i) {
        uint8_t b;
        uint32_t f;
        in.read(reinterpret_cast<char*>(&b), sizeof(b));
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        freq[b] = f;
    }
    return freq;
}

void decompressData(ifstream& in, HuffmanNode* root, uint32_t data_size, ofstream& out) {
    HuffmanNode* current = root;
    uint8_t byte;
    uint32_t decoded = 0;
    while (decoded < data_size && in.get(reinterpret_cast<char&>(byte))) {
        for (int i = 7; i >= 0; --i) {
            current = (byte >> i) & 1 ? current->right : current->left;
            if (!current->left && !current->right) {
                out.put(current->byte);
                if (++decoded == data_size) return;
                current = root;
            }
        }
    }
}

void deleteTree(HuffmanNode* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}
