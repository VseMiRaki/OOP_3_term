#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>

static const std::vector<std::pair<short int, short int>> directions = {{1,  1},
                                                                        {1,  -1},
                                                                        {-1, 1},
                                                                        {-1, -1},
                                                                        {1,  0},
                                                                        {-1, 0},
                                                                        {0,  1},
                                                                        {0,  -1}};

enum turn {
    White, Black
};

class ChessPiece;

class Field;

struct State {
    State(short int _x, short int _y) : x(_x), y(_y) {}

    short int x;
    short int y;

    bool operator==(const State &rhs) const {
        return x == rhs.x &&
               y == rhs.y;
    }

    bool operator!=(const State &rhs) const {
        return !(rhs == *this);
    }
};

class ChessPiece {
public:
    explicit ChessPiece(const State &_state) : state(_state) {}

    virtual std::vector<Field> GetMoves(const Field &field) = 0;

    virtual std::vector<Field> GetMovesWithCheck(const Field &field) = 0;

    virtual bool IsPossible(const Field &field) = 0;

    State state;
};

class King : public ChessPiece {
public:
    explicit King(const State &_state) : ChessPiece(_state) {}

    std::vector<Field> GetMoves(const Field &field) override;

    std::vector<Field> GetMovesWithCheck(const Field &field) override;

    bool IsPossible(const Field &field) override;
};

class Queen : public ChessPiece {
public:
    explicit Queen(const State &_state) : ChessPiece(_state) {}

    std::vector<Field> GetMoves(const Field &field) override;

    std::vector<Field> GetMovesWithCheck(const Field &field) override;

    bool IsPossible(const Field &field) override;
};

class Field {
public:
    Field(const std::shared_ptr<ChessPiece> &_Queen = nullptr, const std::shared_ptr<ChessPiece> &_King = nullptr,
          turn _move = White)
            : Queen(_Queen),
              King(_King), t(_move) {}

    const std::shared_ptr<ChessPiece> &getQueen() const {
        return Queen;
    }

    const std::shared_ptr<ChessPiece> &getKing() const {
        return King;
    }

    friend std::ostream &operator<<(std::ostream &os, const Field &field) {
        for (short int i = 0; i < 8; i++) {
            for (short int j = 0; j < 8; j++) {
                if (i == 2 && j == 2) {
                    os << "+ ";
                    continue;
                }
                if (i == field.King->state.y && j == field.King->state.x) {
                    os << "K" << " ";
                    continue;
                }
                if (i == field.Queen->state.y && j == field.Queen->state.x) {
                    os << "Q" << " ";
                    continue;
                }
                os << 0 << " ";
            }
            std::cout << "\n";
        }
        return os;
    }

    bool operator==(const Field &rhs) const;

    bool operator!=(const Field &rhs) const;

    void setTurn(turn move);

    bool getTurn() const {
        return t;
    }

    bool Checkmate();

private:
    std::shared_ptr<ChessPiece> Queen, King;
    turn t;
};

bool Field::operator==(const Field &rhs) const {
    return Queen->state == rhs.Queen->state &&
           King->state == rhs.King->state;
}

bool Field::operator!=(const Field &rhs) const {
    return !(rhs == *this);
}

void Field::setTurn(turn move) {
    Field::t = move;
}

bool Field::Checkmate() {

}

std::vector<Field> King::GetMoves(const Field &field) {
    std::vector<Field> ans;
    for (short int i = -1; i < 2; i++) {
        for (short int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (this->state.y + i >= 8 ||
                this->state.y + i < 0 ||
                this->state.x + j >= 8 ||
                this->state.x + j < 0) {
                continue;
            }
            Field newField = Field(field.getQueen(),
                                   std::make_shared<King>(State(this->state.x + j, this->state.y + i)),
                                   White);
            if (newField.getKing()->IsPossible(newField)) {
                ans.push_back(newField);
            }
        }
    }
    return ans;
}

std::vector<Field> King::GetMovesWithCheck(const Field &field) {
    std::vector<Field> ans;
    for (short int i = -1; i < 2; i++) {
        for (short int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (this->state.y + i >= 8 ||
                this->state.y + i < 0 ||
                this->state.x + j >= 8 ||
                this->state.x + j < 0) {
                continue;
            }
            Field newField = Field(field.getQueen(),
                                   std::make_shared<King>(State(this->state.x + j, this->state.y + i)),
                                   Black);
            if (!(std::fabs(newField.getKing()->state.x - 2) <= 1 && std::fabs(newField.getKing()->state.y - 2) <= 1) &&
                !(newField.getKing()->state == newField.getQueen()->state) &&
                newField.getQueen()->IsPossible(newField)) {
                ans.push_back(newField);
            }
        }
    }
    return ans;
}

bool King::IsPossible(const Field &field) {
    State location = field.getQueen()->state;
    for (auto lag : directions) {
        location = field.getQueen()->state;
        while (location.x < 8 && location.x >= 0 &&
               location.y < 8 && location.y >= 0 &&
               location != State(2, 2)) {
            if (location == this->state) { return false; }
            location.x += lag.first;
            location.y += lag.second;
        }
    }
    return !(std::fabs(this->state.x - 2) <= 1 && std::fabs(this->state.y - 2) <= 1);
}

