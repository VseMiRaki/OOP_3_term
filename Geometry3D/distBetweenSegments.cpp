#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

template<typename TScalar>
struct CVector {
    CVector(TScalar x, TScalar y, TScalar z) : x(x), y(y), z(z) {}

    TScalar x;
    TScalar y;
    TScalar z;

    CVector operator*(const TScalar &k) const { return CVector(k * x, k * y, k * z); }

    CVector operator/(const TScalar &k) const { return CVector(x / k, y / k, z / k); }

    CVector operator+(const CVector &v) const { return CVector(this->x + v.x, this->y + v.y, this->z + v.z); }

    CVector operator-(const CVector &v) const { return CVector(this->x - v.x, this->y - v.y, this->z - v.z); }

    CVector operator*(const CVector &v) const;


    TScalar Length() const;
};

template<typename TScalar>
using CPoint = CVector<TScalar>;

template<typename TScalar>
TScalar get_dot_product(const CVector<TScalar> &v, const CVector<TScalar> &u) {
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

template<typename TScalar>
TScalar get_cross_product(const CVector<TScalar> &a, const CVector<TScalar> &b) {
    TScalar i = a.y * b.z - a.z * b.y;
    TScalar j = a.z * b.x - a.x * b.z;
    TScalar k = a.x * b.y - a.y * b.x;
    return CVector<TScalar>(i, j, k);
}


template<typename TScalar>
TScalar CVector<TScalar>::Length() const {
    return std::sqrt(get_dot_product(*this, *this));
}

template<typename TScalar>
CVector<TScalar> CVector<TScalar>::operator*(const CVector &v) const {
    return get_cross_product<TScalar>(*this, v);
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
    return CVector<TScalar>(this->v - this->u).Length();
}

template<typename TScalar>
TScalar get_distance_between_segments(const CSegment<TScalar> &S1, const CSegment<TScalar> &S2, TScalar EPS) {
    CVector<TScalar> u(S1.v - S1.u);
    CVector<TScalar> v(S2.v - S2.u);
    CVector<TScalar> w(S1.u - S2.u);
    TScalar dot_u_u = get_dot_product<TScalar>(u, u);
    TScalar dot_v_v = get_dot_product<TScalar>(v, v);
    TScalar dot_u_v = get_dot_product<TScalar>(u, v);
    TScalar dot_u_w = get_dot_product<TScalar>(u, w);
    TScalar dot_v_w = get_dot_product<TScalar>(v, w);

    //Делаем параметризацию отрезков
    //Точка на отрезке будет иметь вид sc * u и tc * v
    TScalar Det = dot_u_u * dot_v_v - dot_u_v * dot_u_v;
    TScalar sc, tc;
    //sc = segment1_N / segment1_D
    TScalar segment1_N, segment1_D = Det;
    TScalar segment2_N, segment2_D = Det;

    if (Det <= EPS) { //проверка на лнз
        segment1_N = 0;
        segment1_D = 1;
        segment2_N = dot_v_w;
        segment2_D = dot_v_v;
    } else {
        segment1_N = (dot_u_v * dot_v_w - dot_v_v * dot_u_w);
        segment2_N = (dot_u_u * dot_v_w - dot_u_v * dot_u_w);
        if (segment1_N <= EPS) {
            segment1_N = 0;
            segment2_N = dot_v_w;
            segment2_D = dot_v_v;
        } else if (segment1_N > segment1_D) {
            segment1_N = segment1_D;
            segment2_N = dot_v_w + dot_u_v;
            segment2_D = dot_v_v;
        }
    }

    if (segment2_N > segment2_D) {
        segment2_N = segment2_D;
        if ((-dot_u_w + dot_u_v) <= EPS) {
            segment1_N = 0;
        } else if ((-dot_u_w + dot_u_v) > dot_u_u) {
            segment1_N = segment1_D;
        } else {
            segment1_N = (-dot_u_w + dot_u_v);
            segment1_D = dot_u_u;
        }
    } else if (segment2_N <= EPS) {
        segment2_N = 0;
        if (dot_u_w >= EPS) {
            segment1_N = 0;
        } else if (-dot_u_w > dot_u_u) {
            segment1_N = segment1_D;
        } else {
            segment1_D = dot_u_u;
            segment1_N = -dot_u_w;
        }
    }
    if (std::abs(segment1_N) <= EPS) {
        sc = 0;
    } else {
        sc = segment1_N / segment1_D;
    }
    if (std::abs(segment2_N) <= EPS) {
        tc = 0;
    } else {
        tc = segment2_N / segment2_D;
    }
    return (w + u * sc - v * tc).Length();
};


int main() {
    int x1, y1, z1, x2, y2, z2;
    std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
    CSegment<long double> S1(CVector<long double>(x1, y1, z1), CVector<long double>(x2, y2, z2));
    std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
    CSegment<long double> S2(CVector<long double>(x1, y1, z1), CVector<long double>(x2, y2, z2));
    std::cout << std::setprecision(12) << get_distance_between_segments<long double>(S1, S2, 1e-15);

    return 0;
}
