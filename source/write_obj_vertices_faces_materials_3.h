#ifndef WRITE_OBJ_VERTICES_FACES_MATERIALS_3_H
#define WRITE_OBJ_VERTICES_FACES_MATERIALS_3_H

#include <filesystem>

#include "source/Vector_3.h"

namespace write_obj_vertices_faces_materials_3 {
bool write_obj_vertices_faces_materials_3(
    const std::string &fname, std::vector<Vector_3<double>> &points,
    std::vector<std::vector<size_t>> &faces,
    std::vector<std::string> &materials,
    std::vector<std::string> &material_files) {
    std::map<std::string, std::vector<std::vector<size_t>>> material_faces_map;
    for (int i = 0; i < std::min(faces.size(), materials.size()); ++i) {
        std::vector<size_t> face = faces[i];
        std::string material = materials[i];
        if (!material_faces_map.count(material) <= 0) {
            material_faces_map.insert(
                std::pair<std::string, std::vector<std::vector<size_t>>>(
                    material, std::vector<std::vector<size_t>>()));
        }
        material_faces_map[material].push_back(face);
    }

    std::filesystem::path fpath = std::filesystem::path(fname);
    std::ofstream file = std::ofstream(fpath);

    for (std::string material_file : material_files) {
        file << "mtllib " << material_file << "\n";
    }

    for (Vector_3<double> point : points) {
        file << "v " << std::to_string(point.x) << " "
             << std::to_string(point.y) << " " << std::to_string(point.z)
             << "\n";
    }

    for (std::pair<std::string, std::vector<std::vector<size_t>>>
             material_faces : material_faces_map) {
        std::string material = material_faces.first;
        std::vector<std::vector<size_t>> faces = material_faces.second;
        file << "usemtl " << material << "\n";
        for (std::vector<size_t> face : faces) {
            std::string line = "f ";
            for (size_t i : face) {
                line += std::to_string(i + 1) + " ";
            }
            line.pop_back();
            file << line << "\n";
        }
    }

    file.close();

    return true;
}
} // namespace write_obj_vertices_faces_materials_3

#endif
