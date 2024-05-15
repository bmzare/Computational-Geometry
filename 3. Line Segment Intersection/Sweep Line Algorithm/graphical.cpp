#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <opencv2/opencv.hpp>
#include <utility>


using namespace std;

typedef cv::Point Point;
typedef cv::Point2d Point2d;

int randomLineSegmentsCount = 50;


double random_double(double s, double e) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dis(s, e);
    return dis(gen);
}

Point2d rearrange_point(Point2d point) {
    double newX = point.x + 90.0;
    double newY = 1090.0 - point.y;
    return {newX, newY};
}

enum EventType {
    START_SEGMENT,
    END_SEGMENT,
    INTERSECTION,
};

struct Segment {
    Point2d start;
    Point2d end;
    int id;
};

struct Status {
    Point2d start;
    Point2d end;
    int id;
    Point2d current_location;
};

struct Event {
    Point2d point;
    EventType type;
    Segment segment;
    Segment segment2;
};

double crossProduct(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return (destinationVertex.x - sourceVertex.x) * (q.y - sourceVertex.y) -
           (destinationVertex.y - sourceVertex.y) * (q.x - sourceVertex.x);
}

bool isQLeftSide(const Point2d &sourceVertex, const Point2d &destinationVertex, const Point2d &q) {
    return crossProduct(sourceVertex, destinationVertex, q) > 0;
}

struct EventCompare {
    bool operator()(const Event &e1, const Event &e2) const {
        bool fC = e1.point.y > e2.point.y;
        bool fC2 = e1.point.y == e2.point.y;

        bool sC = e1.point.x < e2.point.x;
        bool sC2 = e1.point.x == e2.point.x;

        bool tC = isQLeftSide(e1.point, e1.segment.end, e2.segment.end);
        bool tC2 = isQLeftSide(e1.point, e1.segment.end, e2.segment.end) ==
                   isQLeftSide(e2.point, e1.segment.end, e2.segment.end);

        bool foC = e1.segment.id < e2.segment.id;

        return fC || (fC2 && sC) || (fC2 && sC2 && tC) || (fC2 && sC2 && tC2 && foC);
    }
};

struct StatusCompare {
    bool operator()(const Status &e1, const Status &e2) const {
        bool fC = e1.current_location.x < e2.current_location.x;
        bool fC2 = e1.current_location.x == e2.current_location.x;

        bool sC = isQLeftSide(e1.current_location, e1.end, e2.end);

        return fC || (fC2 && sC);
    }
};

set<Event, EventCompare> event_queue;
set<Status, StatusCompare> status_queue;
vector<pair<std::vector<int>, Point2d>> intersection_points;
size_t compared_number = 0;

bool checkIntersection(const Segment &s1, const Segment &s2) {
    compared_number++;
    double det1 = (s2.end - s2.start).cross(s1.start - s2.start);
    double det2 = (s2.end - s2.start).cross(s1.end - s2.start);
    double det3 = (s1.end - s1.start).cross(s2.start - s1.start);
    double det4 = (s1.end - s1.start).cross(s2.end - s1.start);

    return (det1 * det2 < 0) && (det3 * det4 < 0);
}

Point2d getIntersectionPoint(const Segment &s1, const Segment &s2) {
    double t = ((s2.start - s1.start).cross(s2.end - s2.start)) / ((s1.end - s1.start).cross(s2.end - s2.start));
    return s1.start + t * (s1.end - s1.start);
}

void initialEvent(const vector<Segment> &segments) {
    for (const auto &segment: segments) {
        Event start_event = {segment.start, START_SEGMENT, segment};
        Event end_event = {segment.end, END_SEGMENT, segment};

        event_queue.insert(start_event);
        event_queue.insert(end_event);
    }
}

bool sCompare(const Status &a, const Status &b) {
    return isQLeftSide(a.current_location, a.end, b.end);
}

void findNewEvent(Segment s1, Segment s2, Point2d p) {
    if (checkIntersection(s1, s2)) {
        Point2d intersect_point = getIntersectionPoint(s1, s2);
        if (intersect_point.y < p.y || intersect_point.y == p.y && intersect_point.x > p.x) {
            event_queue.insert({intersect_point, INTERSECTION, s1, s2});
            intersection_points.push_back({{s1.id, s2.id}, intersect_point});
        }
    }
}

