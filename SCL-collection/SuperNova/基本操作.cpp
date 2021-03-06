#include <cmath>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#define foreach(e,x) for(__typeof(x.begin()) e=x.begin();e!=x.end();++e)
using namespace std;

const double PI = acos(-1.);
const double EPS = 1e-8;
inline int sign(double a) {
	return a < -EPS ? -1 : a > EPS;
}

struct Point {
	double x, y;
	Point() {
	}
	Point(double _x, double _y) :
			x(_x), y(_y) {
	}
	Point operator+(const Point&p) const {
		return Point(x + p.x, y + p.y);
	}
	Point operator-(const Point&p) const {
		return Point(x - p.x, y - p.y);
	}
	Point operator*(double d) const {
		return Point(x * d, y * d);
	}
	Point operator/(double d) const {
		return Point(x / d, y / d);
	}
	bool operator<(const Point&p) const {
		int c = sign(x - p.x);
		if (c)
			return c == -1;
		return sign(y - p.y) == -1;
	}
	double dot(const Point&p) const {
		return x * p.x + y * p.y;
	}
	double det(const Point&p) const {
		return x * p.y - y * p.x;
	}
	double alpha() const {
		return atan2(y, x);
	}
	double distTo(const Point&p) const {
		double dx = x - p.x, dy = y - p.y;
		return hypot(dx, dy);
	}
	double alphaTo(const Point&p) const {
		double dx = x - p.x, dy = y - p.y;
		return atan2(dy, dx);
	}
	//clockwise
	Point rotAlpha(const double &alpha, const Point &o = Point(0, 0)) const {
		double nx = cos(alpha) * (x - o.x) + sin(alpha) * (y - o.y);
		double ny = -sin(alpha) * (x - o.x) + cos(alpha) * (y - o.y);
		return Point(nx, ny) + o;
	}
	Point rot90() const {
		return Point(-y, x);
	}
	Point unit() {
		return *this / abs();
	}
	void read() {
		scanf("%lf%lf", &x, &y);
	}
	double abs() {
		return hypot(x, y);
	}
	double abs2() {
		return x * x + y * y;
	}
	void write() {
		cout << "(" << x << "," << y << ")" << endl;
	}
};

#define cross(p1,p2,p3) ((p2.x-p1.x)*(p3.y-p1.y)-(p3.x-p1.x)*(p2.y-p1.y))

#define crossOp(p1,p2,p3) sign(cross(p1,p2,p3))

Point isSS(Point p1, Point p2, Point q1, Point q2) {
	double a1 = cross(q1,q2,p1), a2 = -cross(q1,q2,p2);
	return (p1 * a2 + p2 * a1) / (a1 + a2);
}

vector<Point> convexCut(const vector<Point>&ps, Point q1, Point q2) {
	vector<Point> qs;
	int n = ps.size();
	for (int i = 0; i < n; ++i) {
		Point p1 = ps[i], p2 = ps[(i + 1) % n];
		int d1 = crossOp(q1,q2,p1), d2 = crossOp(q1,q2,p2);
		if (d1 >= 0)
			qs.push_back(p1);
		if (d1 * d2 < 0)
			qs.push_back(isSS(p1, p2, q1, q2));
	}
	return qs;
}

double calcArea(const vector<Point>&ps) {
	int n = ps.size();
	double ret = 0;
	for (int i = 0; i < n; ++i) {
		ret += ps[i].det(ps[(i + 1) % n]);
	}
	return ret / 2;
}

vector<Point> convexHull(vector<Point> ps) {
	int n = ps.size();
	if (n <= 1)
		return ps;
	sort(ps.begin(), ps.end());
	vector<Point> qs;
	for (int i = 0; i < n; qs.push_back(ps[i++])) {
		while (qs.size() > 1 && crossOp(qs[qs.size()-2],qs.back(),ps[i]) <= 0)
			qs.pop_back();
	}
	for (int i = n - 2, t = qs.size(); i >= 0; qs.push_back(ps[i--])) {
		while ((int)qs.size() > t && crossOp(qs[(int)qs.size()-2],qs.back(),ps[i]) <= 0)
			qs.pop_back();
	}
	qs.pop_back();
	return qs;
}

