#ifndef __FOUR4S_HPP
#define __FOUR4S_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "fraction.hpp"


/*
    ExpressionInfo Struct: This struct stores a string expression (ex. 
    (4) + (4)), its complexity (defined by how many operators it uses), 
    its purity (defined by the the "purity" of the operator, which is 
    defined by the PURITY constants in four4s.cpp), and the counts of 
    all the unary operators.
*/
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
        : expression{e}, complexity{c}, purity{p}, 
        sqrt_count{sc}, factorial_count{fc}, percent_count{pc}, gamma_count{gc}, square_count{sqc}
    {
    }
};


/*
    Four4s Class: This is where all the magic happens.
*/
class Four4s
{
private:
    // num is the basic number for the puzzle (ex. three 4s's num is 4)
    int num;
    // amount is the number of nums for the puzzle (ex. three 4s's amount is 3)
    int amount;

    // These booleans dictate which operators to allow during the solving.
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

    /*
        expressions is a vector of maps containing all the solutions.
        Each element of the vector contains a map with a different amount of
        nums (ex. for four 4s, expressions[0] would be the map with all the 
        solutions using one 4, expressions[2] would be the map with all the 
        solutions using two 4s, etc.). The last element of expressions is 
        usually the solution to find. These maps' keys are Fractions, with
        their values being ExpressionInfos.
    */
    std::vector<std::unordered_map<Fraction, ExpressionInfo>> expressions;

public:
    Four4s(int n, int a, bool addPercent=false);

    std::vector<std::unordered_map<Fraction, ExpressionInfo>> & getExpressions();

    void addUnaryOperations(int d, bool integerOnly=false);

    void addBinaryOperations(int d, int d2, bool integerOnly=false, bool NoNegatives=true);

};


#endif