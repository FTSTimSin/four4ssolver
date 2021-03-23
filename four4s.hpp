#ifndef __FOUR4S_HPP
#define __FOUR4S_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "fraction.hpp"


struct ExpressionInfo
{
    std::string expression;
    int complexity;
    int sqrt_count;
    int factorial_count;
    int percent_count;
    int gamma_count;
    ExpressionInfo(std::string e, int c, int s=0, int f=0, int p=0, int g=0)
        : expression{e}, complexity{c}, sqrt_count{s}, factorial_count{f}, percent_count{p}, gamma_count{g}
    {
    }
};


class Four4s
{
private:
    int num;
    int amount;

    bool allowDecimal;
    bool allowRepeatingDecimal;
    bool allowSquareRoot;
    bool allowFactorial;
    bool allowPercent;
    bool allowGamma;

    bool allowAddition;
    bool allowSubtraction;
    bool allowMultiplication;
    bool allowDivision;
    bool allowPower;
    bool allowRoot;

    std::vector<std::unordered_map<Fraction, ExpressionInfo>> expressions;

public:
    Four4s(int n, int a);

    std::vector<std::unordered_map<Fraction, ExpressionInfo>> & getExpressions();

    void addUnaryOperations(int d, bool integerOnly=false);

    void addBinaryOperations(int d, int d2, bool integerOnly=false, bool NoNegatives=true);

};


#endif