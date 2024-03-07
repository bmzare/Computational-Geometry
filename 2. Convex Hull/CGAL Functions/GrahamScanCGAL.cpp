#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/ch_graham_andrew.h>
#include <random>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;

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
    std::vector<Point_2> convex_hull;

    CGAL::ch_graham_andrew(vertices.begin(), vertices.end(), std::back_inserter(convex_hull));

    std::cout << "Convex Hull Vertices:" << std::endl;
    for (const auto &p: convex_hull) {
        std::cout << p << std::endl;
    }

    return 0;
}
