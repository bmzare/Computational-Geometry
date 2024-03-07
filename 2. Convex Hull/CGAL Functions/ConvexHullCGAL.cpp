#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/property_map.h>
#include <vector>
#include <numeric>
#include <random>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Convex_hull_traits_adapter_2<K,
        CGAL::Pointer_property_map<Point_2>::type> Convex_hull_traits_2;

int verticesCount = 200;
std::vector<Point_2> vertices;

double randomDouble() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1000.0);
    return dis(gen);
}

int main() {
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(randomDouble(), randomDouble());
    }
    std::vector<std::size_t> indices(vertices.size()), out;
    std::iota(indices.begin(), indices.end(), 0);
    CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
                        Convex_hull_traits_2(CGAL::make_property_map(vertices)));
    for (std::size_t i: out) {
        std::cout << "vertices[" << i << "] = " << vertices[i] << std::endl;
    }
    return 0;
}