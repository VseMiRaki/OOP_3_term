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
#include <queue>
#include <utility>

//#include "Generator.hpp"

const long double EPS = 1e-15;

template<typename TScalar>
class VoronoiDiagram;


template<typename TScalar>
struct CVector {
    explicit CVector(TScalar x = 0, TScalar y = 0) : x(x), y(y) {}

    TScalar x;
    TScalar y;

    CVector operator*(const TScalar &k) const { return CVector(k * x, k * y); }

    CVector operator/(const TScalar &k) const { return CVector(x / k, y / k); }

    CVector operator+(const CVector &v) const { return CVector(this->x + v.x, this->y + v.y); }

    CVector operator-(const CVector &v) const { return CVector(this->x - v.x, this->y - v.y); }

    bool operator<(const CVector &rhs) const;

    bool operator>(const CVector &rhs) const;

    bool operator<=(const CVector &rhs) const;

    bool operator>=(const CVector &rhs) const;

    bool operator==(const CVector<TScalar> &_other);

    bool operator!=(const CVector<TScalar> &_other) { return !(*this == _other); }

    TScalar getLength();

    CVector<TScalar> norm() { return (*this) / this->getLength(); }
};

template<typename TScalar>
TScalar getArea(const CVector<TScalar> p1, const CVector<TScalar> p2) {
    return p1.x * p2.y - p1.y * p2.x;
}

template<typename TScalar>
using CPoint = CVector<TScalar>;

template<typename TScalar>
TScalar getDotProduct(const CVector<TScalar> &_p1, const CVector<TScalar> &_p2) {
    return _p1.x * _p2.x + _p1.y * _p2.y;
}

template<typename TScalar>
TScalar CVector<TScalar>::getLength() {
    return std::sqrt(getDotProduct(*this, *this));
}

template<typename TScalar>
bool CVector<TScalar>::operator==(const CVector<TScalar> &_other) {
    return (std::abs(this->x - _other.x) < EPS && std::abs(this->y - _other.y) < EPS);
}