void handleEventPoint(const vector<Event> &current_events) {
    vector<Status> U_p, L_p, C_p;
    vector<int> segment_ids;
    vector<int> u_ids, l_ids, c_ids;
    Point2d p = current_events[0].point;

    for (auto &e: current_events) {
        switch (e.type) {
            case START_SEGMENT:
                U_p.push_back({e.segment.start, e.segment.end, e.segment.id, p});
                segment_ids.push_back(e.segment.id);
                u_ids.push_back(e.segment.id);
                break;
            case END_SEGMENT:
                L_p.push_back({e.segment.start, e.segment.end, e.segment.id, p});
                segment_ids.push_back(e.segment.id);
                l_ids.push_back(e.segment.id);
                break;
            case INTERSECTION:
                C_p.push_back({e.segment.start, e.segment.end, e.segment.id, p});
                C_p.push_back({e.segment2.start, e.segment2.end, e.segment2.id, p});
                segment_ids.push_back(e.segment.id);
                c_ids.push_back(e.segment.id);
                segment_ids.push_back(e.segment2.id);
                c_ids.push_back(e.segment2.id);
                break;
        }
    }

    vector<Status> oldStatuses;
    vector<Status> oldStatuses2;
    for (auto it = status_queue.begin(); it != status_queue.end(); ++it) {
        const Status &status = *it;
        auto founded = find(segment_ids.begin(), segment_ids.end(), status.id);
        if (founded != segment_ids.end()) {
            oldStatuses.push_back(status);
        } else {
            oldStatuses2.push_back(status);
        }
    }

    for (auto &e: oldStatuses) {
        status_queue.erase(e);
    }
    for (auto &e: U_p) {
        status_queue.insert(e);
    }
    for (auto &e: C_p) {
        status_queue.insert(e);
    }

    for (auto &e: oldStatuses2) {
        status_queue.erase(e);
    }
    for (auto &e: oldStatuses2) {
        double x = e.start.x + (e.end.x - e.start.x) * (p.y - e.start.y) / (e.end.y - e.start.y);
        status_queue.insert({e.start, e.end, e.id,
                             {x, p.y}});
        if (x == p.x) segment_ids.push_back(e.id);
    }

    if (U_p.size() + L_p.size() + C_p.size() > 1) {
        intersection_points.push_back({segment_ids, p});
    }

    if (U_p.size() + C_p.size() == 0) {
        if (status_queue.lower_bound(L_p[0]) != status_queue.begin() &&
            status_queue.upper_bound(L_p[L_p.size() - 1]) != status_queue.end()) {
            Status _pred = *(--status_queue.lower_bound(L_p[0]));
            Status _suc = *(status_queue.upper_bound(L_p[L_p.size() - 1]));

            Segment predecessor = {_pred.start, _pred.end, _pred.id};

            Segment successor = {_suc.start, _suc.end, _suc.id};

            findNewEvent(predecessor, successor, p);
        }
    } else {
        vector<Status> sortedUAndC;
        sortedUAndC.insert(sortedUAndC.end(), U_p.begin(), U_p.end());
        sortedUAndC.insert(sortedUAndC.end(), C_p.begin(), C_p.end());
        std::sort(sortedUAndC.begin(), sortedUAndC.end(), sCompare);

        Status left_most = sortedUAndC[0];
        Status right_most = sortedUAndC[sortedUAndC.size() - 1];


        if (status_queue.lower_bound(left_most) != status_queue.begin()) {
            Status _pred = *(--status_queue.lower_bound(left_most));
            Segment predecessor = {_pred.start, _pred.end, _pred.id};
            Segment left_most_segment = {left_most.start, left_most.end, left_most.id};
            findNewEvent(predecessor, left_most_segment, p);
        }
        if (status_queue.upper_bound(right_most) != status_queue.end()) {
            Status _suc = *(status_queue.upper_bound(right_most));
            Segment successor = {_suc.start, _suc.end, _suc.id};
            Segment right_most_segment = {right_most.start, right_most.end, right_most.id};
            findNewEvent(successor, right_most_segment, p);
        }
    }
}

void findIntersections(const vector<Segment> &segments) {
    initialEvent(segments);
    while (!event_queue.empty()) {
        Event current_event = *event_queue.begin();
        event_queue.erase(event_queue.begin());

        vector<Event> current_events = {current_event};

        while (event_queue.begin()->point.x == current_event.point.x &&
               event_queue.begin()->point.y == current_event.point.y) {
            Event e = *event_queue.begin();
            event_queue.erase(event_queue.begin());
            current_events.push_back(e);
        }
        handleEventPoint(current_events);
    }
}

