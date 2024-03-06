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

double crossProduct(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return (destinationVertex.x - sourceVertex.x) * (q.y - sourceVertex.y) -
           (destinationVertex.y - sourceVertex.y) * (q.x - sourceVertex.x);
}

bool isQLeftSide(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return crossProduct(sourceVertex, destinationVertex, q) > 0;
}

Point2d findLeftmostPoint(const std::vector<Point2d> &points) {
    Point2d leftmost = points[0];
    for (const auto &point: points) {
        if (point.x < leftmost.x)
            leftmost = point;
    }
    return leftmost;
}

std::vector<Point2d> giftWrappingSearch() {
    std::vector<Point2d> convexHull;

    if (vertices.size() < 3) return convexHull;

    Point2d pointOnHull = findLeftmostPoint(vertices);
    Point2d endpoint;
    int i = 0;
    do {
        convexHull.push_back(pointOnHull);
        endpoint = vertices[0];
        for (size_t j = 1; j < vertices.size(); ++j) {
            if (endpoint == pointOnHull || isQLeftSide(convexHull[i], endpoint, vertices[j]))
                endpoint = vertices[j];
        }
        pointOnHull = endpoint;
        ++i;
    } while (endpoint != convexHull[0]);

    return convexHull;
}


int main() {
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(randomDouble(), randomDouble());
    }

    std::vector<Point2d> convexHull = giftWrappingSearch();

    cout << convexHull << endl;

    return 0;
}

