//
// Created by nikita on 04.12.18.
//
#pragma once

#include <vector>
#include <string>
#include <iostream>

static const long long q = 1000000000;


class BigInteger {
public:
    BigInteger(int n = 0);

    friend bool operator<(const BigInteger &a, const BigInteger &b);

    friend bool operator>(const BigInteger &a, const BigInteger &b);

    friend bool operator<=(const BigInteger &a, const BigInteger &b);

    friend bool operator>=(const BigInteger &a, const BigInteger &b);

    friend bool operator==(const BigInteger &a, const BigInteger &b);

    friend bool operator!=(const BigInteger &a, const BigInteger &b);

    friend BigInteger operator+(const BigInteger &a, const BigInteger &b);

    friend BigInteger operator-(const BigInteger &a, const BigInteger &b);

    friend BigInteger operator*(const BigInteger &a, const BigInteger &b);

    friend BigInteger operator/(const BigInteger &a, const BigInteger &b);

    friend BigInteger operator%(const BigInteger &a, const BigInteger &b);

    BigInteger operator/(int b);

    BigInteger &operator+=(const BigInteger &other);

    BigInteger &operator-=(const BigInteger &other);

    BigInteger &operator*=(const BigInteger &other);

    BigInteger &operator/=(const BigInteger &other);

    BigInteger &operator%=(const BigInteger &other);

    BigInteger operator++(int);

    BigInteger &operator++();

    BigInteger operator--(int);

    BigInteger &operator--();


    std::string toString() const;

    void setPosFlag(bool posFlag);

    void setNumber(const std::vector<long long int> &number);

    explicit operator bool() const { return (*this) != BigInteger(0); }

//    operator int() { return std::stoi(toString()); }

    friend std::ostream &operator<<(std::ostream &os, const BigInteger &integer);

    friend std::istream &operator>>(std::istream &os, BigInteger &integer);

    const std::vector<long long int> getNumber() const;

    void pushBack(long long n) { number.push_back(n); }

    BigInteger operator-() const;


private:


    long long operator[](size_t i) const { return number[i]; };


    bool getPosFlag() const { return posFlag; }

    size_t getSize() const { return number.size(); }

private:
    std::vector<long long> number;
    bool posFlag;
};

bool operator==(const BigInteger &a, const BigInteger &b) {
    return !(a < b) && !(b < a);
}

bool operator!=(const BigInteger &a, const BigInteger &b) {
    return (a < b) || (b < a);
}

bool operator<(const BigInteger &a, const BigInteger &b) {
    if (a.getPosFlag() && !b.getPosFlag()) {
        return false;
    }
    if (!a.getPosFlag() && b.getPosFlag()) {
        return true;
    }
    if (!a.getPosFlag()) {
        return (-b < -a);
    }
    size_t i = a.getSize() - 1, j = b.getSize() - 1;
    if (i < j) {
        return true;
    }
    if (i > j) {
        return false;
    }
    while (i != (size_t) -1 && j != (size_t) -1) {
        if (a[i] > b[j]) {
            return false;
        }
        if (a[i] < b[j]) {
            return true;
        }
        i--;
        j--;
    }
    return false;
}

bool operator>(const BigInteger &a, const BigInteger &b) {
    return b < a;
}

bool operator<=(const BigInteger &a, const BigInteger &b) {
    return !(b < a);
}

bool operator>=(const BigInteger &a, const BigInteger &b) {
    return !(a < b);
}

BigInteger::BigInteger(int n) {
    if (n >= 0) {
        posFlag = true;
    } else {
        posFlag = false;
        n = -n;
    }
    if (n < q) {
        number.push_back(n);
    } else {
        number.push_back(n % 100000000);
        number.push_back(n / 100000000);
    }

}

BigInteger BigInteger::operator-() const {
    BigInteger minus = *this;
    minus.setPosFlag(!getPosFlag());
    return minus;
}

std::ostream &operator<<(std::ostream &os, const BigInteger &integer) {
    os << integer.toString();
    return os;
}

