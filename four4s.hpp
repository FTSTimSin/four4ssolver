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
    int purity;
    int sqrt_count;
    int factorial_count;
    int percent_count;
    int gamma_count;
    int square_count;
    ExpressionInfo(std::string e, int c, int p, int sc=0, int fc=0, int pc=0, int gc=0, int sqc=0)
        : expression{e}, complexity{c}, purity{p}, sqrt_count{sc}, factorial_count{fc}, percent_count{pc}, gamma_count{gc}, square_count{sqc}
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
    bool allowSquare;

    bool allowAddition;
    bool allowSubtraction;
    bool allowMultiplication;
    bool allowDivision;
    bool allowPower;
    bool allowRoot;

    std::vector<std::unordered_map<Fraction, ExpressionInfo>> expressions;

public:
    Four4s(int n, int a, bool addPercent=false);

    std::vector<std::unordered_map<Fraction, ExpressionInfo>> & getExpressions();

    void addUnaryOperations(int d, bool integerOnly=false);

    void addBinaryOperations(int d, int d2, bool integerOnly=false, bool NoNegatives=true);

};


#endif