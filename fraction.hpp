#ifndef __FRACTION__HPP
#define __FRACTION__HPP

#include <string>


/*
    IrrationalException: It is thrown when a Fraction/Number is a
    number deemed irrational or undefined.
    ex. sqrt(2), 1/0, etc.
*/
struct IrrationalException: public std::exception
{
    const char* what() const throw()
    {
        return "Number is probably irrational!";
    }
};

/*
    OutOfBoundsException: It is thrown when a Fraction/Number is a
    number deemed too large or too precise. This is determined if
    a Fraction's numerator is greater than NUMERATOR_LIMIT or if a
    Fraction's denominator is greater than DENOMINATOR_LIMIT (both
    are defined in fraction.cpp)
*/
struct OutOfBoundsException: public std::exception
{
    const char* what() const throw()
    {
        return "Number is out of bounds!";
    }
};


/*
    Fraction Class: Objects of this class are rational fractions. This
    class was created in order to prevent imprecise float calculations.
    ex. in Python, 1.1 + 2.2 == 3.30000...3. This class also prevents
    irrational numbers, as I do not think they are necessary for this
    puzzle. The fractions are also always in simplest form.
*/
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