#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "four4s.hpp"
#include "fraction.hpp"

#define UNARY_LIMIT 4


namespace std
{
    template<>
        struct hash<Fraction>
        {
            size_t operator()(const Fraction & f) const
            {
                return hash<double>()(f.getValue());
            }
        };
}

std::string operator * (std::string a, int b) {
    std::string output = "";
    while (b--) {
        output += a;
    }
    return output;
}


Four4s::Four4s(int n, int a)
    :num{n}, amount{a}
{
    if (n >= 10 || n < 0)
    {
        throw std::invalid_argument("Number should be between 0 and 9.");
    }

    allowDecimal = true;
    allowRepeatingDecimal = true;
    allowSquareRoot = true;
    allowFactorial = true;
    allowPercent = true;
    allowGamma = true;

    allowAddition = true;
    allowSubtraction = true;
    allowMultiplication = true;
    allowDivision = true;
    allowPower = true;
    allowRoot = true;

    for (int i = 0; i < amount; i++)
    {
        expressions.push_back(std::unordered_map<Fraction,ExpressionInfo>());
        std::string basicNum = (std::to_string(num) * (i + 1));
        try
        {
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 1), ExpressionInfo(basicNum, 0));
        }
        catch(OutOfBoundsException const & e)
        {
        }
 
        if (allowDecimal)
        {
            long divideBy = 10;
            for (int j = i; j >= 0; j--)
            {
                try
                {
                    std::string decimalNum = (basicNum.substr(0, j) + "." + basicNum.substr(j, basicNum.length()));
                    expressions.at(i).emplace(Fraction(std::stol(basicNum), divideBy), ExpressionInfo(decimalNum, 1));
                    divideBy *= 10;
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }
        }

        if (i == 0 && allowRepeatingDecimal)
        {
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 9), ExpressionInfo(("." + basicNum + "..."), 1));
        }
    }
}

std::vector<std::unordered_map<Fraction, ExpressionInfo>> & Four4s::getExpressions()
{
    return expressions;
}

void Four4s::addUnaryOperations(int d, bool integerOnly)
{
    int digits = d - 1;
    std::unordered_map<Fraction, int> smallestComplexities;
    std::vector<Fraction> frontier;
    for (auto const& [key, val] : expressions.at(digits))
    {
        frontier.push_back(key);
        smallestComplexities.emplace(key, val.complexity);
    }

    while (!frontier.empty())
    {
        Fraction f = frontier.back();
        frontier.pop_back();
        ExpressionInfo ei = expressions.at(digits).at(f);

        if (allowSquareRoot)
        {
            try
            {
                Fraction result = f.sqrt();
                if (
                    (ei.sqrt_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count));
                        smallestComplexities.emplace(result, ei.complexity + 1);
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (ei.complexity + 1 < smallestComplexities.at(result))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count);
                            smallestComplexities.at(result) = ei.complexity + 1;
                        }
                    }
                    
                }
            }
            catch(IrrationalException const & e)
            {
            }
            catch(OutOfBoundsException const & e)
            {
            }
        }

        if (allowFactorial)
        {
            try
            {
                Fraction result = f.factorial();
                if (
                    (ei.sqrt_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("(" + ei.expression + ")!"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count + 1, ei.percent_count, ei.gamma_count));
                        smallestComplexities.emplace(result, ei.complexity + 1);
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (ei.complexity + 1 < smallestComplexities.at(result))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("(" + ei.expression + ")!"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count + 1, ei.percent_count, ei.gamma_count);
                            smallestComplexities.at(result) = ei.complexity + 1;
                        }
                    }
                    
                }
            }
            catch(IrrationalException const & e)
            {
            }
            catch(OutOfBoundsException const & e)
            {
            }
        }

        if (allowPercent)
        {
            try
            {
                Fraction result = f.percent();
                if (
                    (ei.sqrt_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("(" + ei.expression + ")%"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count, ei.percent_count + 1, ei.gamma_count));
                        smallestComplexities.emplace(result, ei.complexity + 1);
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (ei.complexity + 1 < smallestComplexities.at(result))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("(" + ei.expression + ")%"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count, ei.percent_count + 1, ei.gamma_count);
                            smallestComplexities.at(result) = ei.complexity + 1;
                        }
                    }
                    
                }
            }
            catch(IrrationalException const & e)
            {
            }
            catch(OutOfBoundsException const & e)
            {
            }
        }

        if (allowGamma)
        {
            try
            {
                Fraction result = f.gamma();
                if (
                    (ei.sqrt_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("gamma(" + ei.expression + ")"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count + 1));
                        smallestComplexities.emplace(result, ei.complexity + 1);
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (ei.complexity + 1 < smallestComplexities.at(result))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("gamma(" + ei.expression + ")"), ei.complexity + 1, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count + 1);
                            smallestComplexities.at(result) = ei.complexity + 1;
                        }
                    }
                    
                }
            }
            catch(IrrationalException const & e)
            {
            }
            catch(OutOfBoundsException const & e)
            {
            }
        }

    }
}

