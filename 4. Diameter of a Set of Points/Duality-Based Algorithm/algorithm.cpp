#include <random>
#include <cmath>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

typedef cv::Point Point;
typedef cv::Point2d Point2d;

int verticesCount = 10;

struct Vertex {
    int id;
    Point2d point;
};
struct GrahamScanResult {
    vector<Vertex> upperSide;
    vector<Vertex> lowerSide;
};
struct ConvexEnvelopeSpecialPointDetail {
    Vertex vertexDual;
    bool isInUpperEnvelope;
    Point2d intersectionPoint;
};
struct ConvexEnvelope {
    vector<ConvexEnvelopeSpecialPointDetail> upper;
    vector<ConvexEnvelopeSpecialPointDetail> lower;
};

double randomDouble() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-950.0, 950.0);
    return dis(gen);
}

bool compareVerticesLocation(const Vertex &a, const Vertex &b) {
    if (a.point.x != b.point.x)
        return a.point.x < b.point.x;
    else
        return a.point.y < b.point.y;
}

bool compareIntersections(const ConvexEnvelopeSpecialPointDetail &a, const ConvexEnvelopeSpecialPointDetail &b) {
    if (a.intersectionPoint.x != b.intersectionPoint.x)
        return a.intersectionPoint.x < b.intersectionPoint.x;
    else
        return a.intersectionPoint.y < b.intersectionPoint.y;
}

vector<Vertex> reverseVector(vector<Vertex> &vec) {
    vector<Vertex> reversed;
    reverse_copy(vec.begin(), vec.end(), back_inserter(reversed));
    return reversed;
}

double crossProduct(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return (destinationVertex.x - sourceVertex.x) * (q.y - sourceVertex.y) -
           (destinationVertex.y - sourceVertex.y) * (q.x - sourceVertex.x);
}

bool isQLeftSide(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return crossProduct(sourceVertex, destinationVertex, q) >= 0;
}

GrahamScanResult grahamScan(vector<Vertex> &vertices) {
    auto searchConvexHullSide = [&vertices](bool useIsQLeftSide) -> vector<Vertex> {
        vector<Vertex> u = {vertices[0], vertices[1]};
        for (int i = 2; i < verticesCount; ++i) {
            u.emplace_back(vertices[i]);

            size_t j = u.size();
            while (j != 2 && ((useIsQLeftSide &&
                               isQLeftSide(u[u.size() - 3].point, u[u.size() - 2].point, u[u.size() - 1].point)) ||
                              (!useIsQLeftSide &&
                               !isQLeftSide(u[u.size() - 3].point, u[u.size() - 2].point, u[u.size() - 1].point)))) {
                u.erase(u.begin() + u.size() - 2);
                j = u.size();
            }
        }
        return u;
    };

    vector<Vertex> upperSideConvexHull = searchConvexHullSide(true);
    vector<Vertex> lowerSideConvexHull = searchConvexHullSide(false);

    return GrahamScanResult{upperSideConvexHull, lowerSideConvexHull};
}

Point2d findDualIntersection(Point2d &p1, Point2d &p2) {
    double x = (p1.y - p2.y) / (p1.x - p2.x);
    double y = p1.x * x - p1.y;

    return {x, y};
}

double checkDistance(Point2d &p1, Point2d &p2) {
    return sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

int main() {
    vector<Vertex> vertices;
    int idCounter = 0;
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(Vertex{++idCounter, {randomDouble() / 1000, randomDouble() / 1000}});
    }
    sort(vertices.begin(), vertices.end(), compareVerticesLocation);

    GrahamScanResult convexHull = grahamScan(vertices);
    convexHull.upperSide = reverseVector(
            convexHull.upperSide); // The lower envelope in the dual plane is the reverse of the upper convex hull in the primal plane (from left to right).
    ConvexEnvelope convexEnvelope = {{{convexHull.lowerSide[0], true,  Point2d(-1000001, 0)}},
                                     {{convexHull.upperSide[0], false, Point2d(-1000000, 0)}}};

    for (int i = 0; i < convexHull.upperSide.size(); ++i) {
        Vertex vertex = convexHull.upperSide[i];
        if (i != convexHull.upperSide.size() - 1) {
            Vertex nextVertex = convexHull.upperSide[i + 1];
            Point2d dualIntersection = findDualIntersection(vertex.point, nextVertex.point);

            convexEnvelope.lower.emplace_back(ConvexEnvelopeSpecialPointDetail{nextVertex, false, dualIntersection});
        }
    }
    for (int i = 0; i < convexHull.lowerSide.size(); ++i) {
        Vertex vertex = convexHull.lowerSide[i];
        if (i != convexHull.lowerSide.size() - 1) {
            Vertex nextVertex = convexHull.lowerSide[i + 1];
            Point2d dualIntersection = findDualIntersection(vertex.point, nextVertex.point);

            convexEnvelope.upper.emplace_back(ConvexEnvelopeSpecialPointDetail{nextVertex, true, dualIntersection});
        }
    }

    vector<ConvexEnvelopeSpecialPointDetail> concatenatedEnvelope(convexEnvelope.upper.begin(),
                                                                  convexEnvelope.upper.end());
    concatenatedEnvelope.insert(concatenatedEnvelope.end(), convexEnvelope.lower.begin(), convexEnvelope.lower.end());
    sort(concatenatedEnvelope.begin(), concatenatedEnvelope.end(), compareIntersections);

    struct DiameterOfPoints {
        Vertex upperPoint;
        Vertex lowerPoint;
        double distance{};
    };
    int currentUpperEnvelopeIndex = 0;
    int currentLowerEnvelopeIndex = 1;
    double distance = checkDistance(concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual.point,
                                    concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual.point);
    DiameterOfPoints diameterOfPoints = {concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual,
                                         concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual, distance};

    for (int i = 2; i < concatenatedEnvelope.size(); ++i) {
        ConvexEnvelopeSpecialPointDetail a = concatenatedEnvelope[i];
        if (a.isInUpperEnvelope) {
            distance = checkDistance(a.vertexDual.point,
                                     concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual.point);
            currentUpperEnvelopeIndex = i;
            if (distance >= diameterOfPoints.distance)
                diameterOfPoints = {a.vertexDual, concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual, distance};
        } else {
            distance = checkDistance(concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual.point,
                                     a.vertexDual.point);
            currentLowerEnvelopeIndex = i;
            if (distance >= diameterOfPoints.distance)
                diameterOfPoints = {concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual, a.vertexDual, distance};
        }
    }

    cout << "Random vertices (id : location) " << endl;
    for (auto &vertex: vertices) {
        cout << vertex.id << " : " << vertex.point << endl;
    }
    cout << "Diameter Size : " << diameterOfPoints.distance << endl;
    cout << "Diameter Endpoints ID : " << diameterOfPoints.upperPoint.id << " - " << diameterOfPoints.lowerPoint.id
         << endl;


    return 0;
}
