#ifndef READ_OBJ_VERTICES_FACES_MATERIALS_3_H
#define READ_OBJ_VERTICES_FACES_MATERIALS_3_H

#include <filesystem>

#include "Vector_3.h"

namespace read_obj_vertices_faces_materials_3 {

std::vector<std::string> split(const std::string &input, const char &delim) {
    std::stringstream stream = std::stringstream(input);
    std::vector<std::string> output;
    std::string part;
    while (std::getline(stream, part, delim)) {
        if (part.length() > 0) {
            output.push_back(part);
        }
    }
    return output;
}

bool read_obj_vertices_faces_materials_3(
    const std::string &fname, std::vector<Vector_3<double>> &points,
    std::vector<std::vector<size_t>> &faces,
    std::vector<std::string> &materials,
    std::vector<std::string> &material_files) {
    std::filesystem::path fpath = std::filesystem::path(fname);
    std::string extension = fpath.extension().string();
    if (extension != ".obj") {
        std::cerr << "Unknown input file extension: " << extension << std::endl;
        return false;
    }

    points.clear();
    faces.clear();
    materials.clear();
    material_files.clear();

    std::ifstream file = std::ifstream(fpath);
    std::string material = "";
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> parts = split(line, ' ');
        if (parts.empty()) {
            continue;
        }

        if (parts[0] == "v") {
            if (parts.size() <= 3) {
                continue;
            }
            Vector_3<double> point = Vector_3<double>(
                std::stod(parts[1]), std::stod(parts[2]), std::stod(parts[3]));
            points.push_back(point);

        } else if (parts[0] == "f") {
            if (parts.size() <= 3) {
                continue;
            }
            std::vector<size_t> face;
            face.reserve(parts.size() - 1);
            for (int i = 1; i < parts.size(); ++i) {
                std::vector<std::string> indices = split(parts[i], '/');
                if (indices.empty()) {
                    continue;
                }
                face.push_back(std::stoull(indices[0]) - 1);
            }
            faces.push_back(face);
            materials.push_back(material);

        } else if (parts[0] == "usemtl") {
            if (parts.size() <= 1) {
                material = "";
            }
            size_t first_space = line.find_first_of(' ');
            std::string from_first_space = line.substr(first_space);
            size_t first_not_space = from_first_space.find_first_not_of(' ');
            material = from_first_space.substr(first_not_space);
        } else if (parts[0] == "mtllib") {
            if (parts.size() <= 1) {
                continue;
            }
            size_t first_space = line.find_first_of(' ');
            std::string from_first_space = line.substr(first_space);
            size_t first_not_space = from_first_space.find_first_not_of(' ');
            material_files.push_back(from_first_space.substr(first_not_space));
        }
    }
    file.close();

    return true;
}
} // namespace read_obj_vertices_faces_materials_3

#endif
