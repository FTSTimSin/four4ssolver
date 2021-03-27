#include <cmath>
#include <numeric>
#include <string>

#include "fraction.hpp"

#define NUMERATOR_LIMIT 1000000
#define DENOMINATOR_LIMIT 1000


/*
    simplify: Simplifies the fraction. To do this, the greatest
    common denominator is taken for both the numerator and denominator
    and then both are divided by the gcd.
    ex. with 4/6, the gcd would be 2, so (4/2)/(6/2) would be 2/3, the
    simpliest form.
*/
void Fraction::simplify()
{
    long div = std::gcd(numerator, denominator);
    numerator /= div;
    denominator /= div;
}

/*
    Fraction Constructor: Constructs a Fraction with a given numerator
    and denominator. The constructor also automatically simplifies the
    Fraction.
    ex. Fraction(4, 6) would construct a (2/3) Fraction
    If the denominator is 0, then an IrrationalException is thrown (x/0
    is undefined), and if the numerator or denominator goes above their
    respective limits, an OutOfBoundsException is thrown.
*/
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

/*
    Assignment Operator: Assigns the right Fraction's numerator and
    denominators to the left Fraction's.
*/
void Fraction::operator=(const Fraction & f)
{
    numerator = f.getNumerator();
    denominator = f.getDenominator();
}

/*
    Equals Operator (Fraction): Returns true if the two Fractions'
    numerators and denominators are equal. The fractions will always
    be in simplest form so I don't have to worry about 6/4 == 2/3.
*/
bool Fraction::operator==(const Fraction & f) const
{
    return (numerator == f.numerator && denominator == f.denominator);
}

/*
    Equals Operator (long): Returns true if the numerator equals the
    number and the denominator is 1. Because the fraction will always
    be in simplest form, I don't have to worry about numbers where
    the denominator is greater than 1, as it can be assumed that they
    are not integers.
*/
bool Fraction::operator==(const long num) const
{
    return (denominator == 1 && numerator == num);
}

/*
    Equals Operator (double): Returns true if the absolute value of
    the value of the Fraction (numerator / denominator) and the
    double is less than 1e-15. This is done to ensure that imprecise
    floats such as 0.333333... can equal 1/3.
*/
bool Fraction::operator==(const double num) const
{
    return (std::abs(getValue() - num) < 1e-15);
}

/*
    Less Than and Greater Than Operators: Same concepts as the Equals
    Operators (I'm too lazy to write redundant comments LOL).
*/
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

/* 
    All operators return a NEW fraction. The only modification to the 
    Fraction class are the simplify function and the assignment 
    operator.
*/

/*
    Addition Operator: Returns the sum of two fractions. To do this,
    the least common multiple of the two denominators is found as the
    new denominator. Then for the new numerator, the sum of the 
    numerator multiplied by the new denominator divided by the old
    denominator is the new numerator.
    ex. With 1/6 + 1/4, the lcm of the denominators is 12. The 
    numerators would be as such (1 * (12 / 6)) == 2, and (1 * (12 / 4)) 
    == 3. 2 + 3 is 5, so the fraction returned would be 5/12.
*/
Fraction Fraction::operator+(const Fraction & f) const
{
    long new_denominator = std::lcm(denominator, f.getDenominator());
    long mul1 = new_denominator / denominator;
    long mul2 = new_denominator / f.getDenominator();

    long new_numerator = (numerator * mul1) + (f.getNumerator() * mul2);
    return Fraction(new_numerator, new_denominator);
}

/*
    Subtraction Operator: Returns the difference of two fractions. Same 
    concept as the Addition Operator but instead of adding the two 
    numerators at the end, it gets the difference of the two.
*/
Fraction Fraction::operator-(const Fraction & f) const
{
    long new_denominator = std::lcm(denominator, f.getDenominator());
    long mul1 = new_denominator / denominator;
    long mul2 = new_denominator / f.getDenominator();

    long new_numerator = (numerator * mul1) - (f.getNumerator() * mul2);
    return Fraction(new_numerator, new_denominator);
}

