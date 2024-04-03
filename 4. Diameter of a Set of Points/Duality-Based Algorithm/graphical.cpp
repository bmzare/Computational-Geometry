#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace std;

int verticesCount = 10;
bool showVerticesIds = true;

const cv::Scalar RED_COLOR(0, 0, 255);
const cv::Scalar BLACK_COLOR(0, 0, 0);
const cv::Scalar BLUE_COLOR(255, 153, 51);
const cv::Scalar GRAY_COLOR(146, 146, 146);
const cv::Scalar GOLD_COLOR(144, 186, 214);

typedef cv::Point Point;
typedef cv::Point2d Point2d;

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

Point2d rearrangePointAxis(const double x_bias, const double coefficient, const Point2d point) {
    double newX = point.x * coefficient + x_bias;
    double newY = 1090.0 - point.y * coefficient;
    return {newX, newY};
}

Point2d rearrangePointLeftAxis(const Point2d point) {
    return rearrangePointAxis(1090, 1000, point);
}

Point2d rearrangePointRightAxis(const Point2d point) {
    return rearrangePointAxis(3500, 100, point);
}

void generateAxis(const int base, const double coefficient, cv::Mat &image) {
    for (int i = 1; i < 20; ++i) {
        // Draw x-axis with labels
        int x = base + i * 100;
        cv::arrowedLine(image, Point(x, 1090), Point(x, 1075), GRAY_COLOR, 2, cv::LINE_AA, 0,
                        0.1);
        if (i != 10)
            cv::putText(image, to_string(abs(float(i - 10) / 10) * coefficient).substr(0, 4), Point(x - 20, 1120),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        BLACK_COLOR, 1);

        // Draw y-axis with labels
        int y = 2090 - i * 100;
        cv::arrowedLine(image, Point(1000 + base, y), Point(1000 + base + 15, y), GRAY_COLOR, 2,
                        cv::LINE_AA, 0, 0.1);
        if (i != 10)
            cv::putText(image, to_string(abs(float(i - 10) / 10) * coefficient).substr(0, 4),
                        Point(1000 + base - 70, y + 5),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7,
                        BLACK_COLOR, 1);
    }

    // Draw x-axis
    cv::arrowedLine(image, Point(base, 1090), Point(2000 + base, 1090), GRAY_COLOR, 2, cv::LINE_AA, 0,
                    0.02);
    cv::putText(image, "X", Point(2000 + base + 25, 1100), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, BLACK_COLOR,
                2);

    // Draw y-axis
    cv::arrowedLine(image, Point(1000 + base, 2090), Point(1000 + base, 90), GRAY_COLOR, 2, cv::LINE_AA,
                    0, 0.02);
    cv::putText(image, "Y", Point(1000 + base - 10, 55), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, BLACK_COLOR, 2);
}

void drawCircleAndText(cv::Mat &image, const cv::Point &circleCenter, const std::string &text,
                       const cv::Scalar &circleColor, const cv::Scalar &textColor, double textSize,
                       const cv::Point &textOffset = cv::Point(0, 0)) {
    cv::circle(image, circleCenter, 10, circleColor, -1);
    cv::putText(image, text, circleCenter + textOffset, cv::FONT_HERSHEY_DUPLEX, textSize, textColor, 2, cv::LINE_AA);
}

void drawText(cv::Mat &image, const std::string &text, const cv::Point &position,
              const cv::Scalar &textColor, double textSize) {
    cv::putText(image, text, position, cv::FONT_HERSHEY_DUPLEX, textSize, textColor, 2, cv::LINE_AA);
}

