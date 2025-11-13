#include <chrono>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/polygon_soup_io.h>
#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Triangulation_3.h>
#include <CGAL/barycenter.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_3 = K::Point_3;
using Point_3_i = boost::tuple<Point_3, size_t>;
using Traits =
    CGAL::Search_traits_adapter<Point_3_i,
                                CGAL::Nth_of_tuple_property_map<0, Point_3_i>,
                                CGAL::Search_traits_3<K>>;
using Orthogonal_k_neighbor_search = CGAL::Orthogonal_k_neighbor_search<Traits>;
using Tree = Orthogonal_k_neighbor_search::Tree;
using Triangulation_3 = CGAL::Triangulation_3<K>;

int main(int argc, char **argv) {

    // IO

    std::string source = argv[1];
    std::string target = argv[2];
    std::string material = "";

    static struct option long_options[] = {
        {"material", required_argument, 0, 'm'}, {0, 0, 0, 0}};
    optind = 3;
    int option_index = 0;
    for (int i = 0; i < 1000; ++i) {
        int opt = getopt_long(argc, argv, "m:", long_options, &option_index);
        if (opt == -1) {
            break;
        }
        switch (opt) {
        case 'm': {
            material = optarg;
        }
        default: {
            std::cerr << "?? getopt returned character code " << opt << "??"
                      << std::endl;
        }
        }
    }

    // std::vector<std::string/struct> of materials corresponding to
    // source_faces
    std::vector<Point_3> source_points;
    std::vector<std::vector<std::size_t>> source_faces;
    if (!CGAL::IO::read_polygon_soup(source, source_points, source_faces) ||
        source_faces.empty()) {
        std::cout << "Invalid source input: " << source << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << source << ": " << source_points.size() << " points, "
              << source_faces.size() << " faces " << std::endl;

    std::vector<Point_3> target_points;
    std::vector<std::vector<std::size_t>> target_faces;
    if (!CGAL::IO::read_polygon_soup(target, target_points, target_faces) ||
        source_faces.empty()) {
        std::cout << "Invalid target input: " << target << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << target << ": " << target_points.size() << " points, "
              << target_faces.size() << " faces " << std::endl;

    // Source mesh preprocessing

    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    std::cout << "Building face barycentres and kd tree of barycentres from "
                 "source mesh..."
              << std::endl;

    std::vector<Point_3_i> barycentres;
    size_t i = 0;
    for (std::vector<std::size_t> source_face : source_faces) {
        std::vector<std::pair<Point_3, K::FT>> face_points;
        face_points.reserve(source_face.size());
        for (std::size_t i : source_face) {
            std::pair<Point_3, K::FT> weighted_point =
                std::pair<Point_3, K::FT>(source_points[i], 1.0);
            face_points.push_back(weighted_point);
        }
        Point_3 barycentre =
            CGAL::barycenter(face_points.begin(), face_points.end());
        barycentres.push_back(Point_3_i(barycentre, i));
        i++;
    }

    Tree tree = Tree(barycentres.begin(), barycentres.end());

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Took: " << duration.count() << " s" << std::endl;

    // Query source mesh with target mesh faces

    for (std::vector<std::size_t> target_face : target_faces) {
        std::vector<Point_3> face_points;
        face_points.reserve(target_face.size());
        for (std::size_t i : target_face) {
            face_points.push_back(target_points[i]);
        }
        if (face_points.size() == 3) {
            K::Triangle_3 face_triangle =
                K::Triangle_3(face_points[0], face_points[1], face_points[2]);
            Orthogonal_k_neighbor_search search =
                Orthogonal_k_neighbor_search(tree, face_triangle, 1);
        } else if (face_points.size() > 3) {
            std::vector<K::Tetrahedron_3> face_tetrahedrons;
            Triangulation_3 triangulation =
                Triangulation_3(face_points.begin(), face_points.end());
            for (Triangulation_3::Finite_cells_iterator ic =
                     triangulation.finite_cells_begin();
                 ic != triangulation.finite_cells_end(); ++ic) {
                Point_3 p0 = ic->vertex(0)->point();
                Point_3 p1 = ic->vertex(1)->point();
                Point_3 p2 = ic->vertex(2)->point();
                Point_3 p3 = ic->vertex(3)->point();
                K::Tetrahedron_3 face_tetrahedron =
                    K::Tetrahedron_3(p0, p1, p2, p3);
                face_tetrahedrons.push_back(face_tetrahedron);
            }
            // query for each face_tetrahedron
        }
    }
}
