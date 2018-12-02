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
TScalar get_dot_product(const CVector<TScalar> &v, const CVector<TScalar> &u) {
    TScalar ans = v.x * u.x + v.y * u.y + v.z * u.z;
    return ans;
}

template<typename TScalar>
CVector<TScalar> Cross_product(const CVector<TScalar> &a, const CVector<TScalar> &b) {
    TScalar i = a.y * b.z - a.z * b.y;
    TScalar j = a.z * b.x - a.x * b.z;
    TScalar k = a.x * b.y - a.y * b.x;
    return CVector<TScalar>(i, j, k);
}


template<typename TScalar>
TScalar CVector<TScalar>::Length() const {
    CVector<TScalar> v = *this;
    return std::sqrt(get_dot_product(v, v));
}

template<typename TScalar>
CVector<TScalar> CVector<TScalar>::operator*(const CVector &v) const {
    return Cross_product<TScalar>(*this, v);
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
TScalar get_volume(const CVector<TScalar> &v1, const CVector<TScalar> &v2, const CVector<TScalar> &v3) {
    return get_dot_product(v1, v2 * v3);
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

    TScalar Det = dot_u_u * dot_v_v - dot_u_v * dot_u_v;
    TScalar sc, tc;
    TScalar sN, sD = Det;
    TScalar tN, tD = Det;

    if (Det <= EPS) { //проверка на лнз
        sN = 0;
        sD = 1;
        tN = dot_v_w;
        tD = dot_v_v;
    } else {
        sN = (dot_u_v * dot_v_w - dot_v_v * dot_u_w);
        tN = (dot_u_u * dot_v_w - dot_u_v * dot_u_w);
        if (sN <= EPS) {
            sN = 0;
            tN = dot_v_w;
            tD = dot_v_v;
        } else if (sN > sD) {
            sN = sD;
            tN = dot_v_w + dot_u_v;
            tD = dot_v_v;
        }
    }

    if (tN > tD) {
        tN = tD;
        if ((-dot_u_w + dot_u_v) <= EPS) {
            sN = 0;
        } else if ((-dot_u_w + dot_u_v) > dot_u_u) {
            sN = sD;
        } else {
            sN = (-dot_u_w + dot_u_v);
            sD = dot_u_u;
        }
    } else if (tN <= EPS) {
        tN = 0;
        if (dot_u_w >= EPS) {
            sN = 0;
        } else if (-dot_u_w > dot_u_u) {
            sN = sD;
        } else {
            sD = dot_u_u;
            sN = -dot_u_w;
        }
    }
    if (fabs(sN) <= EPS) {
        sc = 0;
    } else {
        sc = sN / sD;
    }
    if (fabs(tN) <= EPS) {
        tc = 0;
    } else {
        tc = tN / tD;
    }
    CVector<TScalar> ans = w + u * sc - v * tc;
    TScalar dist = ans.Length();
    return dist;


};

template<typename TScalar>
struct CPlane {
    CPlane(const CPoint<TScalar> &p, const CVector<TScalar> &u, const CVector<TScalar> &v);

    CPlane(TScalar A, TScalar B, TScalar C, TScalar D);

    TScalar Value(const CPoint<TScalar> &P);

    bool NotSameLocation(const CPoint<TScalar> &P1, const CPoint<TScalar> &P2);

    TScalar A, B, C, D;
    CPoint<TScalar> p;
    CVector<TScalar> u, v;
};

template<typename TScalar>
CPlane<TScalar>::CPlane(const CPoint<TScalar> &p, const CVector<TScalar> &u, const CVector<TScalar> &v) : p(p), u(u),
                                                                                                          v(v) {
    //Code is coming
}

template<typename TScalar>
CPlane<TScalar>::CPlane(TScalar A, TScalar B, TScalar C, TScalar D):A(A), B(B), C(C), D(D) {
    //code is coming
}

template<typename TScalar>
bool CPlane<TScalar>::NotSameLocation(const CPoint<TScalar> &P1, const CPoint<TScalar> &P2) {
    int val1 = Value(P1), val2 = Value(P2);
    return !((val1 >= 0 && val2 >= 0) || (val1 <= 0 && val2 <= 0));
}

template<typename TScalar>
TScalar CPlane<TScalar>::Value(const CPoint<TScalar> &P) {
    return A * P.x + B * P.y + C * P.z + D;
}

template<typename TScalar>
std::size_t my_hash(const CPoint<TScalar> &p) {
    int h = p.x + 1000 * p.y + 1000000 * p.z;
    return std::hash<int>{}(h);
}

template<typename TScalar>
struct hash {
    std::size_t operator()(const CPoint<TScalar> &k) const {

        return my_hash<TScalar>(k);
    }
};

template<typename TScalar>
bool IsPointsCollinear(const CPoint<TScalar> &p1, const CPoint<TScalar> &p2, const CPoint<TScalar> &p3) {
    CVector<TScalar> norm = Cross_product<TScalar>(p1 - p2, p3 - p2);
    return get_dot_product(norm, norm) == 0 ;
}

class ConvexHull {
public:
    explicit ConvexHull();

    ~ConvexHull();

    struct CHPoint;
    struct CHEdge;
    struct CHFace;

    struct CHPoint {
        CHPoint(CPoint<int> p, int n);

        CPoint<int> point;
        int p_number;
        std::shared_ptr<CHEdge> inc_edges;  //incident edge;
        bool flag; //true if point's on the hull
        bool processed; //true if point's processed
        std::shared_ptr<CHPoint> next, prev;
    };

    struct CHFace {
        CHFace(std::shared_ptr<CHPoint> p1, std::shared_ptr<CHPoint> p2, std::shared_ptr<CHPoint> p3);

        std::array<std::shared_ptr<CHEdge>, 3> edges;
        std::array<std::shared_ptr<CHPoint>, 3> points;
        bool visible;
        std::shared_ptr<CHFace> next, prev;
    };

    struct CHEdge {
        CHEdge(std::shared_ptr<CHPoint> p1, std::shared_ptr<CHPoint> p2);

        std::array<std::shared_ptr<CHFace>, 2> faces;
        std::array<std::shared_ptr<CHPoint>, 2> points;
        std::shared_ptr<CHFace> face;
        bool removed; // true if will delete nex time
        std::shared_ptr<CHEdge> next, prev;
    };

    void Read(int n);

    void Build();

    void MakeTetrahedron();

    void Print();


private:

    std::shared_ptr<CHPoint> root;
    std::shared_ptr<CHEdge> e;
    std::shared_ptr<CHFace> f;

    void update_edges();

    void update_points();

    void update_faces();

    void update();

    void Correcting(std::shared_ptr<CHFace> &_f, std::shared_ptr<CHEdge> &_e, std::shared_ptr<CHPoint> &_p);

    std::shared_ptr<CHFace>
    MakeFace(std::shared_ptr<ConvexHull::CHEdge> &_e, std::shared_ptr<ConvexHull::CHPoint> &_p);

    void Add(std::shared_ptr<CHPoint> &_root, std::shared_ptr<CHPoint> &_new);

    void Add(std::shared_ptr<CHFace> &_root, std::shared_ptr<CHFace> &_new);

    void Add(std::shared_ptr<CHEdge> &_root, std::shared_ptr<CHEdge> &_new);


    void Delete(std::shared_ptr<CHPoint> &_root, std::shared_ptr<CHPoint> &_new);

    void Delete(std::shared_ptr<CHFace> &_root, std::shared_ptr<CHFace> &_new);

    void Delete(std::shared_ptr<CHEdge> &_root, std::shared_ptr<CHEdge> &_new);


    bool AddPoint(std::shared_ptr<CHPoint> &_p);

    void addNewFace();

    void markPoints();

};

ConvexHull::CHFace::CHFace(std::shared_ptr<ConvexHull::CHPoint> p1, std::shared_ptr<ConvexHull::CHPoint> p2,
                           std::shared_ptr<ConvexHull::CHPoint> p3) :
        visible(false), next(nullptr), prev(nullptr) {
    edges[0] = edges[1] = edges[2] = nullptr;
    points[0] = std::move(p1);
    points[1] = std::move(p3);
    points[2] = std::move(p2);
}

ConvexHull::CHEdge::CHEdge(std::shared_ptr<ConvexHull::CHPoint> p1, std::shared_ptr<ConvexHull::CHPoint> p2) :
        face(nullptr), removed(false), next(nullptr), prev(nullptr) {
    points[0] = std::move(p1);
    points[1] = std::move(p2);
    faces[0] = faces[1] = nullptr;
}

ConvexHull::CHPoint::CHPoint(CPoint<int> p, int n) : point(p), p_number(n), inc_edges(nullptr), flag(false),
                                                     processed(false), next(nullptr), prev(nullptr) {}

void ConvexHull::Add(std::shared_ptr<CHPoint> &_root, std::shared_ptr<CHPoint> &_new) {
    if (_root) {
        _new->next = _root->next;
        _new->prev = _root;
        _root->next = _new;
        (_new->next)->prev = _new;
    } else {
        _root = _new;
        _root->next = _root->prev = _new;
    }
}

void ConvexHull::Add(std::shared_ptr<CHFace> &_root, std::shared_ptr<CHFace> &_new) {
    if (_root) {
        _new->next = _root->next;
        _new->prev = _root;
        _root->next = _new;
        (_new->next)->prev = _new;
    } else {
        _root = _new;
        _root->next = _root->prev = _new;
    }
}

void ConvexHull::Add(std::shared_ptr<CHEdge> &_root, std::shared_ptr<CHEdge> &_new) {
    if (_root) {
        _new->next = _root->next;
        _new->prev = _root;
        _root->next = _new;
        _new->next->prev = _new;
    } else {
        _root = _new;
        _root->next = _root->prev = _new;
    }
}


void ConvexHull::Read(int n) {
    int x, y, z;
    std::vector<CPoint<int>> v;
    for (int i = 0; i < n; i++) {
        std::cin >> x >> y >> z;
        CPoint<int> p(x, y, z);
        v.push_back(p);
        auto new_point_ptr = std::make_shared<CHPoint>(p, i);
        Add(root, new_point_ptr);
    }
}

void ConvexHull::MakeTetrahedron() {

    std::shared_ptr<CHPoint> v = root;
    auto first_v = v;

    while (IsPointsCollinear(v->point, v->next->point, v->next->next->point)) {
        v = v->next;
        if (first_v == v) {
            //Здесь обработчик случаев, если все точки на прямой
        }
    }
    v->processed = v->next->processed = v->next->next->processed = true;

    auto e1_ptr = std::make_shared<CHEdge>(v, v->next);
    auto e2_ptr = std::make_shared<CHEdge>(v->next, v->next->next);
    auto e3_ptr = std::make_shared<CHEdge>(v->next->next, v);

    Add(e, e1_ptr);
    Add(e, e2_ptr);
    Add(e, e3_ptr);

    auto nf_ptr = std::make_shared<CHFace>(v, v->next, v->next->next);

    nf_ptr->edges[0] = e1_ptr;
    nf_ptr->edges[1] = e2_ptr;
    nf_ptr->edges[2] = e3_ptr;

    Add(f, nf_ptr);

    e1_ptr->faces[0] = e2_ptr->faces[0] = e3_ptr->faces[0] = nf_ptr;

    auto next_v = v->next->next->next;

    while (std::abs(get_volume<int>(nf_ptr->points[0]->point - next_v->point, nf_ptr->points[1]->point - next_v->point,
                                    nf_ptr->points[2]->point - next_v->point)) == 0) {
        next_v = next_v->next;
        if (next_v == v) {
            //Здесь должен быть обработчик случая, если все на плоскости
        }

    }

    next_v->processed = true;

    if (get_volume<int>(nf_ptr->points[0]->point - next_v->point, nf_ptr->points[1]->point - next_v->point,
                        nf_ptr->points[2]->point - next_v->point) < 0) {
        swap(f->points[1], f->points[2]);
        swap(f->edges[1], f->edges[2]);
    }

    e1_ptr->faces[1] = MakeFace(e1_ptr, next_v);
    e2_ptr->faces[1] = MakeFace(e2_ptr, next_v);
    e3_ptr->faces[1] = MakeFace(e3_ptr, next_v);

    update();
}

std::shared_ptr<ConvexHull::CHFace>
ConvexHull::MakeFace(std::shared_ptr<ConvexHull::CHEdge> &_e, std::shared_ptr<ConvexHull::CHPoint> &_p) {

    auto new_f_ptr = std::make_shared<CHFace>(CHFace(nullptr, nullptr, nullptr));

    std::vector<std::shared_ptr<CHEdge>> new_edges(2, nullptr);

    for (int i = 0; i < 2; i++) {
        new_edges[i] = _e->points[i]->inc_edges;
        if (!new_edges[i]) {
            new_edges[i] = std::make_shared<CHEdge>(CHEdge(nullptr, nullptr));
            Add(e, new_edges[i]);
            new_edges[i]->points[0] = _e->points[i];
            new_edges[i]->points[1] = _p;
            _e->points[i]->inc_edges = new_edges[i];
        }
    }


    Add(f, new_f_ptr);


    new_f_ptr->edges[0] = _e;
    new_f_ptr->edges[1] = new_edges[0];
    new_f_ptr->edges[2] = new_edges[1];

    Correcting(new_f_ptr, _e, _p);


    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (!new_edges[i]->faces[j]) {
                new_edges[i]->faces[j] = new_f_ptr;
                break;
            }
        }
    }
    return new_f_ptr;
}

