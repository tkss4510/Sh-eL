#include "math.hpp"
#include "ShelExceptions.hpp"
#include <climits>

long long Math::Add(long long a, long long b)
{
	if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b)) throw MathException("overflow");
	return a + b;
}

long long Math::Sub(long long a, long long b)
{
	if ((b < 0 && a > LLONG_MAX + b) || (b > 0 && a < LLONG_MIN + b)) throw MathException("overflow");
	return a - b;
}

long long Math::Mul(long long a, long long b)
{
	if (a > 0)
	{
		if (b > 0 ? a > LLONG_MAX / b : b < LLONG_MIN / a) throw MathException("overflow");
	}
	else
	{
		if (b > 0 ? a < LLONG_MIN / b : (a != 0 && b < LLONG_MAX / a)) throw MathException("overflow");
	}
	return a * b;
}

long long Math::Div(long long a, long long b)
{
	if (b == 0) throw MathException("division by zero");
	if (a == LLONG_MIN && b == -1) throw MathException("overflow");
	return a / b;
}
