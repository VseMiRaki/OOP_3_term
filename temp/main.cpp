#include <utility>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <set>
#include <algorithm>
#include <memory>
using namespace std;
static const double EPS = 1e-15;
template<typename TSC>
struct CV {
    explicit CV(TSC x = 0, TSC y = 0, TSC z = 0) : x(x), y(y), z(z) {}
    TSC x;
    TSC y;
    TSC z;
    CV operator*(const TSC &k) const { return CV(k * x, k * y, k * z); }
    CV operator/(const TSC &k) const { return CV(x / k, y / k, z / k); }
    CV operator+(const CV &v) const { return CV(this->x + v.x, this->y + v.y, this->z + v.z); }
    CV operator-(const CV &v) const { return CV(this->x - v.x, this->y - v.y, this->z - v.z); }
    CV operator*(const CV &v) const;
    bool operator==(const CV &rhs) const;
    bool operator!=(const CV &rhs) const;
    TSC Length() const;
};
template<typename TSC>
using CP = CV<TSC>;
template<typename TSC>
TSC Dot_product(const CV<TSC> &v, const CV<TSC> &u) {
    TSC ans = v.x * u.x + v.y * u.y + v.z * u.z;
    return ans;
}
template<typename TSC>
CV<TSC> Cross_product(const CV<TSC> &a, const CV<TSC> &b) {
    TSC i = a.y * b.z - a.z * b.y;
    TSC j = a.z * b.x - a.x * b.z;
    TSC k = a.x * b.y - a.y * b.x;
    return CV<TSC>(i, j, k);
}
template<typename TSC>
TSC CV<TSC>::Length() const {
    CV<TSC> v = *this;
    return sqrt(Dot_product(v, v));
}
template<typename TSC>
CV<TSC> CV<TSC>::operator*(const CV &v) const {
    return Cross_product<TSC>(*this, v);
}
template<typename TSC>
bool CV<TSC>::operator==(const CV &rhs) const {
    return x == rhs.x &&
           y == rhs.y &&
           z == rhs.z;
}
template<typename TSC>
bool CV<TSC>::operator!=(const CV &rhs) const {
    return !(rhs == *this);
}
template<typename TSC>
TSC Volume(const CV<TSC> &v1, const CV<TSC> &v2, const CV<TSC> &v3) {
    return Dot_product(v1, v2 * v3);
}
template <typename TSC>
TSC Volume(const CV<TSC>&v1,const CV<TSC>&v2,const CV<TSC>&v3,const CV<TSC>&v4){
TSC x24=v2.x-v4.x;
TSC y24=v2.y-v4.y;
TSC z24=v2.z-v4.z;
TSC x34=v3.x-v4.x;
TSC y34=v3.y-v3.y;
TSC z34=v3.z-v4.z;
return(v1.z-v4.z)*(x24*y34-y24*x34)+
(v1.y-v4.y)*(z24*x34-x24*z34)+
(v1.x-v4.z)*(y24*z34-z24*y34);
}
template<typename TSC>
bool Collinear3Points(const CP<TSC> &p1, const CP<TSC> &p2, const CP<TSC> &p3) {
    CV<TSC> norm = Cross_product<TSC>(p1 - p2, p3 - p2);
    return Dot_product(norm, norm) == 0;
}
class CoH {
public:
explicit CoH();

struct CHP;
struct CHE;
struct CHF;
struct CHP{
CHP(CP<int> p,int n);
CP<int>point;
int pn;
shared_ptr<CHE>inc_edges;//incidentedges;
bool flag;//trueifpoint'sonthehull
bool processed;//trueifpoint'sprocessed
shared_ptr<CHP>next,prev;
};
struct CHF{
CHF(shared_ptr<CHP>p1,shared_ptr<CHP>p2,shared_ptr<CHP>p3);
array<shared_ptr<CHE>,3>edges;
array<shared_ptr<CHP>,3>points;
bool visible;
shared_ptr<CHF>next,prev;
};
struct CHE{
CHE(shared_ptr<CHP>p1,shared_ptr<CHP>p2);
array<shared_ptr<CHF>,2>faces;
array<shared_ptr<CHP>,2>points;
shared_ptr<CHF>face;
bool removed;//trueifwilldeletenextime
shared_ptr<CHE>next,prev;
};
void Read(int n);
void Build();
void MakeTetrahedron();
void Print();
private:
    shared_ptr<CHP> root;
    shared_ptr<CHE> e;
    shared_ptr<CHF> f;
    void update_edges();
    void update_points();
    void update_faces();
    void update();
    void Correcting(shared_ptr<CHF> &_f, shared_ptr<CHE> &_e, shared_ptr<CHP> &_p);
    shared_ptr<CHF>
    MakeFace(shared_ptr<CoH::CHE> &_e, shared_ptr<CoH::CHP> &_p);
    void Add(shared_ptr<CHP> &_root, shared_ptr<CHP> &_new);
    void Add(shared_ptr<CHF> &_root, shared_ptr<CHF> &_new);
    void Add(shared_ptr<CHE> &_root, shared_ptr<CHE> &_new);
    void Delete(shared_ptr<CHP> &_root, shared_ptr<CHP> &_new);
    void Delete(shared_ptr<CHF> &_root, shared_ptr<CHF> &_new);
    void Delete(shared_ptr<CHE> &_root, shared_ptr<CHE> &_new);
    bool AddingPoint(shared_ptr<CHP> &_p);
};
CoH::CHF::CHF(shared_ptr<CoH::CHP> p1, shared_ptr<CoH::CHP> p2,
                           shared_ptr<CoH::CHP> p3) :
        visible(false), next(nullptr), prev(nullptr) {
    edges[0] = edges[1] = edges[2] = nullptr;
    points[0] = move(p1);
    points[1] = move(p3);
    points[2] = move(p2);
}
CoH::CHE::CHE(shared_ptr<CoH::CHP> p1, shared_ptr<CoH::CHP> p2) :
        face(nullptr), removed(false), next(nullptr), prev(nullptr) {
    points[0] = move(p1);
    points[1] = move(p2);
    faces[0] = faces[1] = nullptr;
}
CoH::CHP::CHP(CP<int> p, int n) : point(p), pn(n), inc_edges(nullptr), flag(false),
                                                     processed(false), next(nullptr), prev(nullptr) {}
