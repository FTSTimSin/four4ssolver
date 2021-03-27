#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>

#include <iostream>
#include <chrono>
#include <ctime>

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
#define REPEATING_DEC_PURITY 1
#define ROOT_PURITY 2
#define GAMMA_PURITY 4
#define PERCENT_PURITY 3
#define SQUARE_PURITY 5


/*
    This is a hash function for Fraction because I guess I need 
    it for unordered maps? i miss python man
*/
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

/*
    This is a function I made to multiply strings (ex. "abc" * 
    2 == "abcabc"). i REALLY miss python in case you haven't 
    noticed
*/
std::string operator * (std::string a, int b) {
    std::string output = "";
    while (b--) {
        output += a;
    }
    return output;
}


/*
    Four4s Constructor: Constructs the Four4s object. Basically
    it creates expressions and adds the most basic numbers to it 
    (all the number that can be expressed with the number, a decimal
    point, and concatenation). addPercent also adds the percent of 
    the basic numbers, and this is done to comply with the percent 
    rule outlined by David A. Wheeler's approach to this problem. He 
    only allows the percent to be next to a constant number instead 
    of an equation like a normal operator. To comply with this 
    rule, you would set addPercent to true in the constructor and 
    set allowPercent to false (I call this the "Wheeler Percent").
    David A. Wheeler's Definitive Four Fours Answer Key:
    https://dwheeler.com/fourfours/.
*/
Four4s::Four4s(int n, int a, bool addPercent)
    :num{n}, amount{a}
{
    /*
        num should be a single digit number. I didn't want there to be
        something like ten 10s, because concatenation would be weird 
        (would it be 1010? or 110?).
    */
    if (n >= 10 || n < 0)
    {
        throw std::invalid_argument("Number should be between 0 and 9.");
    }

    /*
        Change these to allow/disallow operators. I might make this a 
        part of a UI or something later but for now just manually change
        these.
    */
    allowDecimal = true;
    allowRepeatingDecimal = true;
    allowSquareRoot = true;
    allowFactorial = true;
    allowPercent = true;
    allowGamma = true;
    allowSquare = true;

    allowAddition = true;
    allowSubtraction = true;
    allowMultiplication = true;
    allowDivision = true;
    allowPower = true;
    allowRoot = true;

    /*
        For each amount of nums (for three 4s it would be 3), this makes
        a new map and adds all the basic numbers to the expression maps.
        ex. for Four 4s, the first map would have 4, .4, and .4..., the 
        second map would have 44, 4.4, and .44, etc.
    */
    for (int i = 0; i < amount; i++)
    {
        // Concatenated Numbers
        expressions.push_back(std::unordered_map<Fraction,ExpressionInfo>());
        std::string basicNum = (std::to_string(num) * (i + 1));
        try
        {
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 1), ExpressionInfo(basicNum, 0, 0));

            // Wheeler Percent 
            if (addPercent)
            {
                expressions.at(i).emplace(Fraction(std::stol(basicNum), 100), ExpressionInfo(basicNum + "%", 0, PERCENT_PURITY));
            }
        }
        catch(OutOfBoundsException const & e)
        {
        }

        // Decimals
        if (allowDecimal)
        {
            long divideBy = 10;
            for (int j = i; j >= 0; j--)
            {
                try
                {
                    std::string decimalNum = (basicNum.substr(0, j) + "." + basicNum.substr(j, basicNum.length()));
                    expressions.at(i).emplace(Fraction(std::stol(basicNum), divideBy), ExpressionInfo(decimalNum, 1, DECIMAL_PURITY));

                    // Wheeler Percent
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

        // Repeated Decimal
        if (i == 0 && allowRepeatingDecimal)
        {
            expressions.at(i).emplace(Fraction(std::stol(basicNum), 9), ExpressionInfo(("." + basicNum + "..."), 1, REPEATING_DEC_PURITY));
            // Wheeler Percent
            if (addPercent)
            {
                expressions.at(i).emplace(Fraction(std::stol(basicNum), 900), ExpressionInfo(("." + basicNum + "...%"), 0, PERCENT_PURITY));
            }
        }

    }
}

/*
    getExpressions: Returns the vector of maps containing all of the Fractions 
    and their respective ExpressionInfos.
*/
std::vector<std::unordered_map<Fraction, ExpressionInfo>> & Four4s::getExpressions()
{
    return expressions;
}

/*
    addUnaryOperations: Adds all the possible unary operators to each Fraction 
    in expressions with d - 1 digits. 
    ex. if expressions[0] contained 4 and .4, then after this function is called,
    it would also contain sqrt(4), 4!, sqrt(.4), sqrt(sqrt(4)), etc.
*/
void Four4s::addUnaryOperations(int d, bool integerOnly)
{
    int digits = d - 1;

    /*
        frontier is a vector of Fractions that need to have unary operators done
        to them. This is done so that more Fractions can be added as the while 
        loop goes on.
        ex. 4 is in the frontier. After the first loop through the while loop, 
        4 is removed and sqrt(4), 4!, etc. are in the frontier. That way, we can 
        get the unary operations of sqrt(4), such as sqrt(sqrt(4)), sqrt(4)!, etc.
        To prevent the obvious problem of this going on infinitely, there is a 
        unary limit" imposed that limits the amount of unary operators for an 
        expression.
        ex. if UNARY_LIMIT is 2, then you would not be able to add sqrt to 
        sqrt(sqrt(4)). You would still be able to do stuff like sqrt(sqrt(4))! 
        and gamma(sqrt(sqrt(4))), given that those other operators haven't 
        reached their unary limit either.
    */
    std::vector<Fraction> frontier;
    for (auto const& [key, val] : expressions.at(digits))
    {
        frontier.push_back(key);
    }

    // Loops until the frontier is empty
    while (!frontier.empty())
    {
        Fraction f = frontier.back();
        frontier.pop_back();
        ExpressionInfo ei = expressions.at(digits).at(f);

        // Adds the square root operator of the number to expressions[d]
        if (allowSquareRoot)
        {
            try
            {
                Fraction result = f.sqrt();
                /*
                    Check if the unary limit is not reached or if the Fraction
                    is an integer while integerOnly is true
                */
                if (
                    (ei.sqrt_count < UNARY_LIMIT) &&
                    ((!integerOnly || result.getDenominator() == 1)))
                {
                    /*
                        The purity would either be the expression's previous
                        purity (if its purity is greater than sqrt's purity),
                        or it would be the sqrt's purity.
                    */
                    int resultPurity = std::max(ei.purity, SQRT_PURITY);

                    /*
                        If the Fraction does not exist in expressions, add it 
                        along with its ExpressionInfo. Its ExpressionInfo would 
                        be the same as its previous ExpressionInfo except for the
                        new expression (with sqrt), its previous complexity 
                        (because we are adding an operator), and its sqrt_count.
                    */
                    if (expressions.at(digits).count(result) == 0)
                    {
                        expressions.at(digits).emplace(
                            result, ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count));
                        frontier.push_back(result);
                    }
                    else
                    {
                        /*
                            If the fraction already exists, only replace the 
                            previous ExpressionInfo if its purity is lower or
                            its purity is equal and its complexity is lower.
                        */
                        if (resultPurity < expressions.at(digits).at(result).purity ||
                            (resultPurity == expressions.at(digits).at(result).purity && ei.complexity + 1 < expressions.at(digits).at(result).complexity))
                        {
                            expressions.at(digits).at(result) = 
                                ExpressionInfo(("sqrt(" + ei.expression + ")"), ei.complexity + 1, resultPurity, ei.sqrt_count + 1, ei.factorial_count, ei.percent_count, ei.gamma_count, ei.square_count);
                        }
                    }
                    
                }
            }
            /*
                If there is an error, it is ensured that those numbers 
                will not be saved to either expressions or the frontier.
            */
            catch(IrrationalException const & e)
            {
            }
            catch(OutOfBoundsException const & e)
            {
            }
        }

        // Adds the factorial operator of the number to expressions[d] (same as above)
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

        // Adds the percent operator of the number to expressions[d] (same as above)
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
        
        // Adds the gamma operator of the number to expressions[d] (same as above)
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

        // Adds the square operator of the number to expressions[d] (same as above)
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

/*
    addBinaryOperations: Gets all the possible binary operators to each Fraction 
    in expressions with d1 - 1 digits and expressions with d2 - 1 digits and adds 
    them to expressions with (d1 + d2) - 1 digits. 
    ex. if expressions[0] contained 4 and expressions[1] contained 44, then 
    expressions[2] would have 4 + 44, 44 - 4, etc.
*/
void Four4s::addBinaryOperations(int d1, int d2, bool integerOnly, bool NoNegatives)
{
    int digits1 = d1 - 1;
    int digits2 = d2 - 1;
    int totalDigits = (d1 + d2) - 1;
    
    /*
        Loops through the two expressions[digits] (These maps should not be added to 
        during this function).
    */
    for (auto const& [key1, val1] : expressions.at(digits1))
    {
        for (auto const& [key2, val2] : expressions.at(digits2))
        {

            // Adds the addition operator (Similar to unary operators)
            if (allowAddition)
            {
                try
                {
                    Fraction result = key1 + key2;

                    /*
                        Check if the Fraction is integer while integerOnly is true or 
                        if the Fraction is not negative while noNegatives is true.
                    */
                    if (
                        (!integerOnly || result.getDenominator() == 1) &&
                        !(NoNegatives && result < 0.0))
                    {
                        // Setting purity is the same here as with unary operators
                        int resultPurity = std::max(std::max(val1.purity, val2.purity), ADDITION_PURITY);

                        /*
                            If the Fraction does not exist in expressions, add it 
                            along with its ExpressionInfo. Its ExpressionInfo would 
                            be the same as its previous... this is just deja vu.
                        */
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
                // Error handling same as unary operators
                catch(IrrationalException const & e)
                {
                }
                catch(OutOfBoundsException const & e)
                {
                }
            }

            // Adds the subtraction operator (Similar to above)
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
                /*
                    With subtraction, division, power and root, the basic code is ran 
                    twice, except with this second time, the two Fractions are swapped.
                    ex. They would add both 4 / 44 and 44 / 4.
                    With addition, and multiplication, swapping the two numbers results 
                    in the same result so they are only ran once.
                */
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

            // Adds the multiplication operator (Similar to addition)
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

            // Adds the division operator (Similar to subtraction)
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
            
            // Adds the power operator (Similar to subtraction)
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

            // Adds the root operator (Similar to subtraction)
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
                                    ("root(" + val2.expression + ", " + val1.expression + ")"), 
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
                                        ("root(" + val2.expression + ", " + val1.expression + ")"), 
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
    auto start = std::chrono::system_clock::now();

    Four4s f = Four4s(4, 4);
    f.addUnaryOperations(1);
    std::cout << "Unary1 " << f.getExpressions().at(0).size() << std::endl;
    f.addBinaryOperations(1, 1);
    std::cout << "Binary2 " << f.getExpressions().at(1).size() << std::endl;
    f.addUnaryOperations(2);
    std::cout << "Unary2 " << f.getExpressions().at(1).size() << std::endl;
    f.addBinaryOperations(1, 2);
    std::cout << "Binary3 " << f.getExpressions().at(2).size() << std::endl;
    f.addUnaryOperations(3);
    std::cout << "Unary3 " << f.getExpressions().at(2).size() << std::endl;

    f.addBinaryOperations(1, 3, true);
    std::cout << "Binary4,1 " << f.getExpressions().at(3).size() << std::endl;
    f.addBinaryOperations(2, 2, true);
    std::cout << "Binary4,2 " << f.getExpressions().at(3).size() << std::endl;
    f.addUnaryOperations(4, true);
    std::cout << "Unary4 " << f.getExpressions().at(3).size() << std::endl;
    std::vector<Fraction> keys;
    for (auto const& [key, val] : f.getExpressions().at(3))
    {
        keys.push_back(key);
    }

    std::sort(keys.begin(), keys.end());

    std::ofstream myfile;
    myfile.open("four4s1m.txt");

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
    
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";
    
}