void ConvexHull::Correcting(std::shared_ptr<ConvexHull::CHFace> &_f, std::shared_ptr<ConvexHull::CHEdge> &_e,
                            std::shared_ptr<CHPoint> &_p) {
    auto temp_f = std::make_shared<CHFace>(CHFace(nullptr, nullptr, nullptr));

    if (!_e->faces[1]) {
        temp_f = _e->faces[0];
    } else if (!_e->faces[0]->visible) {
        temp_f = _e->faces[0];
    } else {
        temp_f = _e->faces[1];
    }
    int i = 0;
    for (; temp_f->points[i] != _e->points[1]; i++) {}
    if (temp_f->points[(i + 1) % 3] != _e->points[0]) {
        _f->points[0] = _e->points[1];
        _f->points[1] = _e->points[0];
    } else {
        _f->points[0] = _e->points[0];
        _f->points[1] = _e->points[1];
        swap(_f->edges[1], _f->edges[2]);
    }
    _f->points[2] = _p;
}


void ConvexHull::Build() {
    auto _v = root;
    auto _vnext = root;


    _vnext = _v->next;
    if (!_v->processed) {
        _v->flag = true;
        AddPoint(_v);
        update();
    }
    _v = _vnext;
    while (_v != root) {
        _vnext = _v->next;
        if (!_v->processed) {
            _v->processed = true;
            AddPoint(_v);
            update();

        }
        _v = _vnext;
    }
}

