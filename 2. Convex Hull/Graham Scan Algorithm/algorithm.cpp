#include <string>
#include <random>
#include <opencv2/opencv.hpp>


typedef cv::Point Point;
typedef cv::Point2d Point2d;
using std::cout;
using std::endl;


int verticesCount = 200;
std::vector<Point2d> vertices;


double randomDouble() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(50.0, 950.0);
    return dis(gen);
}

// Function to rearrange point coordinates based on the axis scale
Point2d rearrangePoint(double x, double y) {
    double newX = x + 90.0;
    double newY = 1090.0 - y;
    return {newX, newY};
}

// Compare function for sorting vertices
bool compareVertices(const Point2d &a, const Point2d &b) {
    if (a.x != b.x)
        return a.x < b.x; // Sort by x-coordinate
    else
        return a.y < b.y; // If x-coordinates are the same, sort by y-coordinate
}

double crossProduct(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return (destinationVertex.x - sourceVertex.x) * (q.y - sourceVertex.y) -
           (destinationVertex.y - sourceVertex.y) * (q.x - sourceVertex.x);
}

bool isQLeftSide(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return crossProduct(sourceVertex, destinationVertex, q) > 0;
}


std::vector<Point2d> search() {
    auto searchConvexHullSide = [](bool useIsQLeftSide) -> std::vector<Point2d> {
        std::vector<Point2d> u = {vertices[0], vertices[1]};
        for (int i = 2; i < verticesCount; ++i) {
            u.emplace_back(vertices[i]);

            size_t j = u.size();
            while (j != 2 && ((useIsQLeftSide && isQLeftSide(u[u.size() - 3], u[u.size() - 2], u[u.size() - 1])) ||
                              (!useIsQLeftSide && !isQLeftSide(u[u.size() - 3], u[u.size() - 2], u[u.size() - 1])))) {
                u.erase(u.begin() + u.size() - 2);
                j = u.size();
            }
        }

        return u;
    };

    std::vector<Point2d> upperSideConvexHull = searchConvexHullSide(true);
    std::vector<Point2d> lowerSideConvexHull = searchConvexHullSide(false);

    // Remove first and last elements of lowerSideConvexHull
    if (!lowerSideConvexHull.empty()) {
        lowerSideConvexHull.erase(lowerSideConvexHull.begin());
        lowerSideConvexHull.pop_back();
    }

    // Append the items of lowerSideConvexHull in reverse order to upperSideConvexHull
    upperSideConvexHull.insert(upperSideConvexHull.end(), lowerSideConvexHull.rbegin(), lowerSideConvexHull.rend());

    return upperSideConvexHull;
}


int main() {
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(randomDouble(), randomDouble());
    }
    std::sort(vertices.begin(), vertices.end(), compareVertices);

    std::vector<Point2d> convexHull = search();

    cout << convexHull << endl;

    std::vector<Point> cvVertices;
    cvVertices.reserve(convexHull.size());
    for (const auto &v: convexHull) {
        Point2d rearrangedVertex = rearrangePoint(v.x, v.y);
        cvVertices.emplace_back(static_cast<int>(rearrangedVertex.x), static_cast<int>(rearrangedVertex.y));
    }

    return 0;
}

