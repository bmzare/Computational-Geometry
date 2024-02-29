#include <string>
#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>


typedef cv::Point Point;
typedef cv::Point2d Point2d;

int randomPointsCount = 50;
std::vector<Point2d> randomPoints;

// Vertices
std::vector<Point2d> vertices = {Point2d(377.6745149557625, 942.1808268605445),
                                 Point2d(238.83733879149833, 900.4243819908085),
                                 Point2d(218.66640166535052, 888.5758770260142),
                                 Point2d(174.48888743765974, 838.7119857935086),
                                 Point2d(159.66761638642764, 806.3739124852123),
                                 Point2d(124.2987042098846, 712.0666450628357),
                                 Point2d(66.9390291444691, 500.06418511498896),
                                 Point2d(63.639573844822506, 453.2099502721152),
                                 Point2d(80.88133716460145, 338.0474041904067),
                                 Point2d(98.1231004843804, 222.88485810869818),
                                 Point2d(118.73772780999876, 140.88099451272916),
                                 Point2d(144.78163285605223, 122.37553169147563),
                                 Point2d(255.23423336143497, 61.63321069053151),
                                 Point2d(371.87287282469026, 46.67324673408757),
                                 Point2d(550.8791145068759, 52.96702383241181),
                                 Point2d(607.7391888594088, 55.77680802943807),
                                 Point2d(645.8902225985598, 77.05017659857782),
                                 Point2d(669.9320273165234, 96.97960782750337),
                                 Point2d(707.8958761317195, 130.81298874449664),
                                 Point2d(773.7141161119062, 206.65814379529147),
                                 Point2d(849.594393786748, 305.57938061799763),
                                 Point2d(886.7754594317931, 362.59176879380584),
                                 Point2d(916.5822906301806, 446.31002645411047),
                                 Point2d(936.3101027179695, 560.2425276965004),
                                 Point2d(933.2665887900832, 591.4378839076402),
                                 Point2d(926.366450511541, 620.6850020908228),
                                 Point2d(801.9767157801314, 750.56104334207),
                                 Point2d(766.0913019184976, 786.3504664267142),
                                 Point2d(660.4457443602121, 879.7537174908357),
                                 Point2d(459.4553422335778, 928.5440520401731)};


double randomDouble() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1000.0);
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

bool search(int i, int j, int randomPointIndex) {
    while (j != i + 1) {
        int m = std::floor((i + j) / 2);

        bool isQLeftSideOfIM = isQLeftSide(vertices[i], vertices[m], randomPoints[randomPointIndex]);
        bool isQLeftSideOfMJ = isQLeftSide(vertices[m], vertices[j], randomPoints[randomPointIndex]);
        bool isQLeftSideOfJI = isQLeftSide(vertices[j], vertices[i], randomPoints[randomPointIndex]);

        if (isQLeftSideOfIM && isQLeftSideOfMJ && isQLeftSideOfJI) {
            return true;
        } else if (!isQLeftSideOfIM && !isQLeftSideOfMJ) {
            return false;
        } else if (!isQLeftSideOfIM) {
            j = m;
        } else {
            i = m;
        }
    }
    return false;
}


int main() {
    cv::Mat image(1250, 1200, CV_8UC3, cv::Scalar(255, 255, 255));
    randomPoints.reserve(randomPointsCount);
    for (int i = 0; i < randomPointsCount; ++i) {
        randomPoints.emplace_back(randomDouble(), randomDouble());
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
    cv::circle(image, Point(420, 1190), 9, cv::Scalar(0, 200, 0), -1);
    cv::putText(image, "Inside", Point(440, 1200), cv::FONT_HERSHEY_DUPLEX, 1.1, cv::Scalar(0, 200, 0), 2,
                cv::LINE_AA);

    cv::circle(image, Point(620, 1190), 9, cv::Scalar(0, 0, 255), -1);
    cv::putText(image, "Outside", Point(640, 1200), cv::FONT_HERSHEY_DUPLEX, 1.1, cv::Scalar(0, 0, 255), 2,
                cv::LINE_AA);


    // Calculate the center of all vertices (Styling)
    Point2d center(0, 0);
    for (const auto &vertex: vertices) {
        center += vertex;
    }
    center *= 1.0 / static_cast<double>(vertices.size());

    // Draw vertices and lines between them
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::vector<Point> cvVertices;
        cvVertices.reserve(vertices.size());
        for (const auto &v: vertices) {
            Point2d rearrangedVertex = rearrangePoint(v.x, v.y);
            cvVertices.emplace_back(static_cast<int>(rearrangedVertex.x), static_cast<int>(rearrangedVertex.y));
        }

        cv::polylines(image, cvVertices, true, cv::Scalar(255, 153, 51), 2, cv::LINE_AA);

        Point2d rearrangedVertex = rearrangePoint(vertices[i].x, vertices[i].y);

        cv::circle(image, rearrangedVertex, 9, cv::Scalar(255, 153, 51), -1);

        // Calculate the direction vector from the center to the current vertex (Styling)
        Point2d direction = rearrangedVertex - center;
        double length = cv::norm(direction);
        Point2d textPosition = rearrangedVertex + (40 / length) * direction;

        // Offset the textPosition by the length of the ID string (e.g., "P12") divided by 2
        // to center the ID text around the calculated position
        cv::Size textSize = cv::getTextSize("P" + std::to_string(i + 1), cv::FONT_HERSHEY_DUPLEX, 0.7, 1, nullptr);
        textPosition -= Point2d(textSize.width / 2, 0);

        // Draw the vertex ID
        cv::putText(image, "P" + std::to_string(i + 1), textPosition, cv::FONT_HERSHEY_DUPLEX, 0.9, cv::Scalar(0, 0, 0),
                    2, cv::LINE_AA);
    }

    // Draw the random vertices
    for (int i = 0; i < randomPoints.size(); ++i) {
        cv::circle(image, rearrangePoint(randomPoints[i].x, randomPoints[i].y), 6, cv::Scalar(17, 190, 233), -1);

        bool isInside = search(0, 29, i);

        cv::Scalar qColor = isInside ? cv::Scalar(0, 200, 0) : cv::Scalar(0, 0, 255);
        cv::circle(image, rearrangePoint(randomPoints[i].x, randomPoints[i].y), 6, qColor, -1);
    }

    cv::imshow("Convex Polygon Points Inclusion Checker", image);
    cv::waitKey(0);
    return 0;
}