/*
    Multiplication Operator: Returns the product of two fractions. It
    simply just returns a fraction with the numerator as the product of 
    the two numerators and the denominator as the product of the two 
    denominators.
    ex. 1/6 * 1/4 = (1 * 1)/ (6 * 4) = 1/24.
*/
Fraction Fraction::operator*(const Fraction & f) const
{
    return Fraction((numerator * f.getNumerator()), (denominator * f.getDenominator()));
}

/*
    Division Operator: Returns the quotient of two fractions. Same 
    concept as the Multiplication Operator but the second fraction's
    numerator and denominators are swapped. Also, if the second 
    fraction's numerator is 0, then an IrrationalException is thrown 
    (Division by Zero).
*/
Fraction Fraction::operator/(const Fraction & f) const
{
    if (f.getNumerator() == 0)
    {
        throw IrrationalException();
    }
    return Fraction((numerator * f.getDenominator()), (denominator * f.getNumerator()));
}

/*
    pow: Returns the result of the fraction raised to the fraction f's
    power. The new numerator is the power of the numerator to f's value
    and the new denominator is the power of the denominator to f's value.
    If either the numerator or denominator are not whole integers, the 
    result is assumed to be irrational and an IrrationalException is 
    thrown.
    ex. (1/2).pow(1/2). Numerator is 1.pow(1/2) = 1. Denominator is 
    2.pow(1/2) = 1.4142.... The denominator is not a whole number, so
    the result is assumed to be irrational.
*/
Fraction Fraction::pow(const Fraction & f) const
{
    double irrationalCheck1 = std::pow(numerator, (f.getValue()));
    double irrationalCheck2 = std::pow(denominator, (f.getValue()));
    long new_numerator = std::round(irrationalCheck1);
    long new_denominator = std::round(irrationalCheck2);
    if (std::abs(new_numerator - irrationalCheck1) >= 1e-15 || 
    std::abs(new_denominator - irrationalCheck2) >= 1e-15)
    {
        throw IrrationalException();
    }
    else
    {
        return Fraction(new_numerator, new_denominator);
    }
    
}

/*
    root: Returns the arbitrary root of the fraction to fraction f's
    root. It simply just calculates the power using the previously
    defined pow function, but f's numerator and denominator are flipped.
*/
Fraction Fraction::root(const Fraction &f) const
{
    return pow(Fraction(f.getDenominator(), f.getNumerator()));
}

/*
    sqrt: Returns the square root of the fraction. It simply calculates
    the power using the previously defined pow function, but the
    second fraction (fraction f) is always 1/2.
*/
Fraction Fraction::sqrt() const
{
    return pow(Fraction(1, 2));
}

/*
    square: Returns the fraction's square. It simply calculates the 
    power using the previously defined pow function, but the second fraction 
    (fraction f) is always 2/1.
*/
Fraction Fraction::square() const
{
    return pow(Fraction(2, 1));
}

/*
    factorial: Returns the fraction's factorial. A pre-calculated 
    factorial is returned using a switch statement (from 0! to 10!) to save 
    computing time (Making a real recursive factorial function is slow!). 
    An IrrationalException is thrown if the denominator is not 1 (the 
    fraction is not a whole integer). An OutOfBoundsException is thrown if 
    the fraction is greater than 10 (11! and onward are big numbers).
*/
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

/*
    gamma: Returns the result of the gamma function applied to the
    fraction. Basically, it returns the factorial of the fraction - 
    1. Just like the factorial function, an IrrationalException is 
    thrown if the denominator is less than 1 (not a whole number).
*/
Fraction Fraction::gamma() const
{
    if (denominator != 1 || numerator < 0)
    {
        throw IrrationalException();
    }
    return Fraction(numerator - 1, denominator).factorial();
}

/*
    percent: Returns the fraction percent (divide by 100). It just
    multiplies the denominator by 100.
*/
Fraction Fraction::percent() const
{
    return Fraction(numerator, (denominator * 100));
}

/*
    getNumerator: Returns the numerator.
*/
long Fraction::getNumerator() const
{
    return numerator;
}

/*
    getDenominator: Returns the denominator.
*/
long Fraction::getDenominator() const
{
    return denominator;
}

/*
    getValue: Returns the double value of the fraction (numerator / 
    denominator).
*/
double Fraction::getValue() const
{
    return double(numerator) / double(denominator);
}