bool ConvexHull::AddPoint(std::shared_ptr<ConvexHull::CHPoint> &_p) {
    auto _f = f;
    bool curVis = false;

    if (get_volume(_f->points[0]->point - _p->point, _f->points[1]->point - _p->point,
                   _f->points[2]->point - _p->point) < 0) {
        _f->visible = true;
        curVis = true;
    }
    _f = _f->next;
    while (_f != f) {
        if (get_volume(_f->points[0]->point - _p->point, _f->points[1]->point - _p->point,
                       _f->points[2]->point - _p->point) < 0) {
            _f->visible = true;
            curVis = true;
        }
        _f = _f->next;
    }
    if (!curVis) {
        _p->flag = false;
        return false;
    }
    auto _e = e;


    auto temp = _e->next;
    if (_e->faces[0]->visible && _e->faces[1]->visible) {
        _e->removed = true;    //Ребро стало внутри
    } else if (_e->faces[0]->visible || _e->faces[1]->visible) {
        _e->face = MakeFace(_e, _p);
    }
    _e = temp;

    while (_e != e) {
        temp = _e->next;
        if (_e->faces[0]->visible && _e->faces[1]->visible) {
            _e->removed = true;    //Ребро стало внутри
        } else if (_e->faces[0]->visible || _e->faces[1]->visible) {
            _e->face = MakeFace(_e, _p);
        }
        _e = temp;
    }
    return true;
}

