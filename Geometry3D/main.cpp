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
TScalar get_dot_product(const CVector<TScalar> &v, const CVector<TScalar> &u) {
    return  v.x * u.x + v.y * u.y + v.z * u.z;
}

template<typename TScalar>
CVector<TScalar> get_cross_product(const CVector<TScalar> &a, const CVector<TScalar> &b) {
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
bool CompareAngle(const CVector<TScalar> &v1, const CVector<TScalar> &v2, const TScalar EPS) {
    if (v1.x > EPS && v2.x < -EPS) {
        return true;
    } else if (v1.x < -EPS && v2.x > EPS) {
        return false;
    }
    TScalar k1 = v1.y / v1.x;
    TScalar k2 = v2.y / v2.x;

    if (std::abs(v1.x) < EPS) {
        if (v2.y > EPS) {
            return v2.x < -EPS;
        } else {
            return false;
        }
    }
    if (std::abs(v2.x) < EPS) {
        if (v2.y > EPS) {
            return v1.x > EPS;
        } else {
            return std::abs(k1 - k2) > EPS;
        }
    }
    if (std::abs(k1 - k2) < EPS) {
        return false;
    }
    return k1 < k2;
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
bool Is_Collinear(const CPoint<TScalar> &p1, const CPoint<TScalar> &p2, const CPoint<TScalar> &p3) {
    CVector<TScalar> norm = get_cross_product<TScalar>(p1 - p2, p3 - p2);
    return get_dot_product(norm, norm) == 0;
}


class ConvexHull {
public:
    explicit ConvexHull();

    ~ConvexHull();

    struct CHPoint;
    struct CHEdge;
    struct CHFace;

    struct CHPoint {
        CHPoint(CPoint<int> _p, int _n);

        CPoint<int> point;
        int p_number;
        std::shared_ptr<CHEdge> inc_edges;  //incident edge;
        bool flag; //true if point's on the hull
        bool processed; //true if point's processed
        std::shared_ptr<CHPoint> next, prev;
    };

    struct CHFace {
        CHFace(std::shared_ptr<CHPoint> _p1, std::shared_ptr<CHPoint> _p2, std::shared_ptr<CHPoint> _p3);

        std::array<std::shared_ptr<CHEdge>, 3> edges;
        std::array<std::shared_ptr<CHPoint>, 3> points;
        bool visible;
        std::shared_ptr<CHFace> next, prev;
    };

    struct CHEdge {
        CHEdge(std::shared_ptr<CHPoint> _p1, std::shared_ptr<CHPoint> _p2);

        std::array<std::shared_ptr<CHFace>, 2> faces;
        std::array<std::shared_ptr<CHPoint>, 2> points;
        std::shared_ptr<CHFace> face;
        bool removed; // true if will delete nex time
        std::shared_ptr<CHEdge> next, prev;
    };

    void Read(int _n);

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


};

ConvexHull::CHFace::CHFace(std::shared_ptr<ConvexHull::CHPoint> _p1, std::shared_ptr<ConvexHull::CHPoint> _p2,
                           std::shared_ptr<ConvexHull::CHPoint> _p3) :
        visible(false), next(nullptr), prev(nullptr) {
    edges[0] = edges[1] = edges[2] = nullptr;
    points[0] = std::move(_p1);
    points[1] = std::move(_p3);
    points[2] = std::move(_p2);
}

ConvexHull::CHEdge::CHEdge(std::shared_ptr<ConvexHull::CHPoint> _p1, std::shared_ptr<ConvexHull::CHPoint> _p2) :
        face(nullptr), removed(false), next(nullptr), prev(nullptr) {
    points[0] = std::move(_p1);
    points[1] = std::move(_p2);
    faces[0] = faces[1] = nullptr;
}

ConvexHull::CHPoint::CHPoint(CPoint<int> _p, int _n) : point(_p), p_number(_n), inc_edges(nullptr), flag(false),
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


void ConvexHull::Read(int _n) {
    int x, y, z;
    std::vector<CPoint<int>> v;
    for (int i = 0; i < _n; i++) {
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

    while (Is_Collinear(v->point, v->next->point, v->next->next->point)) {
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

void ConvexHull::update_edges() {
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

void ConvexHull::update_points() {

    auto temp_e = e;
    temp_e->points[0]->flag = temp_e->points[1]->flag = true;
    temp_e = temp_e->next;

    while (temp_e != e) {
        temp_e->points[0]->flag = temp_e->points[1]->flag = true;
        temp_e = temp_e->next;
    }

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

struct Comparator {
    Comparator(int a, int b, int c) : n1(a), n2(b), n3(c) {}

    int n1, n2, n3;

    bool operator<(const Comparator &rhs) const {
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

    bool operator>(const Comparator &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Comparator &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Comparator &rhs) const {
        return !(*this < rhs);
    }
};

void ConvexHull::Print() {
    int k = 0;
    auto face = f;
    std::vector<Comparator> ans;

    while (face != f || k == 0) {
        Comparator item(face->points[0]->p_number, face->points[1]->p_number, face->points[2]->p_number);

        if (item.n1 < item.n2 && item.n1 < item.n3) {
            ans.emplace_back(item.n1, item.n2, item.n3);
        } else if (item.n2 < item.n1 && item.n2 < item.n3) {
            ans.emplace_back(item.n2, item.n3, item.n1);
        } else {
            ans.emplace_back(item.n3, item.n1, item.n2);
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

template<typename TScalar>
struct CPolygon {
public:
    explicit CPolygon(const std::list<CVector<TScalar>> &_points) {
        points = std::vector<CVector<TScalar>>(std::begin(_points), std::end(_points));
    }

    bool operator&(CPolygon<TScalar> &polygon);

    std::vector<CVector<TScalar>> points;

    bool InPolygon(const CPoint<TScalar> &p, TScalar EPS);
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
        } else if (CompareAngle<long double>(polygon.points[j + 1] - polygon.points[j], points[i + 1] - points[i],
                                             1e-15)) {
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


int main() {
    int n, m;
    std::cin >> n;
    std::list<CVector<long double>> p1, p2;
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
    p1 = std::move(temp1);
    minPoint = CVector<long double>(INT32_MAX, INT32_MAX, 0);
    std::cin >> m;
    for (int i = 0; i < m; i++) {
        std::cin >> x >> y;
        CVector<long double> p(-x, -y, 0);
        minPoint = std::min(minPoint, p);
        p2.emplace_back(-x, -y, 0);
    }
    p2.reverse();
    it = p2.begin();
    while (*it != minPoint) { ++it; }
    std::list<CVector<long double>> temp3(it, p2.end()), temp4(p2.begin(), it);
    temp3.splice(temp3.end(), temp4);
    p2 = std::move(temp3);


    CPolygon<long double> poly1(p1), poly2(p2);
    if (poly1 & poly2) {
        std::cout << "YES";
    } else {
        std::cout << "NO";
    }

    return 0;
}