bool compareIntersectionPoints(const std::pair<std::vector<int>, Point2d> &a, const std::pair<std::vector<int>, Point2d> &b) {
    return a.second.x < b.second.x || (a.second.x == b.second.x && a.second.y < b.second.y);
}

int main() {
    cv::Mat image(1250, 1200, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = 0; i < 10; ++i) {
        // Draw x-axis with labels
        int x = i * 100 + 90;
        cv::arrowedLine(image, Point(x, 1090), Point(x, 1075), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0,
                        0.1);
        cv::putText(image, to_string(i * 100), Point(x - 20, 1120), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(0, 0, 0), 1);

        // Draw y-axis with labels
        int y = 1090 - i * 100;
        cv::arrowedLine(image, Point(90, y), Point(105, y), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0, 0.1);
        cv::putText(image, to_string(i * 100), Point(40, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.7,
                    cv::Scalar(0, 0, 0), 1);
    }

    // Draw x-axis
    cv::arrowedLine(image, Point(90, 1090), Point(1090, 1090), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0,
                    0.02);
    cv::putText(image, "X", Point(1115, 1100), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, cv::Scalar(0, 0, 0), 2);

    // Draw y-axis
    cv::arrowedLine(image, Point(90, 1090), Point(90, 90), cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0, 0.02);
    cv::putText(image, "Y", Point(80, 55), cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 1.3, cv::Scalar(0, 0, 0), 2);

    //    SPECIAL CASE:
//    vector<Segment> segments = {
//            {{400, 500}, {100, 100}, 0},
//            {{400, 500}, {800, 400}, 1},
//            {{800, 400}, {900, 100}, 2},
//            {{400, 600}, {400, 100}, 3},
//            {{20,  350}, {900, 350}, 4},
//            {{700, 700}, {400, 500}, 5},
//            {{900,  400}, {900,  350}, 6},
//            {{20,  350}, {200,  100}, 7},
//    };

    vector<Segment> segments;
    for (int i = 0; i < randomLineSegmentsCount; ++i) {
        double x1 = random_double(0, 1000);
        double y1 = random_double(0, 1000);
        double x2 = random_double(0, 1000);
        double y2 = random_double(0, 500);
        if (y1 > y2) {
            segments.push_back({{x1, y1}, {x2, y2}, i});
        } else {
            segments.push_back({{x2, y2}, {x1, y1}, i});
        }
    }

    cout << "Segments :" << endl;
    for (const auto &seg: segments) {
        cout << "(" << seg.start.x << "," << seg.start.y << "),(" << seg.end.x << ","
             << seg.end.y << ")," << seg.id << ")," << endl;
    }
    cout << endl;

    findIntersections(segments);

    for (size_t i = 0; i < segments.size(); ++i) {
        Point2d rearrangedStartVertex = rearrange_point(segments[i].start);
        Point2d rearrangedEndVertex = rearrange_point(segments[i].end);

        cv::arrowedLine(image, rearrangedStartVertex, rearrangedEndVertex, cv::Scalar(146, 146, 146), 2, cv::LINE_AA, 0,
                        0);
        cv::circle(image, rearrangedStartVertex, 8, cv::Scalar(255, 153, 51), -1);
        cv::circle(image, rearrangedEndVertex, 8, cv::Scalar(255, 153, 51), -1);
    }

    std::sort(intersection_points.begin(), intersection_points.end(), compareIntersectionPoints);

    std::vector<std::pair<std::vector<int>, Point2d>> unique_intersection_points = {intersection_points[0]};
    for (int i = 0; i < intersection_points.size(); i++) {
        Point2d current_intersection_location = intersection_points[i].second;
        Point2d pre_intersection_point = unique_intersection_points[unique_intersection_points.size() - 1].second;
        if (current_intersection_location.x != pre_intersection_point.x ||
            current_intersection_location.y != pre_intersection_point.y) {
            unique_intersection_points.push_back(intersection_points[i]);
        }
    }


    std::cout << "Intersection points:" << std::endl;
    for (const auto &pair: unique_intersection_points) {
        std::cout << "Intersection point : (" << pair.second.x << "," << pair.second.y << ")" << std::endl;
        std::cout << "IDs: ";
        for (const auto &id: pair.first) {
            std::cout << id << ", ";
        }
        std::cout << std::endl;
        cv::circle(image, rearrange_point(pair.second), 6, cv::Scalar(0, 0, 256), -1);
    }
    cout << endl;
    cout << "Compared segments number: " << compared_number << endl;

    cv::imshow("Line Segment Intersection", image);
    cv::waitKey(0);
    return 0;
}