ConvexHull::ConvexHull() : e(nullptr), root(nullptr), f(nullptr) {
}

void ConvexHull::update() {
    update_edges();
    update_faces();
    update_points();

}

void ConvexHull::addNewFace() {
    auto temp_e = e;
    //Проверяем каждое ребро и если мы находим ребро, у которого должна быть новая грань - добавляем ее

    if (temp_e->face) {
        if (temp_e->faces[0]->visible) {
            temp_e->faces[0] = temp_e->face;
        } else {
            temp_e->faces[1] = temp_e->face;
        }
        temp_e->face = nullptr;
    }
    temp_e = temp_e->next;
    while (temp_e != e) {
        if (temp_e->face) {
            if (temp_e->faces[0]->visible) {
                temp_e->faces[0] = temp_e->face;
            } else {
                temp_e->faces[1] = temp_e->face;
            }
            temp_e->face = nullptr;
        }
        temp_e = temp_e->next;
    }
}

void ConvexHull::update_edges() {
    auto temp_e = e;
    //Проверяем каждое ребро и если мы находим ребро, у которого должна быть новая грань - добавляем ее

    addNewFace();

    //Дальше просто удаление

    while (e && e->removed) {
        temp_e = e;
        Delete(e, temp_e);
    }
    temp_e = e->next;

    std::shared_ptr<CHEdge> t = nullptr;


    if (temp_e->removed) {
        t = temp_e;
        temp_e = temp_e->next;
        Delete(e, t);
    } else {
        temp_e = temp_e->next;
    }
    while (temp_e != e) {
        if (temp_e->removed) {
            t = temp_e;
            temp_e = temp_e->next;
            Delete(e, t);
        } else {
            temp_e = temp_e->next;
        }
    }

}