template<typename TScalar>
bool CVector<TScalar>::operator<(const CVector &rhs) const {
    if (x < rhs.x)
        return true;
    if (rhs.x < x)
        return false;
    return y < rhs.y;
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
using Site = CPoint<TScalar>;

template<typename TScalar>
std::vector<CVector<TScalar>>
getIntersectionOfTwoParabolas(const CVector<TScalar> &p1, const CVector<TScalar> &p2, TScalar dir) {
    std::vector<CVector<TScalar>> ans;
    if (std::abs(p1.x - p2.x) < EPS) {
        TScalar y = (p1.y + p2.y) / 2, D = std::sqrt(p1.y * p2.y + dir * dir - (p1.y + p2.y) * dir);
        ans.emplace_back(p1.x - D, y);
        ans.emplace_back(p1.x + D, y);
    } else if (std::abs(p1.y - p2.y) < EPS) {
        TScalar x = (p1.x + p2.x) / 2, D = (p1.y * p1.y - dir * dir + (x - p1.x) * (x - p1.x)) / (p1.y - dir) / 2;
        ans.emplace_back(x, D);
    } else {
        TScalar D = 2 * std::sqrt(
                pow(p1.x - p2.x, 2) * (dir - p1.y) * (dir - p2.y) * (pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
        TScalar T = -2 * dir * pow(p1.x - p2.x, 2) + (p1.y + p2.y) * (pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
        TScalar Q = 2 * pow(p1.y - p2.y, 2);

        TScalar y1 = (T - D) / Q, y2 = (T + D) / Q;
        TScalar x1 = (p1.x * p1.x - p2.x * p2.x + (2 * y1 - p2.y - p1.y) * (p2.y - p1.y)) / (p1.x - p2.x) / 2;
        TScalar x2 = (p1.x * p1.x - p2.x * p2.x + (2 * y2 - p2.y - p1.y) * (p2.y - p1.y)) / (p1.x - p2.x) / 2;

        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        ans.emplace_back(x1, y1);
        ans.emplace_back(x2, y2);
    }
    return ans;
}

template<typename TScalar>
struct Circle {
    Circle(const CPoint<TScalar> &_centre, const CPoint<TScalar> &_bottom) : centre(_centre), bottomPoint(_bottom) {}

    Circle(const CPoint<TScalar> &_p1, const CPoint<TScalar> &_p2, const CPoint<TScalar> &_p3);

    CPoint<TScalar> centre, bottomPoint;
};

template<typename TScalar>
Circle<TScalar>::Circle(const CPoint<TScalar> &_p1, const CPoint<TScalar> &_p2, const CPoint<TScalar> &_p3) {
    CPoint<TScalar> n1 = (_p1 - _p2).norm(), n2 = (_p3 - _p2).norm();
    TScalar area = getArea<TScalar>(n1, n2);
    CVector<TScalar> mid1 = (_p1 + _p2) / 2, mid2 = (_p2 + _p3) / 2;
    centre = CVector<TScalar>(
            (getDotProduct(n1, mid1) * n2.y - getDotProduct(n2, mid2) * n1.y) / area,
            (getDotProduct(n2, mid2) * n1.x - getDotProduct(n1, mid1) * n2.x) / area);
    bottomPoint = CVector<TScalar>(centre.x, centre.y + (_p1 - centre).getLength());
}

//DCEL

//begin

template<typename TScalar>
struct HalfEdge;

template<typename TScalar>
struct Vertex {
    explicit Vertex(const CPoint<TScalar> &point, const std::shared_ptr<HalfEdge<TScalar>> &edge = nullptr) : point(
            point), edge(edge) {}

    CPoint<TScalar> point;
    std::shared_ptr<HalfEdge<TScalar>> edge;
};

template<typename TScalar>
struct HalfEdge {
    std::shared_ptr<Vertex<TScalar>> point;

    HalfEdge(int begin, int end, const std::shared_ptr<Vertex<TScalar>> &point = nullptr) : point(point), begin(begin),
                                                                                            end(end),
                                                                                            twin(nullptr),
                                                                                            next(nullptr),
                                                                                            prev(nullptr),
                                                                                            used(false) {}


    ~HalfEdge() {
        if (twin) twin.reset();
        if (next) next.reset();
        if (prev) prev.reset();
    }


    int begin, end;
    std::shared_ptr<HalfEdge<TScalar>> twin, next, prev;
    

    bool used;
};

template<typename TScalar>
std::pair<std::shared_ptr<HalfEdge<TScalar>>, std::shared_ptr<HalfEdge<TScalar>>> getTwins(int _begin, int _end) {
    std::shared_ptr<HalfEdge<TScalar>> e = std::make_shared<HalfEdge<TScalar>>(_begin, _end);
    std::shared_ptr<HalfEdge<TScalar>> revE = std::make_shared<HalfEdge<TScalar>>(_end, _begin);
    e->twin = revE;
    revE->twin = e;
    return {e, revE};
}

template<typename TScalar>
void Join(std::shared_ptr<HalfEdge<TScalar>> &p1, std::shared_ptr<HalfEdge<TScalar>> &p2) {
    p1->next = p2;
    p2->prev = p1;
}

//end

template<typename TScalar>
struct BeachLineNode;

template<typename TScalar>
class Event {
public:
    explicit Event(const CPoint<TScalar> &_p);

    bool operator<(const Event &rhs) const {
        return CPoint<TScalar>(site.y, site.x) < CPoint<TScalar>(rhs.site.y, rhs.site.x);
    }

    bool operator>(const Event &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Event &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Event &rhs) const {
        return !(*this < rhs);
    }

    virtual void HandleEvent(VoronoiDiagram<TScalar> *_voronoiDiagram) = 0;

    Site<TScalar> site;
};

template<typename TScalar>
Event<TScalar>::Event(const Site<TScalar> &_p) : site(_p) {
}

template<typename TScalar>
class CircleEvent : public Event<TScalar> {
public:
    CircleEvent(const Site<TScalar> &_site, const Circle<TScalar> &_circle,
                const std::shared_ptr<BeachLineNode<TScalar>> &_arc);

    ~CircleEvent() { if (arc) { arc.reset(); }}

    void HandleEvent(VoronoiDiagram<TScalar> *_voronoiDiagram) override;

private:
    Circle<TScalar> circle;
    std::shared_ptr<BeachLineNode<TScalar>> arc;
};

template<typename TScalar>
CircleEvent<TScalar>::CircleEvent(const Site<TScalar> &_site, const Circle<TScalar> &_circle,
                                  const std::shared_ptr<BeachLineNode<TScalar>> &_arc) :
        circle(_circle), arc(_arc), Event<TScalar>(_site) {}


template<typename TScalar>
class SiteEvent : public Event<TScalar> {
public:
    SiteEvent(const Site<TScalar> &_p, int index) : Event<TScalar>(_p), index(index) {}

    void HandleEvent(VoronoiDiagram<TScalar> *_voronoiDiagram) override;

    int GetIndex() { return index; }

private:
    int index;
};

template<typename TScalar>
struct BeachLineNode {

    explicit BeachLineNode(const std::pair<int, int> &_indices,
                           TScalar *_sweepline = nullptr,
                           const std::vector<Site<TScalar>> &_points = nullptr);

    ~BeachLineNode();

    int hight;
    TScalar *sweepLine;
    const std::vector<Site<TScalar>> &points;
    std::pair<int, int> indices;

    std::shared_ptr<BeachLineNode<TScalar>> left, right;
    std::shared_ptr<BeachLineNode<TScalar>> parent;

    std::shared_ptr<CircleEvent<TScalar>> circleEvent;
    std::shared_ptr<HalfEdge<TScalar>> edge;

    std::shared_ptr<BeachLineNode<TScalar>> next, prev;


    TScalar getValue();

};


template<typename TScalar>
void JoinLeaves(std::shared_ptr<BeachLineNode<TScalar>> arc1,
                std::shared_ptr<BeachLineNode<TScalar>> arc2) {
    arc1->next = arc2;
    arc2->prev = arc1;
}

template<typename TScalar>
void UpdateHight(const std::shared_ptr<BeachLineNode<TScalar>> _root) {
    if (_root) {
        _root->hight = std::max(getHight(_root->left), getHight(_root->right)) + 1;
    }
}


template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> Find(std::shared_ptr<BeachLineNode<TScalar>> _root, TScalar _x) {
    if (!_root) return nullptr;
    auto newNode = _root;
    while (newNode->indices.first != newNode->indices.second) {
        if (newNode->getValue() >= _x) {
            newNode = newNode->left;
        } else {
            newNode = newNode->right;
        }
    }
    return newNode;
}

template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> DoLeftRotate(std::shared_ptr<BeachLineNode<TScalar>> _node) {
    if (!_node || !_node->right) {
        return nullptr;
    }
    std::shared_ptr<BeachLineNode<TScalar>> right = _node->right;
    if (_node->parent) {
        if (_node == _node->parent->left) {
            _node->parent->left = right;
        } else {
            _node->parent->right = right;
        }
    }
    right->parent = _node->parent;

    _node->right = right->left;
    if (right->left) {
        right->left->parent = _node;
    }
    right->left = _node;
    _node->parent = right;
    UpdateHight(_node);
    UpdateHight(right);
    UpdateHight(right->parent);
    return right;
}

template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> DoRightRotate(std::shared_ptr<BeachLineNode<TScalar>> _node) {
    if (!_node || !_node->left) {
        return nullptr;
    }
    std::shared_ptr<BeachLineNode<TScalar>> left = _node->left;
    if (_node->parent) {
        if (_node == _node->parent->left) {
            _node->parent->left = left;
        } else {
            _node->parent->right = left;
        }
    }
    left->parent = _node->parent;
    _node->left = left->right;
    if (left->right) {
        left->right->parent = _node;
    }
    left->right = _node;
    _node->parent = left;
    UpdateHight(_node);
    UpdateHight(left);
    UpdateHight(left->parent);
    return left;
}

template<typename TScalar>
TScalar getHight(const std::shared_ptr<BeachLineNode<TScalar>> &_node) {
    return _node ? _node->hight : 0;
}

template<typename TScalar>
TScalar getBalance(const std::shared_ptr<BeachLineNode<TScalar>> &_node) {
    return getHight(_node->left) - getHight(_node->right);
}


template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> Swap(std::shared_ptr<BeachLineNode<TScalar>> _node1,
                                             std::shared_ptr<BeachLineNode<TScalar>> _node2) {
    if (!_node1) {
        return _node2;
    }
    TScalar separator = _node2->getValue();

    std::shared_ptr<BeachLineNode<TScalar>> parent = _node1->parent;
    _node2->parent = parent;
    if (parent) {
        parent->getValue() < separator ? parent->right = _node2 : parent->left = _node2;
    }
    _node1 = _node2;


    while (parent) {
        UpdateHight(parent);
        int balance = getHight(parent->left) - getHight(parent->right);
        if (balance > 1) {
            if (parent->left && parent->left->indices.first != parent->left->indices.second &&
                getBalance(parent->left) < 0) {
                parent->left = DoLeftRotate(parent->left);
            }
            parent = DoRightRotate(parent);
        } else if (balance < -1) {
            if (parent->right && parent->right->indices.first != parent->left->indices.second &&
                getBalance(parent->right) > 0) {
                parent->right = DoRightRotate(parent->right);
            }
            parent = DoLeftRotate(parent);
        }
        _node1 = parent;
        parent = parent->parent;
    }


    return _node1;
}

template<typename TScalar>
BeachLineNode<TScalar>::BeachLineNode(const std::pair<int, int> &_indices,
                                      TScalar *_sweepline,
                                      const std::vector<Site<TScalar>> &_points):
        indices(_indices), sweepLine(_sweepline),
        points(_points), left(nullptr), right(nullptr),
        hight(1), parent(nullptr) {}

template<typename TScalar>
TScalar BeachLineNode<TScalar>::getValue() {
    if (points.empty()) { return std::numeric_limits<TScalar>::infinity(); }
    if (indices.first == indices.second) { return points[indices.first].x; }
    CPoint<TScalar> p1 = points[indices.first], p2 = points[indices.second];
    std::vector<CPoint<TScalar>> interPoints = getIntersectionOfTwoParabolas(p1, p2, *sweepLine);
    if (interPoints.size() != 2) {
        return interPoints[0].x;
    }
    if (p1.y >= p2.y) {
        return interPoints[1].x;
    }
    return interPoints[0].x;

}

template<typename TScalar>
BeachLineNode<TScalar>::~BeachLineNode() {
    if (this->parent)
        this->parent.reset();
    if (this->left)
        this->left.reset();
    if (this->right)
        this->right.reset();
    if (this->prev)
        this->prev.reset();
    if (this->next)
        this->next.reset();
    if (this->circleEvent)
        this->circleEvent.reset();
    if (this->edge)
        this->edge.reset();
}

template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> MakeTreeForSingleIntersection(int _index, int _prevIndex, TScalar *_sweepline,
                                                                      const std::vector<CPoint<TScalar>> &_points,
                                                                      std::vector<std::shared_ptr<HalfEdge<TScalar>>> *_edges) {
    std::shared_ptr<BeachLineNode<TScalar>> root, left, right;
    std::pair<std::shared_ptr<HalfEdge<TScalar>>, std::shared_ptr<HalfEdge<TScalar>>> twinEdges = getTwins<TScalar>(
            _prevIndex, _index);

    _edges->push_back(twinEdges.first);
    _edges->push_back(twinEdges.second);
    if (_points[_index].x < _points[_prevIndex].x) {
        std::pair<int, int> temp = {_index, _prevIndex};
        root = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        temp = {_index, _index};
        left = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        temp = {_prevIndex, _prevIndex};
        right = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        root->edge = twinEdges.second;
    } else {
        std::pair<int, int> temp = {_prevIndex, _index};
        root = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        temp = {_index, _index};
        right = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        temp = {_prevIndex, _prevIndex};
        left = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
        root->edge = twinEdges.first;
    }
    root->left = left;
    root->right = right;
    right->parent = root;
    left->parent = root;
    JoinLeaves(left, right);
    UpdateHight(root);
    return root;
}

template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> MakeTreeForDoubleIntersection(int _index, int _prevIndex, TScalar *_sweepline,
                                                                      const std::vector<CPoint<TScalar>> &_points,
                                                                      std::vector<std::shared_ptr<HalfEdge<TScalar>>> *_edges) {

    std::pair<int, int> temp = {_prevIndex, _index};
    std::shared_ptr<BeachLineNode<TScalar>> node1 = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
    temp = {_index, _prevIndex};
    std::shared_ptr<BeachLineNode<TScalar>> node2 = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline, _points);
    temp = {_prevIndex, _prevIndex};
    std::shared_ptr<BeachLineNode<TScalar>> leftLeaf = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline,
                                                                                                _points);
    temp = {_index, _index};
    std::shared_ptr<BeachLineNode<TScalar>> midLeaf = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline,
                                                                                               _points);
    temp = {_prevIndex, _prevIndex};
    std::shared_ptr<BeachLineNode<TScalar>> rightLeaf = std::make_shared<BeachLineNode<TScalar>>(temp, _sweepline,
                                                                                                 _points);

    //        node1
    //       /     \
    //   leftleaf   node2
    //              /   \
    //         midleaf rightleaf

    node1->right = node2;
    node2->parent = node1;
    node1->left = leftLeaf;
    leftLeaf->parent = node1;
    node2->left = midLeaf;
    midLeaf->parent = node2;
    node2->right = rightLeaf;
    rightLeaf->parent = node2;
    std::pair<std::shared_ptr<HalfEdge<TScalar>>, std::shared_ptr<HalfEdge<TScalar>>> twinEdges = getTwins<TScalar>(
            _prevIndex, _index);
    node1->edge = twinEdges.first;
    node2->edge = twinEdges.second;
    _edges->push_back(twinEdges.first);
    _edges->push_back(twinEdges.second);
    JoinLeaves(leftLeaf, midLeaf);
    JoinLeaves(midLeaf, rightLeaf);
    UpdateHight(node2);
    UpdateHight(node1);
    return node1;
}

template<typename TScalar>
std::pair<std::shared_ptr<BeachLineNode<TScalar>>, std::shared_ptr<BeachLineNode<TScalar>>>
getInterPoints(std::shared_ptr<BeachLineNode<TScalar>> arc) {
    if (!arc || !arc->next || !arc->next) {
        return {nullptr, nullptr};
    }
    std::shared_ptr<BeachLineNode<TScalar>> parent = arc->parent, gparent = arc->parent;
    std::pair<int, int> FirstPairOfIntPoints(arc->prev->indices.first, arc->indices.first);
    std::pair<int, int> SecondPairOfIntPoints(arc->indices.first, arc->next->indices.first);
    std::pair<int, int> IntPoints;
    bool flag = true;
    if (parent->indices == FirstPairOfIntPoints) {
        IntPoints = SecondPairOfIntPoints;
        flag = false;
    } else if (parent->indices == SecondPairOfIntPoints) {
        IntPoints = FirstPairOfIntPoints;
        flag = true;
    }
    while (gparent) {
        if (gparent->indices == IntPoints) { break; }
        gparent = gparent->parent;
    }
    if (flag) {
        return {gparent, parent};
    }
    return {parent, gparent};
}

template<typename TScalar>
std::shared_ptr<BeachLineNode<TScalar>> Remove(std::shared_ptr<BeachLineNode<TScalar>> node) {
    if (!node) {
        return nullptr;
    }
    std::shared_ptr<BeachLineNode<TScalar>> nodeParent = node->parent, gparent = nodeParent->parent;
    std::pair<int, int> FirstIntPoints(node->prev->indices.first, node->indices.first);
    std::pair<int, int> SecondIntPoints(node->indices.first, node->next->indices.first), IntPoints;

    if (nodeParent->indices == FirstIntPoints) {
        IntPoints = SecondIntPoints;
    } else if (nodeParent->indices == SecondIntPoints) {
        IntPoints = FirstIntPoints;
    }

    std::shared_ptr<BeachLineNode<TScalar>> subTree;
    if (nodeParent->left == node) {
        subTree = nodeParent->right;
    } else {
        subTree = nodeParent->left;
    }

    subTree->parent = gparent;
    if (gparent->left == nodeParent) {
        gparent->left = subTree;
    } else {
        gparent->right = subTree;
    }
    std::shared_ptr<BeachLineNode<TScalar>> newNode = gparent;

    while (gparent) {
        if (gparent->indices == IntPoints) {
            gparent->indices = {node->prev->indices.first, node->next->indices.first};
        }
        UpdateHight(gparent);
        int balance = getBalance(gparent);
        if (balance > 1) {
            if (gparent->left && gparent->left->indices.first != gparent->left->indices.second &&
                getBalance(gparent->left) < 0) {
                gparent->left = DoLeftRotate(gparent->left);
            }
            gparent = DoRightRotate(gparent);
        } else if (balance < -1) {
            if (gparent->right && gparent->right->indices.first != gparent->right->indices.second &&
                getBalance(gparent->right) < 0) {
                gparent->right = DoRightRotate(gparent->right);
            }
            gparent = DoLeftRotate(gparent);
        }
        newNode = gparent;
        gparent = gparent->parent;
    }
    JoinLeaves(node->prev, node->next);
    return newNode;
}

template<typename TScalar>
struct Comparator {
    bool operator()(const std::shared_ptr<Event<TScalar>> &e1, const std::shared_ptr<Event<TScalar>> &e2) {
        return (*e2.get()) < (*e1.get());
    }
};


template<typename TScalar>
class VoronoiDiagram {
public:
    explicit VoronoiDiagram(std::vector<CPoint<TScalar>> &_points);

    TScalar getAnswer();

    void Build();

    std::shared_ptr<BeachLineNode<TScalar>> &GetRoot() { return beachLineRoot; }

    std::priority_queue<std::shared_ptr<Event<TScalar>>, std::vector<std::shared_ptr<Event<TScalar>>>, Comparator<TScalar>> &
    GetEvents() { return queueEvent; }

    std::vector<std::shared_ptr<HalfEdge<TScalar>>> *
    GetEdges() { return &halfEdges; }

    std::vector<CPoint<TScalar>> &GetPoints() { return points; }

    std::vector<std::shared_ptr<HalfEdge<TScalar>>> &GetFaces() { return faces; }

    std::vector<std::shared_ptr<Vertex<TScalar>>> &GetVertices() { return vertices; }


private:
    std::priority_queue<std::shared_ptr<Event<TScalar>>, std::vector<std::shared_ptr<Event<TScalar>>>, Comparator<TScalar>> queueEvent;
    std::vector<CPoint<TScalar>> points;
    TScalar sweepLine;
    std::shared_ptr<BeachLineNode<TScalar>> beachLineRoot;
    std::vector<std::shared_ptr<HalfEdge<TScalar>>> halfEdges;
    std::vector<std::shared_ptr<Vertex<TScalar>>> vertices;
    std::vector<std::shared_ptr<HalfEdge<TScalar>>> faces;

    int getPolygonStats(std::shared_ptr<HalfEdge<TScalar>> _start);

};

template<typename TScalar>
std::shared_ptr<CircleEvent<TScalar>> makeCircleEvent(std::shared_ptr<BeachLineNode<TScalar>> leftArc,
                                                      std::shared_ptr<BeachLineNode<TScalar>> midArc,
                                                      std::shared_ptr<BeachLineNode<TScalar>> rightArc,
                                                      const std::vector<CVector<TScalar>> &points,
                                                      TScalar *sweepLine) {
    if (!leftArc || !midArc || !rightArc) {
        return nullptr;
    }

    CPoint<TScalar> p1 = points[leftArc->indices.first], p2 = points[midArc->indices.first], p3 = points[rightArc->indices.first];
    Circle<TScalar> circle(p1, p2, p3);
    if (p2.y > p1.y && p2.y > p3.y) {
        return nullptr;
    }

    if (std::abs(circle.bottomPoint.y - *sweepLine) < EPS || *sweepLine < circle.bottomPoint.y) {
        midArc->circleEvent = std::make_shared<CircleEvent<TScalar>>(circle.bottomPoint, circle, midArc);
        return midArc->circleEvent;
    }

    return nullptr;
}

template<typename TScalar>
void VoronoiDiagram<TScalar>::Build() {
    for (int i = 0; i < points.size(); i++) {
        queueEvent.push(std::make_shared<SiteEvent<TScalar>>(points[i], i));
    }
    faces.resize(points.size(), nullptr);
    sweepLine = 0L;

    while (!queueEvent.empty()) {
        auto event = queueEvent.top();
        queueEvent.pop();

        sweepLine = event->site.y;

        if (std::dynamic_pointer_cast<SiteEvent<TScalar>>(event) != nullptr) {
            std::shared_ptr<SiteEvent<TScalar>> siteEvent = std::dynamic_pointer_cast<SiteEvent<TScalar>>(event);
            auto IndexPair = std::make_pair<int, int>(siteEvent->GetIndex(), siteEvent->GetIndex());
            if (!beachLineRoot) {
                beachLineRoot = std::make_shared<BeachLineNode<TScalar>>(IndexPair,
                                                                         &sweepLine,
                                                                         points);
                continue;
            }
            siteEvent->HandleEvent(this);
        } else if (std::dynamic_pointer_cast<CircleEvent<TScalar>>(event) != nullptr) {
            std::shared_ptr<CircleEvent<TScalar>> circleEvent = std::dynamic_pointer_cast<CircleEvent<TScalar>>(event);
            circleEvent->HandleEvent(this);
        }
    }
    for (int i = 0; i < halfEdges.size(); i++) {
        std::shared_ptr<HalfEdge<TScalar>> edge = halfEdges[i];
        if (!edge->prev || !faces[edge->begin]) {
            faces[edge->begin] = edge;
        }
    }
}

template<typename TScalar>
VoronoiDiagram<TScalar>::VoronoiDiagram(std::vector<CPoint<TScalar>> &_points) : points(_points),
                                                                                 beachLineRoot(nullptr) {
}


template<typename TScalar>
TScalar VoronoiDiagram<TScalar>::getAnswer() {


    int numberOfPolygons = 0, numberOfEdges = 0;
    for (auto edge : halfEdges) {
        if (!edge->used) {
            int n = getPolygonStats(edge);
            if (n > 0) {
                numberOfEdges += n;
                numberOfPolygons++;
            }
        }
    }
    if (numberOfPolygons == 0) { return 0; }
    TScalar ans = TScalar(numberOfEdges) / TScalar(numberOfPolygons);
    return ans;


}

template<typename TScalar>
int VoronoiDiagram<TScalar>::getPolygonStats(std::shared_ptr<HalfEdge<TScalar>> _start) {

    auto nextEdge = _start->next;
    _start->used = true;
    int ans = 1;
    while (nextEdge != _start) {
        if (nextEdge == nullptr) {
            return 0;
        }
        nextEdge->used = true;
        ans++;
        nextEdge = nextEdge->next;
    }
    return ans;
}


template<typename TScalar>
void SiteEvent<TScalar>::HandleEvent(VoronoiDiagram<TScalar> *_voronoiDiagram) {
    std::shared_ptr<BeachLineNode<TScalar>> &root = _voronoiDiagram->GetRoot();
    std::vector<std::shared_ptr<HalfEdge<TScalar>>> *halfEdges = _voronoiDiagram->GetEdges();

    std::shared_ptr<BeachLineNode<TScalar>> current_arc = Find(root, this->site.x);
    std::shared_ptr<BeachLineNode<TScalar>> leftChild, rightChild;
    std::shared_ptr<BeachLineNode<TScalar>> subTree;

    if (current_arc->circleEvent) {
        current_arc->circleEvent = nullptr;
    }

    //intersect 2 arcs
    if (_voronoiDiagram->GetPoints()[current_arc->indices.first] != this->site &&
        std::abs(root->points[current_arc->indices.first].y - this->site.y) < EPS) {
        subTree = MakeTreeForSingleIntersection(index, current_arc->indices.first, root->sweepLine,
                                                _voronoiDiagram->GetPoints(),
                                                halfEdges);

        leftChild = subTree->left;
        rightChild = subTree->right;
    } else if (_voronoiDiagram->GetPoints()[current_arc->indices.first] != this->site &&
               std::abs(root->points[current_arc->indices.first].y - this->site.y) >= EPS) {
        subTree = MakeTreeForDoubleIntersection(index, current_arc->indices.first, root->sweepLine, root->points,
                                                halfEdges);
        leftChild = subTree->left;
        rightChild = subTree->right->right;
    } else { return; }

    if (current_arc->prev) {
        JoinLeaves(current_arc->prev, leftChild);
    }

    if (current_arc->next) {
        JoinLeaves(rightChild, current_arc->next);
    }

    root = Swap(current_arc, subTree);

    std::shared_ptr<CircleEvent<TScalar>> newEvent = makeCircleEvent(leftChild->prev, leftChild, leftChild->next,
                                                                     _voronoiDiagram->GetPoints(),
                                                                     root->sweepLine);
    if (newEvent != nullptr) {
        _voronoiDiagram->GetEvents().push(newEvent);
    }
    newEvent = makeCircleEvent(rightChild->prev, rightChild, rightChild->next, _voronoiDiagram->GetPoints(),
                               root->sweepLine);
    if (newEvent != nullptr) {
        _voronoiDiagram->GetEvents().push(newEvent);
    }
}

template<typename TScalar>
void CircleEvent<TScalar>::HandleEvent(VoronoiDiagram<TScalar> *_voronoiDiagram) {

    std::shared_ptr<BeachLineNode<TScalar>> currentArc = this->arc, nextLeaf, prevLeaf;
    std::pair<std::shared_ptr<BeachLineNode<TScalar>>, std::shared_ptr<BeachLineNode<TScalar>>> interPoints = getInterPoints(
            currentArc);

    if (!interPoints.first || !interPoints.second) {
        return;
    }
    if (std::abs(interPoints.first->getValue() - interPoints.second->getValue()) > EPS * 1e5) { return; }

    std::shared_ptr<Vertex<TScalar>> v = std::make_shared<Vertex<TScalar>>(this->circle.centre);
    std::shared_ptr<HalfEdge<TScalar>> e1 = interPoints.first->edge, e2 = interPoints.second->edge;
    _voronoiDiagram->GetVertices().push_back(v);
    if (currentArc->prev && currentArc->prev->circleEvent) {
        currentArc->prev->circleEvent = nullptr;
    }
    if (currentArc->next && currentArc->next->circleEvent) {
        currentArc->next->circleEvent = nullptr;
    }

    prevLeaf = currentArc->prev;
    nextLeaf = currentArc->next;

    std::shared_ptr<BeachLineNode<TScalar>> newArc;
    if (currentArc->parent == interPoints.first) {
        newArc = interPoints.second;
    } else {
        newArc = interPoints.first;
    }

    _voronoiDiagram->GetRoot() = Remove(currentArc);

    std::pair<std::shared_ptr<HalfEdge<TScalar>>, std::shared_ptr<HalfEdge<TScalar>>> twinEdges = getTwins<TScalar>(
            prevLeaf->indices.first, nextLeaf->indices.first);
    newArc->edge = twinEdges.first;

    Join(e2, e1->twin);
    Join(e1, twinEdges.first);
    Join(twinEdges.second, e2->twin);
    _voronoiDiagram->GetEdges()->push_back(twinEdges.first);
    _voronoiDiagram->GetEdges()->push_back(twinEdges.second);
    if (prevLeaf && nextLeaf) {
        std::shared_ptr<CircleEvent<TScalar>> NewEvent = makeCircleEvent<TScalar>(prevLeaf->prev, prevLeaf, nextLeaf,
                                                                                  _voronoiDiagram->GetPoints(),
                                                                                  arc->sweepLine);
        if (NewEvent) {
            _voronoiDiagram->GetEvents().push(NewEvent);
        }
        NewEvent = makeCircleEvent(prevLeaf, nextLeaf, nextLeaf->next, _voronoiDiagram->GetPoints(), arc->sweepLine);
        if (NewEvent) {
            _voronoiDiagram->GetEvents().push(NewEvent);
        }
    }
}


int main() {
    std::vector<CPoint<long double>> points;
    CPoint<long double> Point;
    while (std::cin >> Point.y) {
        std::cin >> Point.x;
        points.push_back(Point);
    }
    VoronoiDiagram<long double> v(points);
    v.Build();
    std::cout << v.getAnswer();
    return 0;
}
