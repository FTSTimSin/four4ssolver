# Examples
These are the results I have provided from using this solver for myself.
## Four4s1.txt and Four4s2.txt
This is a set of solutions from 0 to 1 million using my arbitrary rules and rankings. Here is how I determined the purity scores:
* 0: addition, subtraction, multiplication, division, square root, power, and factorial. I also allowed decimals (such as .4 and .44) and basic concatenation (such as 44 and 444). However, I do not allow concatenation of expressions (4||(4 + 4) = 48 would not be valid).
* 1: repeating decimal. It is basically just .4... .
* 2: arbitrary root (where root(4, .4) would be 4^(1 / .4) = 32)
* 3: percent sign. This can be used as a general "divide by 100" operator. (such as 4% = 0.04, (4 + 4)% = 0.08, (4 / 4)% = 0.01, etc.)
* 4: gamma function: It is basically (x - 1)!. I know it's also some complicated integral but I didn't make this project to get Calculus PTSD.
* 5: square function: It is basically x^2. I feel like this is the edge of what's considered as cheating but I included it just because it is very helpful for larger numbers while still being a relatively simple function.

Because Github only allows individual files to be under 25mb, I have split my solution to two files.
Four4s1.txt has the solutions for 0 - 500,000
Four4s2.txt has the solutions for 500,001 - 1,000,000

Of the first 10,000 numbers, my solutions are only missing 11. These numbers are 7907, 8507, 8513, 8843, 8867, 9133, 9467, 9547, 9707, 9737, and 9893.
Of the first 1,000,000 numbers, my solutions have 404,129 numbers solved, or 40.4%.

## Four4sWheelerRules.txt
This is a set of solutions from 0 to 1 million using the rules set by David A. Wheeler from [The Definitive Four Fours Answer Key](https://dwheeler.com/fourfours/). His purity scores are determined like this:
* 0: addition, subtraction, multiplication, division, square root, factorial, and power
* 1: Unused to my knowledge
* 2: repeating decimal
* 3: arbitrary root
* 4: gamma function
* 5: percent sign. However unlike my rules, a percent sign can only follow a constant. For example, 4%, .4%, and .4...% would be valid but (sqrt(4))% and (4 + 4)% are invalid.
* 6: square function

NOTE: David A. Wheeler also details a purity level 7 and 8, which have to do with logical operators such as OR, AND, XOR, <<, etc. These are not included in my solution because I have not implemented them in my program.

Of the first 10,000 numbers, my solutions are missing 857, including 1723, 2103, 2107, 2217, 2227, 2237, and 2487.
Of the first 1,000,000 numbers, my solutions have 180,481 numbers solved, or 18.0%
<br/>
Please check out [The Definitive Four Fours Answer Key](https://dwheeler.com/fourfours/) if you are a fan of this puzzle. This is an incredible list of solutions from 0 to 40,000 and it is what inspired me to create this program.