void ConvexHull::Delete(std::shared_ptr<ConvexHull::CHEdge> &_root, std::shared_ptr<ConvexHull::CHEdge> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new->next.reset();
        _new->prev.reset();
        _new->points[0].reset();
        _new->points[1].reset();
        _new->faces[0].reset();
        _new->faces[1].reset();
        _new->face.reset();
        _new.reset();
    }
}

void ConvexHull::Delete(std::shared_ptr<ConvexHull::CHPoint> &_root, std::shared_ptr<ConvexHull::CHPoint> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new->next.reset();
        _new->prev.reset();
        _new->inc_edges.reset();
        _new.reset();
    }
}

void ConvexHull::Delete(std::shared_ptr<ConvexHull::CHFace> &_root, std::shared_ptr<ConvexHull::CHFace> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new->next.reset();
        _new->prev.reset();
        _new->edges[0].reset();
        _new->edges[1].reset();
        _new->edges[2].reset();
        _new->points[0].reset();
        _new->points[1].reset();
        _new->points[2].reset();
        _new.reset();
    }
}

void ConvexHull::update_faces() {
    auto temp_f = f;
    while (f && f->visible) {
        temp_f = f;
        Delete(f, temp_f);
    }

    temp_f = f->next;
    auto t = temp_f;

    if (temp_f->visible) {
        t = temp_f;
        temp_f = temp_f->next;
        Delete(f, t);
    } else {
        temp_f = temp_f->next;
    }
    while (temp_f != f) {
        if (temp_f->visible) {
            t = temp_f;
            temp_f = temp_f->next;
            Delete(f, t);
        } else {
            temp_f = temp_f->next;
        }
    }
}

void ConvexHull::markPoints() {
    auto temp_e = e;
    temp_e->points[0]->flag = temp_e->points[1]->flag = true;
    temp_e = temp_e->next;

    while (temp_e != e) {
        temp_e->points[0]->flag = temp_e->points[1]->flag = true;
        temp_e = temp_e->next;
    }
}