void Four4s::addBinaryOperations(int d1, int d2, bool integerOnly, bool NoNegatives)
{
    int digits1 = d1 - 1;
    int digits2 = d2 - 1;
    int totalDigits = (d1 + d2) - 1;
    
    std::unordered_map<Fraction, int> smallestComplexities;
    for (auto const& [key, val] : expressions.at(totalDigits))
    {
        smallestComplexities.emplace(key, val.complexity);
    }

    for (auto const& [key1, val1] : expressions.at(digits1))
    {
        for (auto const& [key2, val2] : expressions.at(digits2))
        {
            if (allowAddition)
            {
                try
                {
                    Fraction result = key1 + key2;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") + (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") + (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            if (allowSubtraction)
            {
                try
                {
                    Fraction result = key1 - key2;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") - (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") - (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
                try
                {
                    Fraction result = key2 - key1;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ") - (" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ") - (" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            if (allowMultiplication)
            {
                try
                {
                    Fraction result = key1 * key2;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") * (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") * (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            if (allowDivision)
            {
                try
                {
                    Fraction result = key1 / key2;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") / (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") / (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
                try
                {
                    Fraction result = key2 / key1;
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ") / (" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ") / (" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            if (allowPower)
            {
                try
                {
                    Fraction result = key1.pow(key2);
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ")^(" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ")^(" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
                try
                {
                    Fraction result = key2.pow(key1);
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ")^(" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ")^(" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            if (allowRoot)
            {
                try
                {
                    Fraction result = key1.root(key2);
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
                try
                {
                    Fraction result = key2.root(key1);
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("root(" + val2.expression + ", " + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count));
                            smallestComplexities.emplace(result, val1.complexity + val2.complexity + 1);
                        }
                        else
                        {
                            if (val1.complexity + val2.complexity + 1 < smallestComplexities.at(result))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("root(" + val2.expression + ", " + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count);
                                smallestComplexities.at(result) = val1.complexity + val2.complexity + 1;
                            }
                        }
                    }
                }
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

        }
    }
}


int main()
{
    
    Four4s f = Four4s(4, 4);
    f.addUnaryOperations(1);
    f.addBinaryOperations(1, 1);
    f.addUnaryOperations(2);
    f.addBinaryOperations(1, 2);
    f.addUnaryOperations(3);

    f.addBinaryOperations(1, 3, true);
    f.addBinaryOperations(2, 2. true);
    f.addUnaryOperations(4, true);

    std::vector<Fraction> keys;
    for (auto const& [key, val] : f.getExpressions().at(3))
    {
        keys.push_back(key);
    }

    std::sort(keys.begin(), keys.end());

    std::ofstream myfile;
    myfile.open("four4s.txt");

    for (Fraction key: keys)
    {
        ExpressionInfo val = f.getExpressions().at(3).at(key);
        /*
        std::cout << key.getValue()
                << std::endl
                << key.getNumerator()
                << "/"
                << key.getDenominator()
                << std::endl
                << val.expression
                << std::endl
                << std::endl;
        */
        myfile << key.getValue() << ": " << val.expression << std::endl;
    }
    myfile.close();
    std::cout << std::endl;
    
}

