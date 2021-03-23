#include <cmath>
#include <numeric>
#include <string>

#include "fraction.hpp"

#define NUMERATOR_LIMIT 1000000
#define DENOMINATOR_LIMIT 1000


void Fraction::simplify()
{
    long div = std::gcd(numerator, denominator);
    numerator /= div;
    denominator /= div;
}

Fraction::Fraction(long n, long d)
    :numerator{n}, denominator{d}
{
    simplify();
    if (denominator == 0)
    {
        throw IrrationalException();
    }
    if (numerator > NUMERATOR_LIMIT || denominator > DENOMINATOR_LIMIT)
    {
        throw OutOfBoundsException();
    }
}

void Fraction::operator=(const Fraction & f)
{
    numerator = f.getNumerator();
    denominator = f.getDenominator();
}

bool Fraction::operator==(const Fraction & f) const
{
    return (numerator == f.numerator && denominator == f.denominator);
}

bool Fraction::operator==(const long num) const
{
    return (numerator == num && denominator == 1);
}

bool Fraction::operator==(const double num) const
{
    return (std::abs(getValue() - num) < 1e-15);
}

bool Fraction::operator<(const Fraction & f) const
{
    return (getValue() < f.getValue());
}

bool Fraction::operator<(const long num) const
{
    return (getValue() < num);
}

bool Fraction::operator<(const double num) const
{
    return (getValue() < num);
}

bool Fraction::operator>(const Fraction & f) const
{
    return (getValue() > f.getValue());
}

bool Fraction::operator>(const long num) const
{
    return (getValue() > num);
}

bool Fraction::operator>(const double num) const
{
    return (getValue() > num);
}

Fraction Fraction::operator+(const Fraction & f) const
{
    long new_denominator = std::lcm(denominator, f.getDenominator());
    long mul1 = new_denominator / denominator;
    long mul2 = new_denominator / f.getDenominator();

    long new_numerator = (numerator * mul1) + (f.getNumerator() * mul2);
    return Fraction(new_numerator, new_denominator);
}

Fraction Fraction::operator-(const Fraction & f) const
{
    long new_denominator = std::lcm(denominator, f.getDenominator());
    long mul1 = new_denominator / denominator;
    long mul2 = new_denominator / f.getDenominator();

    long new_numerator = (numerator * mul1) - (f.getNumerator() * mul2);
    return Fraction(new_numerator, new_denominator);
}

Fraction Fraction::operator*(const Fraction & f) const
{
    return Fraction((numerator * f.getNumerator()), (denominator * f.getDenominator()));
}

Fraction Fraction::operator/(const Fraction & f) const
{
    if (f.getNumerator() == 0)
    {
        throw IrrationalException();
    }
    return Fraction((numerator * f.getDenominator()), (denominator * f.getNumerator()));
}

Fraction Fraction::pow(const Fraction & f) const
{
    double irrationalCheck1 = std::pow(numerator, (f.getValue()));
    double irrationalCheck2 = std::pow(denominator, (f.getValue()));
    long new_numerator = std::round(irrationalCheck1);
    long new_denominator = std::round(irrationalCheck2);
    if (std::abs(new_numerator - irrationalCheck1) >= 1e-15 || std::abs(new_denominator - irrationalCheck2) >= 1e-15)
    {
        throw IrrationalException();
    }
    else
    {
        return Fraction(new_numerator, new_denominator);
    }
    
}

Fraction Fraction::root(const Fraction &f) const
{
    return pow(Fraction(f.getDenominator(), f.getNumerator()));
}

Fraction Fraction::sqrt() const
{
    return pow(Fraction(1, 2));
}

Fraction Fraction::factorial() const
{
    if (denominator != 1 || numerator < 0)
    {
        throw IrrationalException();
    }
    switch (numerator)
    {
        case 0:
            return Fraction(1, 1);
            break;
        case 1:
            return Fraction(1, 1);
            break;
        case 2:
            return Fraction(2, 1);
            break;
        case 3:
            return Fraction(6, 1);
            break;
        case 4:
            return Fraction(24, 1);
            break;
        case 5:
            return Fraction(120, 1);
            break;
        case 6:
            return Fraction(720, 1);
            break;
        case 7:
            return Fraction(5040, 1);
            break;
        case 8:
            return Fraction(40320, 1);
            break;
        case 9:
            return Fraction(362880, 1);
            break;
        case 10:
            return Fraction(3628800, 1);
            break;
        default:
            throw OutOfBoundsException();
    }
}

Fraction Fraction::gamma() const
{
    if (denominator != 1 || numerator < 0)
    {
        throw IrrationalException();
    }
    return Fraction(numerator - 1, denominator).factorial();
}

Fraction Fraction::percent() const
{
    return Fraction(numerator, (denominator * 100));
}

long Fraction::getNumerator() const
{
    return numerator;
}

long Fraction::getDenominator() const
{
    return denominator;
}

double Fraction::getValue() const
{
    return double(numerator) / double(denominator);
}