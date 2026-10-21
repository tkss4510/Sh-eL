#ifndef MATH_HPP
#define MATH_HPP

// Integer arithmetic that throws MathException
namespace Math
{
long long Add(long long a, long long b);
long long Sub(long long a, long long b);
long long Mul(long long a, long long b);
long long Div(long long a, long long b); // truncates
} // namespace Math

#endif
