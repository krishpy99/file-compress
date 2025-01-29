#include "pkg/file_io.h"
#include "lib/httplib.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace httplib;

void setupServer(Server& server) {
    server.Post("/compress", [](const Request& req, Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("No file uploaded", "text/plain");
            return;
        }

        const auto& file = req.get_file_value("file");
        const string temp_input = "temp_input";
        const string temp_output = "temp_compressed.bin";

        ofstream temp_file(temp_input, ios::binary);
        temp_file.write(file.content.data(), file.content.size());
        temp_file.close();

        if (!compressFile(temp_input, temp_output)) {
            res.status = 500;
            res.set_content("Compression failed", "text/plain");
            remove(temp_input.c_str());
            return;
        }

        ifstream compressed_file(temp_output, ios::binary);
        if (!compressed_file) {
            res.status = 500;
            res.set_content("Failed to read compressed file", "text/plain");
            remove(temp_input.c_str());
            remove(temp_output.c_str());
            return;
        }

        string compressed_data((istreambuf_iterator<char>(compressed_file)), istreambuf_iterator<char>());
        compressed_file.close();

        remove(temp_input.c_str());
        remove(temp_output.c_str());

        res.set_content(compressed_data, "application/octet-stream");
    });

    server.Post("/decompress", [](const Request& req, Response& res) {
        if (!req.has_file("file")) {
            res.status = 400;
            res.set_content("No file uploaded", "text/plain");
            return;
        }

        const auto& file = req.get_file_value("file");
        const string temp_input = "temp_compressed.bin";
        const string temp_output = "temp_decompressed.txt";

        ofstream temp_file(temp_input, ios::binary);
        temp_file.write(file.content.data(), file.content.size());
        temp_file.close();

        if (!decompressFile(temp_input, temp_output)) {
            res.status = 500;
            res.set_content("Decompression failed", "text/plain");
            remove(temp_input.c_str());
            return;
        }

        ifstream decompressed_file(temp_output, ios::binary);
        if (!decompressed_file) {
            res.status = 500;
            res.set_content("Failed to read decompressed file", "text/plain");
            remove(temp_input.c_str());
            remove(temp_output.c_str());
            return;
        }

        string decompressed_data((istreambuf_iterator<char>(decompressed_file)), istreambuf_iterator<char>());
        decompressed_file.close();

        remove(temp_input.c_str());
        remove(temp_output.c_str());

        res.set_content(decompressed_data, "application/octet-stream");
    });
}

int main() {
    Server server;
    setupServer(server);

    cout << "Server started at http://localhost:8080\n";
    server.listen("localhost", 8080);

    return 0;
}