void ConvexHull::update_points() {

    markPoints();

    auto temp_p = root;

    while (root && root->processed && !root->flag) {
        temp_p = root;
        Delete(root, temp_p);
    }

    temp_p = root->next;
    auto t = root;

    if (temp_p->processed && !temp_p->flag) {
        t = temp_p;
        temp_p = temp_p->next;
        Delete(root, t);
    } else {
        temp_p = temp_p->next;
    }
    while (temp_p != root) {
        if (temp_p->processed && !temp_p->flag) {
            t = temp_p;
            temp_p = temp_p->next;
            Delete(root, t);
        } else {
            temp_p = temp_p->next;
        }
    }

    temp_p = root;
    temp_p->inc_edges = nullptr;
    temp_p->flag = false;
    temp_p = temp_p->next;

    while (temp_p != root) {
        temp_p->inc_edges = nullptr;
        temp_p->flag = false;
        temp_p = temp_p->next;
    }
}

struct Cmp {
    Cmp(int a, int b, int c) : n1(a), n2(b), n3(c) {}

    int n1, n2, n3;

    bool operator<(const Cmp &rhs) const {
        if (n1 < rhs.n1)
            return true;
        if (rhs.n1 < n1)
            return false;
        if (n2 < rhs.n2)
            return true;
        if (rhs.n2 < n2)
            return false;
        return n3 < rhs.n3;
    }

    bool operator>(const Cmp &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Cmp &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Cmp &rhs) const {
        return !(*this < rhs);
    }
};

void ConvexHull::Print() {
    int k = 0;
    auto face = f;
    std::vector<Cmp> ans;

    while (face != f || k == 0) {
        Cmp i(face->points[0]->p_number, face->points[1]->p_number, face->points[2]->p_number);

        if (i.n1 < i.n2 && i.n1 < i.n3) {
            ans.emplace_back(i.n1, i.n2, i.n3);
        } else if (i.n2 < i.n1 && i.n2 < i.n3) {
            ans.emplace_back(i.n2, i.n3, i.n1);
        } else {
            ans.emplace_back(i.n3, i.n1, i.n2);
        }
        k++;
        face = face->next;
    }

    std::sort(ans.begin(), ans.end());
    std::cout << k << '\n';

    for (auto i : ans) {
        std::cout << 3 << " ";
        std::cout << i.n1 << " " << i.n2 << " " << i.n3 << "\n";

    }
}

ConvexHull::~ConvexHull() {
    auto temp_p = root->next;
    root->next.reset();
    root->prev.reset();
    root->inc_edges.reset();
    root.reset();
    root = temp_p;
    while (root) {
        temp_p = root->next;
        root->next.reset();
        root->prev.reset();
        root->inc_edges.reset();
        root.reset();
        root = temp_p;
    }

    auto temp_e = e->next;
    e->next.reset();
    e->prev.reset();
    e->points[0].reset();
    e->points[1].reset();
    e->face.reset();
    e->faces[0].reset();
    e->faces[1].reset();
    e = temp_e;
    while (e) {
        temp_e = e->next;
        e->next.reset();
        e->prev.reset();
        e->points[0].reset();
        e->points[1].reset();
        e->face.reset();
        e->faces[0].reset();
        e->faces[1].reset();
        e = temp_e;
    }


    auto temp_f = f->next;
    f->next.reset();
    f->prev.reset();
    f->points[0].reset();
    f->points[1].reset();
    f->points[2].reset();
    f->edges[0].reset();
    f->edges[1].reset();
    f->edges[2].reset();
    f = temp_f;
    while (f) {
        temp_f = f->next;
        f->next.reset();
        f->prev.reset();
        f->points[0].reset();
        f->points[1].reset();
        f->points[2].reset();
        f->edges[0].reset();
        f->edges[1].reset();
        f->edges[2].reset();
        f = temp_f;
    }
}


int main() {
    int n;

    int number_of_test;
    std::cin >> number_of_test;
    for (int i = 0; i < number_of_test; i++) {
        std::cin >> n;

        ConvexHull CH;
        CH.Read(n);
        CH.MakeTetrahedron();
        CH.Build();
        CH.Print();
    }
    return 0;
}
