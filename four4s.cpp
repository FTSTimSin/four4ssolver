#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "four4s.hpp"
#include "fraction.hpp"

#define UNARY_LIMIT 4

#define ADDITION_PURITY 0
#define SUBTRACTION_PURITY 0
#define MULTIPLICATION_PURITY 0
#define DIVISION_PURITY 0
#define SQRT_PURITY 0
#define FACTORIAL_PURITY 0
#define POWER_PURITY 0
#define DECIMAL_PURITY 0
#define REPEATING_DEC_PURITY 2
#define ROOT_PURITY 3
#define GAMMA_PURITY 4
#define PERCENT_PURITY 5
#define SQUARE_PURITY 6


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


Four4s::Four4s(int n, int a, bool addPercent)
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
    allowPercent = false;
    allowGamma = true;
    allowSquare = true;

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
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 1), ExpressionInfo(basicNum, 0, 0));

            if (addPercent)
            {
                expressions.at(i).emplace(Fraction(std::stol(basicNum), 100), ExpressionInfo(basicNum + "%", 0, PERCENT_PURITY));
            }
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
                    expressions.at(i).emplace(Fraction(std::stol(basicNum), divideBy), ExpressionInfo(decimalNum, 1, DECIMAL_PURITY));

                    if (addPercent)
                    {
                        expressions.at(i).emplace(Fraction(std::stol(basicNum), divideBy * 100), ExpressionInfo(decimalNum + "%", 0, PERCENT_PURITY));
                    }

                    divideBy *= 10;
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }
        }

        if (i == 0 && allowRepeatingDecimal)
        {
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 9), ExpressionInfo(("." + basicNum + "..."), 1, REPEATING_DEC_PURITY));
            if (addPercent)
            {
                expressions.at(i).emplace(Fraction(std::stol(basicNum), 900), ExpressionInfo(("." + basicNum + "...%"), 0, PERCENT_PURITY));
            }
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
    std::vector<Fraction> frontier;
    for (auto const& [key, val] : expressions.at(digits))
    {
        frontier.push_back(key);
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
                    int resultPurity = std::max(ei.purity, SQRT_PURITY);
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count));
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count);
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
                    (ei.factorial_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    int resultPurity = std::max(ei.purity, FACTORIAL_PURITY);
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("(" + ei.expression + ")!"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count + 1, ei.percent_count, ei.gamma_count, ei.square_count));
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("(" + ei.expression + ")!"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count + 1, ei.percent_count, ei.gamma_count, ei.square_count);
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
                    (ei.percent_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    int resultPurity = std::max(ei.purity, PERCENT_PURITY);
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("(" + ei.expression + ")%"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count + 1, ei.gamma_count, ei.square_count));
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("(" + ei.expression + ")%"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count + 1, ei.gamma_count, ei.square_count);
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
                    (ei.gamma_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    int resultPurity = std::max(ei.purity, GAMMA_PURITY);
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("gamma(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count + 1, ei.square_count));
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("gamma(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count + 1, ei.square_count);
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

        if (allowSquare)
        {
            try
            {
                Fraction result = f.square();
                if (
                    (ei.square_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    int resultPurity = std::max(ei.purity, SQUARE_PURITY);
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("square(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count + 1));
                        frontier.push_back(result);
                    }
                    else
                    {
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("square(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count + 1);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), ADDITION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") + (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") + (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), SUBTRACTION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") - (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") - (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), SUBTRACTION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ") - (" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ") - (" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), MULTIPLICATION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") * (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") * (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), DIVISION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ") / (" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ") / (" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), DIVISION_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ") / (" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ") / (" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), POWER_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val1.expression + ")^(" + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val1.expression + ")^(" + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), POWER_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("(" + val2.expression + ")^(" + val1.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("(" + val2.expression + ")^(" + val1.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), ROOT_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), ROOT_PURITY);
                        if (expressions.at(totalDigits).count(result) == 0)
                        {
                            expressions.at(totalDigits).emplace(
                                result, 
                                ExpressionInfo(
                                    ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                    val1.complexity + val2.complexity + 1, 
                                    resultPurity, 
                                    val1.sqrt_count + val2.sqrt_count, 
                                    val1.factorial_count + val2.factorial_count, 
                                    val1.percent_count + val2.percent_count, 
                                    val1.gamma_count + val2.gamma_count,
                                    val1.square_count + val2.square_count));
                        }
                        else
                        {
                            if (resultPurity < expressions.at(totalDigits).at(result).purity ||
                            (resultPurity == expressions.at(totalDigits).at(result).purity && val1.complexity + val2.complexity + 1 < expressions.at(totalDigits).at(result).complexity))
                            {
                                expressions.at(totalDigits).at(result) =
                                    ExpressionInfo(
                                        ("root(" + val1.expression + ", " + val2.expression + ")"), 
                                        val1.complexity + val2.complexity + 1, 
                                        resultPurity, 
                                        val1.sqrt_count + val2.sqrt_count, 
                                        val1.factorial_count + val2.factorial_count, 
                                        val1.percent_count + val2.percent_count, 
                                        val1.gamma_count + val2.gamma_count,
                                        val1.square_count + val2.square_count);
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
    
    Four4s f = Four4s(4, 4, true);
    f.addUnaryOperations(1);
    f.addBinaryOperations(1, 1);
    f.addUnaryOperations(2);
    f.addBinaryOperations(1, 2);
    f.addUnaryOperations(3);

    f.addBinaryOperations(1, 3, true);
    f.addBinaryOperations(2, 2, true);
    f.addUnaryOperations(4, true);
    std::vector<Fraction> keys;
    for (auto const& [key, val] : f.getExpressions().at(3))
    {
        keys.push_back(key);
    }

    std::sort(keys.begin(), keys.end());

    std::ofstream myfile;
    myfile.open("four4swheelerpercent.txt");

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
        if (std::fmod(key.getValue(), 1.0) == 0.0)
        {
            myfile << key.getValue() << " (" << val.purity << ") = " << val.expression << std::endl;
        }
    }
    myfile.close();
    std::cout << std::endl;
    
}