std::vector<Field> Queen::GetMoves(const Field &field) {
    std::vector<Field> ans;
    State location = this->state;
    for (auto lag : directions) {
        location = this->state;
        while (location.x < 8 && location.x >= 0 &&
               location.y < 8 && location.y >= 0 &&
               location != State(2, 2)) {
            if (location.x == this->state.x && location.y == this->state.y) {
                location.x += lag.first;
                location.y += lag.second;
                continue;
            }
            Field newField = Field(std::make_shared<Queen>(location),
                                   field.getKing(),
                                   White);
            if (newField.getQueen()->IsPossible(newField)) {
                ans.push_back(newField);
            }
            location.x += lag.first;
            location.y += lag.second;
        }
    }
    return ans;
}

std::vector<Field> Queen::GetMovesWithCheck(const Field &field) {
    return std::vector<Field>();
}

bool Queen::IsPossible(const Field &field) {
    if (this->state.x == 2 && this->state.y == 2) { return false; }

    if (!(std::fabs(this->state.x - 2) <= 1 && std::fabs(this->state.y - 2) <= 1)) {
        return !(std::fabs(field.getKing()->state.x - this->state.x) <= 1 &&
                 std::fabs(field.getKing()->state.y - this->state.y) <= 1);
    }
    return !(this->state == field.getKing()->state);
}

int hash(const Field &f) {
    return (f.getKing()->state.x + f.getKing()->state.y * 8 + f.getQueen()->state.x * 64 +
            f.getQueen()->state.y * 64 * 8) + (f.getTurn() * (7 + 8 * 7 + 8 * 8 * 7 + 8 * 8 * 8 * 7 + 1)); // = 4096
}


class Avengers_4_Solver {
public:
    explicit Avengers_4_Solver(const Field &_field) : field(_field) {
        Distance.resize(4096 * 2, 1e9);
        allFields.resize(4096 * 2);
    }

    int MakeAFingerSnap();

private:
    Field field;
    std::vector<int> mates;

    std::vector<Field> allFields;
    std::vector<int> Distance;

    void FillAllCheckmates();
};

int Avengers_4_Solver::MakeAFingerSnap() {
    FillAllCheckmates();
    std::queue<int> q;

    for (const auto &mate : mates) {
        auto moves = allFields[mate].getQueen()->GetMoves(allFields[mate]);
        for (const auto &move : moves) {
            q.push(hash(move));
        }
    }
    while (!q.empty() && Distance[hash(field)] == 1e9) {
        int curHash = q.front();
        Field current = allFields[curHash];
        q.pop();
        if (current.getTurn() == White) {
            int min = 1e9;
            auto moves = current.getQueen()->GetMoves(current);
            if (moves.empty()) { continue; }
            for (auto &move : moves) {
                move.setTurn(Black);
                min = std::min(min, Distance[hash(move)]);
            }
            moves = current.getKing()->GetMovesWithCheck(current);
            if (Distance[hash(current)] > min + 1) {
                Distance[hash(current)] = min + 1;
                for (const auto &move : moves) {
                    if (!(Distance[hash(move)] != 1e9 && Distance[hash(move)] <= Distance[hash(current)] + 1)) {
                        q.push(hash(move));
                    }
                }
            }
        } else {
            int max = -1e9;
            auto moves = current.getKing()->GetMoves(current);
            if (moves.empty()) { continue; }
            for (auto &move : moves) {
                max = std::max(max, Distance[hash(move)]);
            }
            moves = current.getQueen()->GetMoves(current);
            if (max != 1e9) {
                if (Distance[hash(current)] == 1e9 || Distance[hash(current)] < max + 1) {
                    Distance[hash(current)] = max + 1;
                    for (const auto &move : moves) {
                        if (!(Distance[hash(move)] != 1e9 && Distance[hash(move)] >= Distance[hash(current)] + 1)) {
                            q.push(hash(move));
                        }
                    }
                }
            }
        }
    }
    return Distance[hash(field)];
}

void Avengers_4_Solver::FillAllCheckmates() {
    for (short int x1 = 0; x1 < 8; x1++) {
        for (short int y1 = 0; y1 < 8; y1++) {
            for (short int x2 = 0; x2 < 8; x2++) {
                for (short int y2 = 0; y2 < 8; y2++) {
                    Field newField(std::make_shared<Queen>(State(x1, y1)),
                                   std::make_shared<King>(State(x2, y2)), White);
                    allFields[hash(newField)] = newField;
                    newField.setTurn(Black);
                    allFields[hash(newField)] = newField;
                    if ((x2 == 2 && y2 == 2) || (x1 == 2 && y1 == 2)) { continue; }
                    if (!newField.getKing()->IsPossible(newField) && newField.getQueen()->IsPossible(newField) &&
                        newField.getKing()->GetMoves(newField).empty() &&
                        !(std::fabs(newField.getKing()->state.x - 2) <= 1 &&
                          std::fabs(newField.getKing()->state.y - 2) <= 1)) {
                        mates.push_back(hash(newField));
                        Distance[hash(newField)] = 0;
                    }
                }
            }
        }
    }
}

int main() {
    short int y1, x1, y2, x2;
    char a, b;
    std::cin >> a >> y1 >> b >> y2;
    y1--;
    y2--;
    x1 = a - 'a';
    x2 = b - 'a';
    auto k = std::make_shared<King>(State(x2, y2));
    auto q = std::make_shared<Queen>(State(x1, y1));
    Field f(q, k, White);
    Avengers_4_Solver solver(f);
    int ans = solver.MakeAFingerSnap();
    if (ans == 1e9) {
        std::cout << "IMPOSSIBLE";
    } else {
        std::cout << ans;
    }

    return 0;
}