void CoH::Add(shared_ptr<CHP> &_root, shared_ptr<CHP> &_new) {
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
void CoH::Add(shared_ptr<CHF> &_root, shared_ptr<CHF> &_new) {
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
void CoH::Add(shared_ptr<CHE> &_root, shared_ptr<CHE> &_new) {
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
void CoH::Read(int n) {
    int x, y, z;
    for (int i = 0; i < n; i++) {
        cin >> x >> y >> z;
        CP<int> p(x, y, z);
        auto new_point_ptr = make_shared<CHP>(p, i);
        Add(root, new_point_ptr);
    }
}
void CoH::MakeTetrahedron() {
    shared_ptr<CHP> v = root;
    auto first_v = v;
    while (Collinear3Points(v->point, v->next->point, v->next->next->point)) {
        v = v->next;
        if (first_v == v) {
            //do smth
        }
    }
    v->processed = v->next->processed = v->next->next->processed = true;
    auto e1_ptr = make_shared<CHE>(v, v->next);
    auto e2_ptr = make_shared<CHE>(v->next, v->next->next);
    auto e3_ptr = make_shared<CHE>(v->next->next, v);
    Add(e, e1_ptr);
    Add(e, e2_ptr);
    Add(e, e3_ptr);
    auto nf_ptr = make_shared<CHF>(v, v->next, v->next->next);
    nf_ptr->edges[0] = e1_ptr;
    nf_ptr->edges[1] = e2_ptr;
    nf_ptr->edges[2] = e3_ptr;
    Add(f, nf_ptr);
    e1_ptr->faces[0] = e2_ptr->faces[0] = e3_ptr->faces[0] = nf_ptr;
    auto next_v = v->next->next->next;
    while (Volume<int>(v->next->point, v->next->next->point, next_v->point, v->point) == 0) {
        next_v = next_v->next;
        if (next_v == v) {
        }
    }
    next_v->processed = true;
    if (Volume<int>(v->next->point, v->next->next->point, next_v->point, v->point) < 0) {
        swap(f->points[1], f->points[2]);
        swap(f->edges[1], f->edges[2]);
    }
    e1_ptr->faces[1] = MakeFace(e1_ptr, next_v);
    e2_ptr->faces[1] = MakeFace(e2_ptr, next_v);
    e3_ptr->faces[1] = MakeFace(e3_ptr, next_v);
    update();
}
shared_ptr<CoH::CHF>
CoH::MakeFace(shared_ptr<CoH::CHE> &_e, shared_ptr<CoH::CHP> &_p) {
    auto new_f_ptr = make_shared<CHF>(CHF(nullptr, nullptr, nullptr));
    vector<shared_ptr<CHE>> new_edges(2, nullptr);
    for (int i = 0; i < 2; i++) {
        new_edges[i] = _e->points[i]->inc_edges;
        if (!new_edges[i]) {
            new_edges[i] = make_shared<CHE>(CHE(nullptr, nullptr));
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
void CoH::Correcting(shared_ptr<CoH::CHF> &_f, shared_ptr<CoH::CHE> &_e,
                            shared_ptr<CHP> &_p) {
    auto temp_f = make_shared<CHF>(CHF(nullptr, nullptr, nullptr));
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
void CoH::Build() {
    auto _v = root;
    auto _vnext = root;
    _vnext = _v->next;
    if (!_v->processed) {
        _v->flag = true;
        AddingPoint(_v);
        update();
    }
    _v = _vnext;
    while (_v != root) {
        _vnext = _v->next;
        if (!_v->processed) {
            _v->processed = true;
            AddingPoint(_v);
            update();
        }
        _v = _vnext;
    }
}
bool CoH::AddingPoint(shared_ptr<CoH::CHP> &_p) {
    auto _f = f;
    bool curVis = false;
    if (Volume(_f->points[0]->point, _f->points[1]->point,
               _f->points[2]->point, _p->point) < 0) {
        _f->visible = true;
        curVis = true;
    }
    _f = _f->next;
    while (_f != f) {
        if (Volume(_f->points[0]->point, _f->points[1]->point,
                   _f->points[2]->point, _p->point) < 0) {
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
        _e->removed = true;
    } else if (_e->faces[0]->visible || _e->faces[1]->visible) {
        _e->face = MakeFace(_e, _p);
    }
    _e = temp;
    while (_e != e) {
        temp = _e->next;
        if (_e->faces[0]->visible && _e->faces[1]->visible) {
            _e->removed = true;
        } else if (_e->faces[0]->visible || _e->faces[1]->visible) {
            _e->face = MakeFace(_e, _p);
        }
        _e = temp;
    }
    return true;
}
CoH::CoH() : e(nullptr), root(nullptr), f(nullptr) {
}
void CoH::update() {
    update_edges();
    update_faces();
    update_points();
}
void CoH::update_edges() {
    auto temp_e = e;
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
    shared_ptr<CHE> t = nullptr;
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
void CoH::Delete(shared_ptr<CoH::CHE> &_root, shared_ptr<CoH::CHE> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new.reset();
    }
}
void CoH::Delete(shared_ptr<CoH::CHP> &_root, shared_ptr<CoH::CHP> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new.reset();
    }
}
void CoH::Delete(shared_ptr<CoH::CHF> &_root, shared_ptr<CoH::CHF> &_new) {
    if (_root) {
        if (_root == _root->next) {
            _root = nullptr;
        } else if (_new == _root) {
            _root = _root->next;
        }
        _new->next->prev = _new->prev;
        _new->prev->next = _new->next;
        _new.reset();
    }
}
void CoH::update_faces() {
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
void CoH::update_points() {
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
    }
    else {
        temp_p = temp_p->next;
    }
    while (temp_p != root) {
        if (temp_p->processed && !temp_p->flag) {
            t = temp_p;
            temp_p = temp_p->next;
            Delete(root, t);
        }
        else {
            temp_p = temp_p->next;
        }
    }
    temp_p = root;
    temp_p->inc_edges = nullptr;
    temp_p->flag = false;
    temp_p = temp_p->next;
    while(temp_p != root) {
        temp_p->inc_edges = nullptr;
        temp_p->flag = false;
        temp_p = temp_p->next;
    }
}
struct cmp {
    cmp(int a, int b, int c) : n1(a), n2(b), n3(c) {}
    int n1, n2, n3;
    bool operator<(const cmp &rhs) const {
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
};
void CoH::Print() {
    int k = 0;
    auto face = f;
    vector<cmp> ans;
    while (face != f || k == 0) {
        cmp i(face->points[0]->pn, face->points[1]->pn, face->points[2]->pn);
        if ( i.n1 < i.n2 && i.n1 < i.n3) {
            ans.emplace_back(i.n1, i.n2, i.n3);
        }
        else if (i.n2 < i.n1 && i.n2 < i.n3) {
            ans.emplace_back(i.n2, i.n3, i.n1);
        }
        else {
            ans.emplace_back(i.n3, i.n1, i.n2);
        }
        k++;
        face = face->next;
    }
    sort(ans.begin(), ans.end());
    cout << k << '\n';
    for (auto i : ans) {
        cout << 3 << " ";
        cout << i.n1 << " " << i.n2 << " " << i.n3 << "\n";
    }
}
int main() {
    int n;
    int number_of_test;
    cin >> number_of_test;
    for (int i = 0; i < number_of_test; i++) {
        cin >> n;
        CoH CH;
        CH.Read(n);
        CH.MakeTetrahedron();
        CH.Build();
        CH.Print();

    }
    return 0;
}