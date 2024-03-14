#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <opencv2/opencv.hpp>


using namespace std;

typedef cv::Point Point;
typedef cv::Point2d Point2d;

int randomLineSegmentsCount = 20;


double random_double(double s, double e) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dis(s, e);
    return dis(gen);
}

struct Event {
    Point2d point;
    int segment_id;
    bool is_start;
};

struct Segment {
    Point2d start;
    Point2d end;
    int id;
};

struct EventCompare {
    bool operator()(const Event &e1, const Event &e2) const {
        return e1.point.x < e2.point.x || (e1.point.x == e2.point.x && e1.point.y < e2.point.y);
    }
};

set<Event, EventCompare> event_queue;
vector<Point2d> intersection_points;

bool intersect(const Segment &s1, const Segment &s2) {
    double det1 = (s2.end - s2.start).cross(s1.start - s2.start);
    double det2 = (s2.end - s2.start).cross(s1.end - s2.start);
    double det3 = (s1.end - s1.start).cross(s2.start - s1.start);
    double det4 = (s1.end - s1.start).cross(s2.end - s1.start);

    return (det1 * det2 < 0) && (det3 * det4 < 0);
}

Point2d intersection_point(const Segment &s1, const Segment &s2) {
    double t = ((s2.start - s1.start).cross(s2.end - s2.start)) / ((s1.end - s1.start).cross(s2.end - s2.start));
    return s1.start + t * (s1.end - s1.start);
}

void add_events(const vector<Segment> &segments) {
    for (size_t i = 0; i < segments.size(); ++i) {
        Event start_event;
        start_event.point = segments[i].start;
        start_event.segment_id = segments[i].id;
        start_event.is_start = true;

        Event end_event;
        end_event.point = segments[i].end;
        end_event.segment_id = segments[i].id;
        end_event.is_start = false;

        event_queue.insert(start_event);
        event_queue.insert(end_event);
    }
}

void find_intersections(const vector<Segment> &segments) {
    vector<Segment> active_segments;

    while (!event_queue.empty()) {
        Event current_event = *event_queue.begin();
        event_queue.erase(event_queue.begin());

        if (current_event.is_start) {
            for (auto &seg: active_segments) {
                if (intersect(segments[current_event.segment_id], seg)) {
                    Point2d intersect_point = intersection_point(segments[current_event.segment_id], seg);
                    intersection_points.push_back(intersect_point);
                }
            }
            active_segments.push_back(segments[current_event.segment_id]);
            sort(active_segments.begin(), active_segments.end(), [](const Segment &s1, const Segment &s2) {
                return s1.end.y < s2.end.y;
            });
        } else {
            active_segments.erase(remove_if(active_segments.begin(), active_segments.end(), [&](const Segment &s) {
                return s.id == current_event.segment_id;
            }), active_segments.end());
        }
    }
}

int main() {
    vector<Segment> segments;
    for (int i = 0; i < randomLineSegmentsCount; ++i) {
        double x1 = random_double(0, 700);
        double y1 = random_double(0, 1000);
        double x2 = random_double(x1, 1000);
        double y2 = random_double(0, 1000);
        segments.push_back({{x1, y1}, {x2, y2}, i});
    }

    cout << "Segments :" << endl;
    for (const auto &seg: segments) {
        cout << "Segment " << seg.id << ": (" << seg.start.x << "," << seg.start.y << ")-(" << seg.end.x << ","
             << seg.end.y << ")" << endl;
    }

    add_events(segments);
    find_intersections(segments);

    cout << "Intersection points:" << endl;
    for (const auto &point: intersection_points) {
        cout << "(" << point.x << ", " << point.y << ")" << endl;

    }

    return 0;
}