BigInteger operator*(const BigInteger &a, const BigInteger &b) {
    std::vector<long long> arrForSum(a.getSize() + b.getSize(), 0);
    for (size_t j = 0; j < a.getSize(); j++) {
        int extra = 0;
        for (size_t i = 0; i < b.getSize() || extra > 0; i++) {
            long long mul;
            if (i < b.getSize()) {
                mul = a[j] * b[i] + extra;
            } else {
                mul = extra;
            }
            arrForSum[i + j] += mul;
            long long temp = arrForSum[i + j];
            extra = temp / static_cast<long long>(100000000);
            arrForSum[i + j] %= 100000000;
        }
    }

    while (arrForSum.size() > 1 && arrForSum.back() == 0) {
        arrForSum.pop_back();
    }

    BigInteger newInt;
    newInt.setPosFlag((a.getPosFlag() ? 1 : -1) * (b.getPosFlag() ? 1 : -1) != -1);
    newInt.setNumber(arrForSum);
    return newInt;
}

void BigInteger::setPosFlag(bool posFlag) {
    BigInteger::posFlag = posFlag;
}

template<class BidirectionalIterator>
void Reverse(BidirectionalIterator first, BidirectionalIterator last) {
    while ((first != last) && (first != --last)) {
        std::iter_swap(first, last);
        ++first;
    }
}

std::string BigInteger::toString() const {
    std::string intInString;

    for (size_t i = 0; i < number.size() - 1; i++) {
        long long digit = number[i];
        for (int j = 0; j < 8; j++) {
            intInString += (digit % 10 + '0');
            digit /= 10;
        }
    }
    int sizeOfFirstDigit = 1;
    long long digit = number.back();
    long long size = 10;
    if (digit == 0) { return "0"; }
    while (digit * size <= 1000000000 - 1) {
        sizeOfFirstDigit++;
        size *= 10;
    }
    for (int i = 0; i <= 9 - sizeOfFirstDigit; i++) {
        intInString += (digit % 10 + '0');
        digit /= 10;
    }
    if (!posFlag) {
        intInString += '-';
    }
    Reverse(intInString.begin(), intInString.end());
    return intInString;
}

std::istream &operator>>(std::istream &os, BigInteger &integer) {
    std::string number;
    os >> number;
    std::vector<long long> arrForInteger;
    int i = 1;
    long long currentNumber = 0;
    size_t j = number.size() - 1;
    long long size = 1;
    size_t flag = (number[0] == '-' ? 1 : 0);
    while (j != flag - 1) {
        if (i > 8) {
            arrForInteger.push_back(currentNumber);
            currentNumber = 0;
            i = 1;
            size = 1;
        }
        currentNumber += (number[j] - '0') * size;
        size *= 10;
        i++;
        j--;
    }
    arrForInteger.push_back(currentNumber);
    integer.setNumber(arrForInteger);
    integer.setPosFlag(flag == 0);
    return os;
}

void BigInteger::setNumber(const std::vector<long long int> &number) {
    BigInteger::number = number;
}

BigInteger operator+(const BigInteger &a, const BigInteger &b) {
    std::vector<long long> arrForSum;

    if (!b.getPosFlag() && !a.getPosFlag()) {
        return -((-a) + (-b));
    } else if (!a.getPosFlag()) {
        return b - (-a);
    } else if (!b.getPosFlag()) {
        return a - (-b);
    }

    int extra = 0;
    for (size_t i = 0; i < (a.getSize() < b.getSize() ? b.getSize() : a.getSize()); i++) {
        long long sum = extra;
        if (i < a.getSize()) {
            sum += a[i];
        }
        if (i < b.getSize()) {
            sum += b[i];
        }
        extra = 0;
        if (sum >= 100000000) {
            sum -= 100000000;
            extra = 1;
        }
        arrForSum.push_back(sum);
    }
    if (extra == 1) {
        arrForSum.push_back(1);
    }


    BigInteger newInt;
    newInt.setNumber(arrForSum);
    return newInt;
}

