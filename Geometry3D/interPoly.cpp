#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <variant>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <cassert>

//#include "Generator.hpp"


template<typename TScalar>
struct CVector {
    explicit CVector(TScalar x = 0, TScalar y = 0, TScalar z = 0) : x(x), y(y), z(z) {}

    TScalar x;
    TScalar y;
    TScalar z;

    CVector operator*(const TScalar &k) const { return CVector(k * x, k * y, k * z); }

    CVector operator/(const TScalar &k) const { return CVector(x / k, y / k, z / k); }

    CVector operator+(const CVector &v) const { return CVector(this->x + v.x, this->y + v.y, this->z + v.z); }

    CVector operator-(const CVector &v) const { return CVector(this->x - v.x, this->y - v.y, this->z - v.z); }

    CVector operator*(const CVector &v) const;

    CVector operator-() { return CVector<TScalar>(-x, -y, -z); }

    bool operator==(const CVector &rhs) const;

    bool operator!=(const CVector &rhs) const;

    bool operator<(const CVector &rhs) const;

    bool operator>(const CVector &rhs) const;

    bool operator<=(const CVector &rhs) const;

    bool operator>=(const CVector &rhs) const;

    TScalar Length() const;
};

template<typename TScalar>
using CPoint = CVector<TScalar>;

template<typename TScalar>
TScalar GetDotProduct(const CVector<TScalar> &v, const CVector<TScalar> &u) {
    TScalar ans = v.x * u.x + v.y * u.y + v.z * u.z;
    return ans;
}

template<typename TScalar>
CVector<TScalar> GetCrossProduct(const CVector<TScalar> &a, const CVector<TScalar> &b) {
    TScalar i = a.y * b.z - a.z * b.y;
    TScalar j = a.z * b.x - a.x * b.z;
    TScalar k = a.x * b.y - a.y * b.x;
    return CVector<TScalar>(i, j, k);
}


template<typename TScalar>
TScalar CVector<TScalar>::Length() const {
    CVector<TScalar> v = *this;
    return std::sqrt(GetDotProduct(v, v));
}

template<typename TScalar>
CVector<TScalar> CVector<TScalar>::operator*(const CVector &v) const {
    return GetCrossProduct<TScalar>(*this, v);
}

template<typename TScalar>
bool CVector<TScalar>::operator==(const CVector &rhs) const {
    return x == rhs.x &&
           y == rhs.y &&
           z == rhs.z;
}

template<typename TScalar>
bool CVector<TScalar>::operator!=(const CVector &rhs) const {
    return !(rhs == *this);
}

template<typename TScalar>
bool CVector<TScalar>::operator<(const CVector &rhs) const {
    if (x < rhs.x)
        return true;
    if (rhs.x < x)
        return false;
    if (y < rhs.y)
        return true;
    if (rhs.y < y)
        return false;
    return z < rhs.z;
}

template<typename TScalar>
bool CVector<TScalar>::operator>(const CVector &rhs) const {
    return rhs < *this;
}

template<typename TScalar>
bool CVector<TScalar>::operator<=(const CVector &rhs) const {
    return !(rhs < *this);
}

template<typename TScalar>
bool CVector<TScalar>::operator>=(const CVector &rhs) const {
    return !(*this < rhs);
}

template<typename TScalar>
TScalar GetVolume(const CVector<TScalar> &v1, const CVector<TScalar> &v2, const CVector<TScalar> &v3) {
    return GetDotProduct(v1, v2 * v3);
}


template<typename TScalar>
bool CompareAngle(const CVector<TScalar> &v1, const CVector<TScalar> &v2, const TScalar EPS) {
    if (v1.x > EPS && v2.x < -EPS) {
        return true;
    } else if(v1.x < -EPS && v2.x > EPS) {
        return false;
    }
    TScalar k1 = v1.y / v1.x;
    TScalar k2 = v2.y / v2.x;

    if (std::abs(v1.x) < EPS) {
        if (v2.y > EPS) {
            return v2.x < -EPS;
        }
        else {
            return false;
        }
    }
    if (std::abs(v2.x) < EPS) {
        if (v2.y > EPS) {
            return v1.x > EPS;
        }
        else {
            return std::abs(k1 - k2) > EPS;
        }
    }
    if (std::abs(k1 - k2) < EPS) {
        return false;
    }
    return  k1 < k2;
}

