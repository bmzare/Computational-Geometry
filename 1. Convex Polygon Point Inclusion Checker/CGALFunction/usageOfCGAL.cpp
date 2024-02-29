#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <iostream>
#include <random>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;
using std::cout;
using std::endl;


Point points[] = {Point(377.6745149557625, 942.1808268605445),
                  Point(238.83733879149833, 900.4243819908085),
                  Point(218.66640166535052, 888.5758770260142),
                  Point(174.48888743765974, 838.7119857935086),
                  Point(159.66761638642764, 806.3739124852123),
                  Point(124.2987042098846, 712.0666450628357),
                  Point(66.9390291444691, 500.06418511498896),
                  Point(63.639573844822506, 453.2099502721152),
                  Point(80.88133716460145, 338.0474041904067),
                  Point(98.1231004843804, 222.88485810869818),
                  Point(118.73772780999876, 140.88099451272916),
                  Point(144.78163285605223, 122.37553169147563),
                  Point(255.23423336143497, 61.63321069053151),
                  Point(371.87287282469026, 46.67324673408757),
                  Point(550.8791145068759, 52.96702383241181),
                  Point(607.7391888594088, 55.77680802943807),
                  Point(645.8902225985598, 77.05017659857782),
                  Point(669.9320273165234, 96.97960782750337),
                  Point(707.8958761317195, 130.81298874449664),
                  Point(773.7141161119062, 206.65814379529147),
                  Point(849.594393786748, 305.57938061799763),
                  Point(886.7754594317931, 362.59176879380584),
                  Point(916.5822906301806, 446.31002645411047),
                  Point(936.3101027179695, 560.2425276965004),
                  Point(933.2665887900832, 591.4378839076402),
                  Point(926.366450511541, 620.6850020908228),
                  Point(801.9767157801314, 750.56104334207),
                  Point(766.0913019184976, 786.3504664267142),
                  Point(660.4457443602121, 879.7537174908357),
                  Point(459.4553422335778, 928.5440520401731)};

double randomDouble() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1000.0);
    return dis(gen);
}

void search(Point pt, Point *pgn_begin, Point *pgn_end, K traits) {
    cout << "The point " << pt;
    switch (CGAL::bounded_side_2(pgn_begin, pgn_end, pt, traits)) {
        case CGAL::ON_BOUNDED_SIDE :
            cout << " is inside the polygon.\n";
            break;
        case CGAL::ON_BOUNDARY:
            cout << " is on the polygon boundary.\n";
            break;
        case CGAL::ON_UNBOUNDED_SIDE:
            cout << " is outside the polygon.\n";
            break;
    }
}

int main() {
    Point randomPoint = Point(randomDouble(), randomDouble());
    std::cout << "Random point" << std::endl;
    search(randomPoint, points, points + 30, K());
    return 0;
}