BigInteger operator-(const BigInteger &a, const BigInteger &b) {
    std::vector<long long> arrForSum;
    if (!b.getPosFlag()) {
        return a + (-b);
    } else if (!a.getPosFlag()) {
        return -(-a + b);
    } else if (a < b) {
        return -(b - a);
    }

    int extra = 0;
    for (size_t i = 0; i < (a.getSize() < b.getSize() ? b.getSize() : a.getSize()); i++) {
        long long sum = extra;
        if (i < a.getSize()) {
            sum += a[i];
        }
        if (i < b.getSize()) {
            sum -= b[i];
        }
        extra = 0;
        if (sum < 0) {
            sum += 100000000;
            extra = -1;
        }
        arrForSum.push_back(sum);
    }
    while (arrForSum.size() > 1 && arrForSum.back() == 0) {
        arrForSum.pop_back();
    }
    BigInteger newInt;
    newInt.setNumber(arrForSum);
    return newInt;
}

const std::vector<long long int> BigInteger::getNumber() const {
    return number;
}

template<class T>
T abs(const T &x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

BigInteger operator/(const BigInteger &a, const BigInteger &b) {
    if (abs(b) > abs(a)) { return BigInteger(0); }
    if (!a.posFlag) {
        return -((-a) / b);
    }
    if (!b.posFlag) {
        return -((a) / (-b));
    }

    BigInteger l(0), r(a);

    while (l < r) {
        BigInteger mid = (l + r) / 2;
//        std::cout << l << " " << mid << " " << r << "\n";
        if (b * mid > a) {
            r = mid;
        } else if (a >= b + b * mid) {
            l = mid + BigInteger(1);
        } else {
            l.setPosFlag((a.getPosFlag() ? 1 : -1) * (b.getPosFlag() ? 1 : -1) != -1);
            return mid;
        }
    }
    l.setPosFlag((a.getPosFlag() ? 1 : -1) * (b.getPosFlag() ? 1 : -1) != -1);
    return l;
}


BigInteger BigInteger::operator/(int b) {
    std::vector<long long> arrForDiv(number.size(), 0);
    if (BigInteger(b) > *this) { return BigInteger(0); }
    bool bSign = b >= 0;
    b = (b > 0 ? b : -b);
    long long extra = 0;
    for (size_t i = number.size() - 1; i != (size_t) -1; i--) {
        long long divRes = number[i] + extra * 100000000;
        arrForDiv[i] = divRes / b;
        extra = divRes % b;
    }
    while (arrForDiv.size() > 1 && arrForDiv.back() == 0) {
        arrForDiv.pop_back();
    }
    BigInteger ans;
    ans.setNumber(arrForDiv);
    ans.setPosFlag(!(bSign xor posFlag));
    return ans;
}

BigInteger operator%(const BigInteger &a, const BigInteger &b) {
    return a - (a / b) * b;
}

BigInteger &BigInteger::operator++() {
    (*this) = (*this) + BigInteger(1);
    return (*this);
}

BigInteger BigInteger::operator++(int) {
    BigInteger original = (*this);
    (*this) = (*this) + BigInteger(1);
    return original;
}

BigInteger &BigInteger::operator--() {
    (*this) = (*this) - BigInteger(1);
    return (*this);
}

BigInteger BigInteger::operator--(int) {
    BigInteger original = (*this);
    (*this) = (*this) - BigInteger(1);
    return original;

}

BigInteger &BigInteger::operator+=(const BigInteger &other) {
    (*this) = (*this) + other;
    return (*this);
}

BigInteger &BigInteger::operator-=(const BigInteger &other) {
    (*this) = (*this) - other;
    return (*this);
}

BigInteger &BigInteger::operator*=(const BigInteger &other) {
    (*this) = (*this) * other;
    return (*this);
}

BigInteger &BigInteger::operator/=(const BigInteger &other) {
    (*this) = (*this) / other;
    return (*this);
}

BigInteger &BigInteger::operator%=(const BigInteger &other) {
    (*this) = (*this) % other;
    return (*this);
}