GrahamScanResult grahamScan(vector<Vertex> &vertices, cv::Mat &image) {
    auto searchConvexHullSide = [&vertices, &image](bool useIsQLeftSide) -> vector<Vertex> {
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
        for (auto &i: u) {
            cv::circle(image, rearrangePointLeftAxis(i.point), 11, RED_COLOR, -1);
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

double checkDistance(Point2d &p1, Point2d &p2, cv::Mat &image) {
    line(image, rearrangePointLeftAxis(p1),
         rearrangePointLeftAxis(p2), GOLD_COLOR, 2);
    return sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

int main() {
    vector<Vertex> vertices;
    int idCounter = 0;
    cv::Mat image(2500, 4800, CV_8UC3, cv::Scalar(255, 255, 255));
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(Vertex{++idCounter, {randomDouble() / 1000, randomDouble() / 1000}});
    }
    sort(vertices.begin(), vertices.end(), compareVerticesLocation);

    generateAxis(90, 1, image);
    generateAxis(2500, 10, image);

    for (auto &vertex: vertices) {
        Point2d rearrangedVertex = rearrangePointLeftAxis(vertex.point);

        cv::circle(image, rearrangedVertex, 11, BLUE_COLOR, -1);

        if (showVerticesIds) {
            Point2d direction = rearrangedVertex - rearrangePointLeftAxis(Point2d(0, 0));
            double length = cv::norm(direction);
            Point2d textPosition = rearrangedVertex + (50 / length) * direction;

            cv::Size textSize = cv::getTextSize("P" + to_string(vertex.id), cv::FONT_HERSHEY_DUPLEX, 1, 1,
                                                nullptr);
            textPosition -= Point2d(textSize.width / 2, 0);

            drawText(image, "P" + to_string(vertex.id), textPosition, BLACK_COLOR, 1);
        }
    }

    GrahamScanResult convexHull = grahamScan(vertices, image);
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
            cv::circle(image, rearrangePointRightAxis(dualIntersection), 11,
                       cv::Scalar(140, 0, 186), -1);
        }

        Point2d pt1(-10, (-10 * vertex.point.x - vertex.point.y));
        Point2d pt2(10, (10 * vertex.point.x - vertex.point.y));
        line(image, rearrangePointRightAxis(pt1), rearrangePointRightAxis(pt2), cv::Scalar(140, 0, 186), 2);
        if (showVerticesIds) {
            drawText(image, "P" + to_string(vertex.id), rearrangePointRightAxis(pt1) - Point2d(85, 0), BLACK_COLOR,
                     1.1);
            drawText(image, "*", rearrangePointRightAxis(pt1) - Point2d(20, 23), BLACK_COLOR,
                     1.1);
        }
    }
    for (int i = 0; i < convexHull.lowerSide.size(); ++i) {
        Vertex vertex = convexHull.lowerSide[i];
        if (i != convexHull.lowerSide.size() - 1) {
            Vertex nextVertex = convexHull.lowerSide[i + 1];
            Point2d dualIntersection = findDualIntersection(vertex.point, nextVertex.point);

            convexEnvelope.upper.emplace_back(ConvexEnvelopeSpecialPointDetail{nextVertex, true, dualIntersection});
            cv::circle(image, rearrangePointRightAxis(dualIntersection), 11,
                       cv::Scalar(140, 0, 186), -1);
        }

        Point2d pt1(-10, (-10 * vertex.point.x - vertex.point.y));
        Point2d pt2(10, (10 * vertex.point.x - vertex.point.y));
        line(image, rearrangePointRightAxis(pt1), rearrangePointRightAxis(pt2), cv::Scalar(140, 0, 186), 2);
        if (showVerticesIds) {
            drawText(image, "P" + to_string(vertex.id), rearrangePointRightAxis(pt2) + Point2d(20, 0), BLACK_COLOR,
                     1.1);
            drawText(image, "*", rearrangePointRightAxis(pt2) + Point2d(83, -23), BLACK_COLOR,
                     1.1);
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
                                    concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual.point, image);
    DiameterOfPoints diameterOfPoints = {concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual,
                                         concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual, distance};

    for (int i = 2; i < concatenatedEnvelope.size(); ++i) {
        ConvexEnvelopeSpecialPointDetail a = concatenatedEnvelope[i];
        if (a.isInUpperEnvelope) {
            distance = checkDistance(a.vertexDual.point,
                                     concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual.point,
                                     image);
            currentUpperEnvelopeIndex = i;
            if (distance >= diameterOfPoints.distance)
                diameterOfPoints = {a.vertexDual, concatenatedEnvelope[currentLowerEnvelopeIndex].vertexDual, distance};
        } else {
            distance = checkDistance(concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual.point,
                                     a.vertexDual.point,
                                     image);
            currentLowerEnvelopeIndex = i;
            if (distance >= diameterOfPoints.distance)
                diameterOfPoints = {concatenatedEnvelope[currentUpperEnvelopeIndex].vertexDual, a.vertexDual, distance};
        }
    }

    line(image, rearrangePointLeftAxis(diameterOfPoints.upperPoint.point),
         rearrangePointLeftAxis(diameterOfPoints.lowerPoint.point), cv::Scalar(140, 0, 186), 7);
    cv::circle(image, rearrangePointLeftAxis(diameterOfPoints.upperPoint.point), 14, RED_COLOR, -1);
    cv::circle(image, rearrangePointLeftAxis(diameterOfPoints.lowerPoint.point), 14, RED_COLOR, -1);


    drawCircleAndText(image, Point(90 + 800, 2240), "Inside", BLUE_COLOR, BLUE_COLOR, 1.2, Point(20, 10));
    drawCircleAndText(image, Point(90 + 1000, 2240), "OnBoundary", RED_COLOR, RED_COLOR, 1.2, Point(20, 10));

    drawText(image, "Primal Plane", Point(930, 2350), BLACK_COLOR, 1.6);
    drawText(image, "Dual Plane", Point(3360, 2350), BLACK_COLOR, 1.6);
    drawText(image, "Upper Envelope", Point(3320, 650), BLACK_COLOR, 1.4);
    drawText(image, "Lower Envelope", Point(3320, 1550), BLACK_COLOR, 1.4);


    cv::imshow("Diameter of a Set of Points", image);
    cv::waitKey(0);
    return 0;
}
