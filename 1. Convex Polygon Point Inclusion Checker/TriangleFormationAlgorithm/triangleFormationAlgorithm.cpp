#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>


typedef cv::Point Point;
typedef cv::Point2d Point2d;
using std::cout;
using std::endl;

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

double crossProduct(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return (destinationVertex.x - sourceVertex.x) * (q.y - sourceVertex.y) -
           (destinationVertex.y - sourceVertex.y) * (q.x - sourceVertex.x);
}

bool isQLeftSide(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return crossProduct(sourceVertex, destinationVertex, q) > 0;
}

Point2d randomPoint(randomDouble(), randomDouble());

bool search(int i, int j) {
    while (j != i + 1) {
        int m = std::floor((i + j) / 2);

        bool isQLeftSideOfIM = isQLeftSide(vertices[i], vertices[m], randomPoint);
        bool isQLeftSideOfMJ = isQLeftSide(vertices[m], vertices[j], randomPoint);
        bool isQLeftSideOfJI = isQLeftSide(vertices[j], vertices[i], randomPoint);

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
    bool isInside = search(0, 29);
    cout << "Random point: " << randomPoint << endl;
    cout << (isInside ? "Point is inside" : "Point is outside") << endl;
    return 0;
}

