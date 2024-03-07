#include <string>
#include <random>
#include <opencv2/opencv.hpp>


typedef cv::Point Point;
typedef cv::Point2d Point2d;
using std::cout;
using std::endl;


int verticesCount = 30;
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

std::vector<Point2d> giftWrappingSearch(cv::Mat &image) {
    std::vector<Point2d> convexHull;

    if (vertices.size() < 3) return convexHull;

    Point2d pointOnHull = findLeftmostPoint(vertices);
    cv::circle(image, rearrangePoint(pointOnHull.x, pointOnHull.y), 20, cv::Scalar(0, 0, 256), -1);
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

    for (auto &j: convexHull) {
        cv::circle(image, rearrangePoint(j.x, j.y), 9, cv::Scalar(0, 0, 256), -1);
    }

    return convexHull;
}


int main() {
    cv::Mat image(1250, 1200, CV_8UC3, cv::Scalar(255, 255, 255));
    vertices.reserve(verticesCount);
    for (int i = 0; i < verticesCount; ++i) {
        vertices.emplace_back(randomDouble(), randomDouble());
    }

    for (int i = 0; i < 10; ++i) {
        // Draw x-axis with labels
        int x = i * 100 + 90;
        cv::arrowedLine(image, Point(x, 1090), Point(x, 1075), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0,
                        0.1);
        cv::putText(image, std::to_string(i * 100), Point(x - 20, 1120), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(0, 0, 0), 1);

        // Draw y-axis with labels
        int y = 1090 - i * 100;
        cv::arrowedLine(image, Point(90, y), Point(105, y), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0, 0.1);
        cv::putText(image, std::to_string(i * 100), Point(40, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(0, 0, 0), 1);
    }

    // Draw x-axis
    cv::arrowedLine(image, Point(90, 1090), Point(1090, 1090), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0,
                    0.02);
    cv::putText(image, "X", Point(1115, 1100), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, cv::Scalar(0, 0, 0), 2);

    // Draw y-axis
    cv::arrowedLine(image, Point(90, 1090), Point(90, 90), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0, 0.02);
    cv::putText(image, "Y", Point(80, 55), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, cv::Scalar(0, 0, 0), 2);

    // Description
    cv::circle(image, Point(380, 1190), 9, cv::Scalar(255, 153, 51), -1);
    cv::putText(image, "Inside", Point(400, 1200), cv::FONT_HERSHEY_DUPLEX, 1.1, cv::Scalar(255, 153, 51), 2,
                cv::LINE_AA);

    cv::circle(image, Point(580, 1190), 9, cv::Scalar(0, 0, 255), -1);
    cv::putText(image, "OnBoundary", Point(600, 1200), cv::FONT_HERSHEY_DUPLEX, 1.1, cv::Scalar(0, 0, 255), 2,
                cv::LINE_AA);


    // Calculate the center of all vertices (Styling)
    Point2d center(0, 0);
    for (const auto &vertex: vertices) {
        center += vertex;
    }
    center *= 1.0 / static_cast<double>(vertices.size());

    // Draw vertices and lines between them
    for (size_t i = 0; i < vertices.size(); ++i) {
        Point2d rearrangedVertex = rearrangePoint(vertices[i].x, vertices[i].y);

        cv::circle(image, rearrangedVertex, 9, cv::Scalar(255, 153, 51), -1);

        // Calculate the direction vector from the center to the current vertex (Styling)
        Point2d direction = rearrangedVertex - center;
        double length = cv::norm(direction);
        Point2d textPosition = rearrangedVertex + (40 / length) * direction;

        // Offset the textPosition by the length of the ID string (e.g., "P12") divided by 2
        // to center the ID text around the calculated position
        cv::Size textSize = cv::getTextSize(std::to_string(i + 1), cv::FONT_HERSHEY_DUPLEX, 0.7, 1, nullptr);
        textPosition -= Point2d(textSize.width / 2, 0);

        // Draw the vertex ID
        cv::putText(image, std::to_string(i + 1), textPosition, cv::FONT_HERSHEY_DUPLEX, 0.9, cv::Scalar(0, 0, 0),
                    2, cv::LINE_AA);
    }

    std::vector<Point2d> convexHull = giftWrappingSearch(image);

    cout << convexHull.size() << endl;

    std::vector<Point> cvVertices;
    cvVertices.reserve(convexHull.size());
    for (const auto &v: convexHull) {
        Point2d rearrangedVertex = rearrangePoint(v.x, v.y);
        cvVertices.emplace_back(static_cast<int>(rearrangedVertex.x), static_cast<int>(rearrangedVertex.y));
    }

    cv::polylines(image, cvVertices, true, cv::Scalar(0, 0, 256), 2, cv::LINE_AA);


    cv::imshow("Convex Hull, Gift-Wrapping (Jarvis March) algorithm", image);
    cv::waitKey(0);
    return 0;
}

