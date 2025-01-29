#include "file_io.h"
#include "huffman.h"
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

bool compressFile(const string& inputFilename, const string& outputFilename) {
    ifstream in_file(inputFilename, ios::binary);
    if (!in_file.is_open()) {
        cerr << "Failed to open input file: " << inputFilename << endl;
        return false;
    }

    vector<uint8_t> data((istreambuf_iterator<char>(in_file)), istreambuf_iterator<char>());
    in_file.close();

    if (data.empty()) {
        cerr << "Input file is empty: " << inputFilename << endl;
        return false;
    }

    ofstream out_file(outputFilename, ios::binary);
    if (!out_file.is_open()) {
        cerr << "Failed to open output file: " << outputFilename << endl;
        return false;
    }

    auto freq = buildFrequencyTable(data);
    HuffmanNode* root = buildHuffmanTree(freq);
    unordered_map<uint8_t, string> codes;
    generateCodes(root, "", codes);

    writeHeader(out_file, freq);
    compressData(data, codes, out_file);
    out_file.close();

    deleteTree(root);
    return true;
}

bool decompressFile(const string& inputFilename, const string& outputFilename) {
    ifstream in_file(inputFilename, ios::binary);
    if (!in_file.is_open()) {
        cerr << "Failed to open input file: " << inputFilename << endl;
        return false;
    }

    ofstream out_file(outputFilename, ios::binary);
    if (!out_file.is_open()) {
        cerr << "Failed to open output file: " << outputFilename << endl;
        in_file.close();
        return false;
    }

    auto freq = readHeader(in_file);
    uint32_t data_size = 0;
    for (const auto& pair : freq) data_size += pair.second;

    HuffmanNode* root = buildHuffmanTree(freq);
    if (root->left == nullptr && root->right == nullptr) {
        uint8_t b = root->byte;
        for (uint32_t i = 0; i < data_size; ++i)
            out_file.put(b);
    } else {
        decompressData(in_file, root, data_size, out_file);
    }

    in_file.close();
    out_file.close();
    deleteTree(root);
    return true;
}
