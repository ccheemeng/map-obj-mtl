#include <getopt.h>

#include <CGAL/AABB_traits_3.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_triangle_primitive_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/barycenter.h>

#include "Vector_3.h"
#include "read_obj_vertices_faces_materials_3.h"
#include "write_obj_vertices_faces_materials_3.h"

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;
using Delaunay = CGAL::Delaunay_triangulation_3<
    K,
    CGAL::Triangulation_data_structure_3<
        CGAL::Triangulation_vertex_base_with_info_3<size_t, K>,
        CGAL::Delaunay_triangulation_cell_base_3<K>>,
    CGAL::Fast_location>;
using Triangle_3 = K::Triangle_3;
using Tree = CGAL::AABB_tree<
    CGAL::AABB_traits_3<K, CGAL::AABB_triangle_primitive_3<
                               K, std::vector<Triangle_3>::const_iterator>>>;

int main(int argc, char **argv) {
    // Input
    std::string source = argv[1];
    std::string target = argv[2];
    std::string out = argv[3] ? argv[3] : "";

    // static struct option long_options[] = {{"out", required_argument, 0,
    // 'o'},
    //                                        {0, 0, 0, 0}};
    // optind = 3;
    // int option_index = 0;
    // for (int i = 0; i < 1000; ++i) {
    //     int opt = getopt_long(argc, argv, "o:", long_options, &option_index);
    //     if (opt == -1) {
    //         break;
    //     }
    //     switch (opt) {
    //     case 'o': {
    //         out = optarg;
    //     }
    //     default: {
    //         std::cerr << "?? getopt returned character code " << opt << "??"
    //                   << std::endl;
    //     }
    //     }
    // }

    std::vector<Vector_3<double>> vector_3s;
    std::vector<std::vector<size_t>> source_faces;
    std::vector<std::string> source_materials;
    if (!read_obj_vertices_faces_materials_3::
            read_obj_vertices_faces_materials_3(source, vector_3s, source_faces,
                                                source_materials) ||
        source_faces.empty()) {
        std::cerr << "Invalid source input: " << source << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<Point_3> source_points;
    source_points.reserve(vector_3s.size());
    for (Vector_3<double> vector_3 : vector_3s) {
        Point_3 source_point = Point_3(vector_3.x, vector_3.y, vector_3.z);
        source_points.push_back(source_point);
    }
    std::cout << source << ": " << source_points.size() << " points, "
              << source_faces.size() << " faces " << std::endl;

    std::vector<Point_3> target_points;
    std::vector<std::vector<std::size_t>> target_faces;
    if (!CGAL::IO::read_polygon_soup(target, target_points, target_faces) ||
        source_faces.empty()) {
        std::cerr << "Invalid target input: " << target << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << target << ": " << target_points.size() << " points, "
              << target_faces.size() << " faces " << std::endl;

    // Source mesh preprocessing
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    std::cout << "Triangulating source mesh..." << std::endl;

    std::vector<Vector_3<size_t>> source_triangles;
    std::vector<std::string> new_materials;
    for (int i = 0; i < source_faces.size(); ++i) {
        std::vector<size_t> source_face = source_faces[i];
        std::string source_material = source_materials[i];
        if (source_face.size() == 3) {
            source_triangles.push_back(Vector_3<size_t>(
                source_face[0], source_face[1], source_face[2]));
            new_materials.push_back(source_material);
        } else if (source_face.size() > 3) {
            std::vector<std::pair<Point_3, size_t>> face_points;
            face_points.reserve(source_face.size());
            for (size_t j : source_face) {
                face_points.push_back(
                    std::pair<Point_3, size_t>(source_points[j], j));
            }
            Delaunay delaunay =
                Delaunay(face_points.begin(), face_points.end());
            for (Delaunay::Facet facet : delaunay.finite_facets()) {
                std::vector<size_t> source_triangle;
                source_triangle.reserve(3);
                for (int k = 0; k < 4; ++k) {
                    if (k == facet.second) {
                        continue;
                    }
                    source_triangle.push_back(facet.first->vertex(k)->info());
                }
                source_triangles.push_back(
                    Vector_3<size_t>(source_triangle[0], source_triangle[1],
                                     source_triangle[2]));
                new_materials.push_back(source_material);
            }
        }
    }
    source_materials = new_materials;

    std::cout << "Triangulate: " << source_points.size() << " points, "
              << source_triangles.size() << " triangles " << std::endl;

    std::cout << "Building AABB tree from source mesh..." << std::endl;

    std::vector<Triangle_3> triangles;
    triangles.reserve(source_triangles.size());
    for (Vector_3<size_t> source_triangle : source_triangles) {
        triangles.push_back(Triangle_3(source_points[source_triangle.x],
                                       source_points[source_triangle.y],
                                       source_points[source_triangle.z]));
    }
    Tree tree = Tree(triangles.begin(), triangles.end());

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    // Query tree with barycentres of target faces
    start = std::chrono::steady_clock::now();
    std::cout << "Building face barycentres from target mesh..." << std::endl;

    std::vector<Point_3> barycentres;
    barycentres.reserve(target_faces.size());
    for (std::vector<size_t> target_face : target_faces) {
        std::vector<std::pair<Point_3, K::FT>> face_points;
        face_points.reserve(target_face.size());
        for (std::size_t i : target_face) {
            face_points.push_back(
                std::pair<Point_3, K::FT>(target_points[i], 1.0));
        }
        Point_3 barycentre =
            CGAL::barycenter(face_points.begin(), face_points.end());
        barycentres.push_back(barycentre);
    }

    std::cout << "Querying..." << std::endl;

    std::vector<std::string> target_materials;
    target_materials.reserve(target_faces.size());
    for (Point_3 barycentre : barycentres) {
        Tree::Point_and_primitive_id point_and_primitive_id =
            tree.closest_point_and_primitive(barycentre);
        std::size_t i = point_and_primitive_id.second - triangles.cbegin();
        target_materials.push_back(source_materials[i]);
    }

    end = std::chrono::steady_clock::now();
    duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    // Output
    std::vector<Vector_3<double>> out_points;
    out_points.reserve(target_points.size());
    for (Point_3 point : target_points) {
        out_points.push_back(Vector_3(point.x(), point.y(), point.z()));
    }

    if (out.empty()) {
        out = "out.obj";
    }
    std::cout << "Writing to " << out << "..." << std::endl;
    if (!write_obj_vertices_faces_materials_3::
            write_obj_vertices_faces_materials_3(out, out_points, target_faces,
                                                 target_materials)) {
        std::cerr << "Could not write!" << std::endl;
    }

    return 0;
}