template<typename TScalar>
struct CSegment {
    CSegment(const CVector<TScalar> &u, const CVector<TScalar> &v) : v(v), u(u) {}

    CVector<TScalar> u;
    CVector<TScalar> v;

    TScalar Length() const;
};

template<typename TScalar>
using CLine = CSegment<TScalar>;

template<typename TScalar>
TScalar CSegment<TScalar>::Length() const {
    CVector<TScalar> V(this->v - this->u);
    return CVector<TScalar>(V).Length();
}

template<typename TScalar>
bool Collinear3Points(const CPoint<TScalar> &p1, const CPoint<TScalar> &p2, const CPoint<TScalar> &p3) {
    CVector<TScalar> norm = GetCrossProduct<TScalar>(p1 - p2, p3 - p2);
    return GetDotProduct(norm, norm) == 0;
}


template<typename TScalar>
struct CPolygon {
public:
    explicit CPolygon(const std::list<CVector<TScalar>> & _points) {
        points = std::vector<CVector<TScalar>>(std::begin(_points), std::end(_points));
    }

    bool operator&(CPolygon<TScalar> &polygon);
    std::vector<CVector<TScalar>> points;

    bool InPolygon(const CPoint<TScalar> & p, TScalar EPS);
};

template<typename TScalar>
bool CPolygon<TScalar>::operator&(CPolygon<TScalar> &polygon) {
    int i = 0, j = 0;
    std::list<CVector<TScalar>> inter;
    this->points.push_back(points[0]);
    this->points.push_back(points[1]);
    polygon.points.push_back(polygon.points[0]);
    polygon.points.push_back(polygon.points[1]);
    for (; i < points.size() - 2 && j < polygon.points.size() - 2;) {
        inter.push_back(points[i] + polygon.points[j]);
        if (CompareAngle<long double>(points[i + 1] - points[i], polygon.points[j + 1] - polygon.points[j], 1e-15)) {
            i++;
        } else if (CompareAngle<long double>(polygon.points[j + 1] - polygon.points[j], points[i + 1] - points[i], 1e-15)) {
            j++;
        } else {
            j++;
            i++;
        }
    }
    this->points.pop_back();
    this->points.pop_back();
    polygon.points.pop_back();
    polygon.points.pop_back();

    CPolygon<TScalar> answer(inter);

    return answer.InPolygon(CPoint<TScalar>(0, 0, 0), 1e-15);
}

template<typename TScalar>
bool CPolygon<TScalar>::InPolygon(const CPoint<TScalar> &p, TScalar EPS) {
    bool result = false;
    size_t i, j;
    for (i = 0, j = points.size() - 1; i < points.size(); j = i++) {
        if (((points[i].y > p.y) != (points[j].y > p.y)) &&
            p.x < (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x) {
            result = !result;
        }

    }

    return result;
}

std::list<CVector<long double>> Input(int n) {
    std::list<CVector<long double>> p1;
    long double x, y;
    CVector<long double> minPoint(INT32_MAX, INT32_MAX, 0);
    for (int i = 0; i < n; i++) {
        std::cin >> x >> y;
        CVector<long double> p(x, y, 0);
        minPoint = std::min(minPoint, p);
        p1.emplace_back(x, y, 0);
    }
    p1.reverse();
    auto it = p1.begin();
    while (*it != minPoint) { ++it; }
    std::list<CVector<long double>> temp1(it, p1.end()), temp2(p1.begin(), it);
    temp1.splice(temp1.end(), temp2);
    return temp1;
}


int main() {
    int n, m;
    std::cin >> n;
    auto p1 = Input(n);
    std::cin >> m;
    auto p2 = Input(m);

    for (auto &i : p2) {
        i = i * -1;
    }

    CPolygon<long double> poly1(p1), poly2(p2);
    if ( poly1 & poly2) {
        std::cout << "YES";
    }
    else {
        std::cout << "NO";
    }

    return 0;
}
