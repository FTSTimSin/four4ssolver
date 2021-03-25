#ifndef __FRACTION__HPP
#define __FRACTION__HPP

#include <string>


struct IrrationalException: public std::exception
{
    const char* what() const throw()
    {
        return "Number is probably irrational!";
    }
};

struct OutOfBoundsException: public std::exception
{
    const char* what() const throw()
    {
        return "Number is out of bounds!";
    }
};


class Fraction
{
private:
    long numerator;
    long denominator;

public:
    Fraction(long n, long d);

    void simplify();

    void operator=(const Fraction & f);

    bool operator==(const Fraction & f) const;
    bool operator==(const long num) const;
    bool operator==(const double num) const;

    bool operator<(const Fraction & f) const;
    bool operator<(const long num) const;
    bool operator<(const double num) const;

    bool operator>(const Fraction & f) const;
    bool operator>(const long num) const;
    bool operator>(const double num) const;

    Fraction operator+(const Fraction & f) const;
    Fraction operator-(const Fraction & f) const;
    Fraction operator*(const Fraction & f) const;
    Fraction operator/(const Fraction & f) const;

    Fraction pow(const Fraction & f) const;
    Fraction root(const Fraction &f) const;
    Fraction sqrt() const;
    Fraction square() const;

    Fraction factorial() const;
    Fraction gamma() const;

    Fraction percent() const;

    long getNumerator() const;
    long getDenominator() const;
    double getValue() const;

};


#endif 