double convexDiameter(const vector<Point>&ps) {
	int n = ps.size();
	int is = 0, js = 0;
	for (int i = 1; i < n; ++i) {
		if (ps[i].x > ps[is].x)
			is = i;
		if (ps[i].x < ps[js].x)
			js = i;
	}
	double maxd = ps[is].distTo(ps[js]);
	int i = is, j = js;
	do {
		if ((ps[(i + 1) % n] - ps[i]).det(ps[(j + 1) % n] - ps[j]) >= 0)
			(++j) %= n;
		else
			(++i) %= n;
		maxd = max(maxd, ps[i].distTo(ps[j]));
	} while (i != is || j != js);
	return maxd;
}

int onSegment(Point p, Point q1, Point q2)
{
	return crossOp(q1, q2, p) == 0 && sign((p - q1).dot(p - q2)) <= 0;
}

int isIntersect(Point p1, Point p2, Point q1, Point q2)
{
	return crossOp(p1, p2, q1) * crossOp(p1, p2, q2) < 0 && crossOp(q1, q2, p1) * cross(q1, q2, p2) < 0;
}

int isParallel(Point p1, Point p2, Point q1, Point q2)
{
	return sign((p2 - p1).det(q2 - q1)) == 0;
}

double distPointToLine(Point p, Point u, Point v)
{
	return abs((u - p).det(v - p)) / u.distTo(v);
}

int insidePolygon(Point q, vector<Point> &p)
{
	int n = p.size();
	for(int i = 0; i < n; ++ i) {
		if (onSegment(q, p[i], p[(i + 1) % n])) return false;
	}
	Point q2;
	double offsite = LIM;
	for( ; ; ) {
		int flag = true;
		int rnd = rand() % 10000;
		q2.x = cos(rnd) * offsite;
		q2.y = sin(rnd) * offsite;
		for(int i = 0; i < n; ++ i) {
			if (onSegment(p[i], q, q2)) {
				flag = false;
				break;
			}
		}
		if (flag) break;
	}
	int cnt = 0;
	for(int i = 0; i < n; ++ i) {
		cnt += isIntersect(p[i], p[(i + 1) % n], q, q2);
	}
	return cnt & 1;
}

int isIntersectLineToCircle(Point c, double r, Point l1, Point l2)
{
	return (distPointToLine(c, l1, l2) - r) <= 0;
}

int isIntersectSegmentToCircle(Point c, double r, Point p1, Point p2)
{
	if ((distPointToLine(c, p1, p2) - r) > 0) return false;
	if (sign(c.distTo(p1) - r) <= 0 || sign(c.distTo(p2) - r) <= 0) return true;
	Point c2 = (p2 - p1).rot90() + c;
	return crossOp(c, c2, p1) * crossOp(c, c2, p2) <= 0;
}

int isIntersectCircleToCircle(Point c1, double r1, Point c2, double r2)
{
	double dis = c1.distTo(c2);
	return sign(dis - abs(r1 - r2)) >= 0 && sign(dis - (r1 + r2)) <= 0;
}

void intersectionLineToCircle(Point c, double r, Point l1, Point l2, Point& p1, Point& p2) {
	Point c2 = c + (l2 - l1).rot90();
	c2 = isSS(c, c2, l1, l2);
	double t = sqrt(r * r - (c2 - c).abs2());
	p1 = c2 + (l2 - l1).unit() * t;
	p2 = c2 - (l2 - l1).unit() * t;
}

void intersectionCircleToCircle(Point c1, double r1, Point c2, double r2, Point &p1, Point &p2) {
	double t = (1 + (r1 * r1 - r2 * r2) / (c1 - c2).abs2()) / 2;
	Point u = c1 + (c2 - c1) * t;
	Point v = u + (c2 - c1).rot90();
	intersectionLineToCircle(c1, r1, u, v, p1, p2);
}
