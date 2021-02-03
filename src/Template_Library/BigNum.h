#ifndef _BigNum_h_
#define _BigNum_h_

#include <gmp.h>
#include <ostream>
#include <string>
using namespace std;


namespace KCBox {


class BigInt
{
	friend int sscanf( char str[], BigInt & i );
	friend void printf( BigInt & i );
	friend istream & operator >> ( istream & fin, BigInt & i );
	friend ostream & operator << ( ostream & fout, const BigInt & i );
	friend double DoubleQuotient( const BigInt & n, const BigInt & d );
public:
	BigInt() { mpz_init(_xCount); }
	BigInt(long int num) { mpz_init_set_si(_xCount, num); }
	BigInt(const BigInt& xBigNum_) { mpz_init_set(_xCount, xBigNum_._xCount); }
	~BigInt() { mpz_clear(_xCount); }
    void operator = (const BigInt &other) { mpz_set(_xCount, other._xCount); }
    void operator = (long num) { mpz_set_si(_xCount, num); }
	void operator += (const BigInt& other) { mpz_add(_xCount, _xCount, other._xCount); }
	void operator += (unsigned long num) { mpz_add_ui(_xCount, _xCount, num); }
	void operator -= (const BigInt& other) { mpz_sub(_xCount, _xCount, other._xCount); }
	void operator *= (const BigInt& other) { mpz_mul(_xCount, _xCount, other._xCount); }
	void operator *= (unsigned long iCount_) { mpz_mul_ui(_xCount, _xCount, iCount_); }
	void operator /= (const BigInt& other) { mpz_fdiv_q(_xCount, _xCount, other._xCount); }
	void operator %= (const BigInt& other) { mpz_fdiv_r(_xCount, _xCount, other._xCount); }
	bool operator < (const long int num) { return mpz_cmp_si(_xCount, num) < 0; }
	bool operator < (const BigInt& other) { return mpz_cmp(_xCount, other._xCount) < 0; }
	bool operator <= (const long int num) { return mpz_cmp_si(_xCount, num) <= 0; }
	bool operator > (const long int num) { return mpz_cmp_si(_xCount, num) > 0; }
	bool operator > (const BigInt& other) { return mpz_cmp(_xCount, other._xCount) > 0; }
	bool operator >= (const long int num) { return mpz_cmp_si(_xCount, num) >= 0; }
	bool operator == ( const BigInt& other ) const { return mpz_cmp(_xCount, other._xCount) == 0; }
    bool operator == ( const long num ) const { return mpz_cmp_si(_xCount, num) == 0; }
	bool operator != ( const BigInt& other ) const { return mpz_cmp(_xCount, other._xCount) != 0; }
	bool operator != ( const long num ) const { return mpz_cmp_si(_xCount, num) != 0; }
	double TransformDouble() { return mpz_get_d( _xCount ); }
	void Assign_2exp( const int e ) { mpz_set_si(_xCount, 1);  mpz_mul_2exp(_xCount, _xCount, e); }
	void Mul_2exp( const int e ) { mpz_mul_2exp(_xCount, _xCount, e); }
	void Div_2exp( const int e ) { mpz_div_2exp(_xCount, _xCount, e); }
	bool Divisible_2exp( const int e ) { return mpz_divisible_2exp_p(_xCount, e ); }
	size_t Memory() const { return sizeof(mpz_t) + _xCount->_mp_alloc * sizeof(mp_limb_t); }
protected:
	mpz_t _xCount;
};

class BigFloat
{
	friend int sscanf( char str[], BigFloat & f );
	friend void printf( BigFloat & f );
	friend double Normalize( const BigFloat & left, const BigFloat & right );
	friend double Normalize( const BigFloat & left, const BigFloat & right, BigFloat & sum );
	friend ostream & operator << ( ostream & fout, const BigFloat & d );
public:
    BigFloat() { mpf_init(_xCount); }
    BigFloat( double num ) { mpf_init_set_d(_xCount, num); }
    BigFloat( const BigFloat &other ) { mpf_init_set(_xCount, other._xCount); }
    ~BigFloat() { mpf_clear(_xCount); }
    void operator = (const BigFloat &other) { mpf_set(_xCount, other._xCount); }
    void operator = (double num) { mpf_set_d(_xCount, num); }
    void operator += (const BigFloat &other) { mpf_add(_xCount, _xCount, other._xCount); }
    void operator -= (const BigFloat &other) { mpf_sub(_xCount, _xCount, other._xCount); }
    void operator *= (const BigFloat &other) { mpf_mul(_xCount, _xCount, other._xCount); }
    void operator /= (const BigFloat &other) { mpf_div(_xCount, _xCount, other._xCount); }
    bool operator < (const BigFloat &other) const { return mpf_cmp(_xCount, other._xCount) < 0; }
    bool operator < (const double num) const { return mpf_cmp_d(_xCount, num) < 0; }
    bool operator > (const BigFloat &other) const { return mpf_cmp(_xCount, other._xCount) > 0; }
    bool operator > (const double num) const { return mpf_cmp_d(_xCount, num) > 0; }
    bool operator >= (const BigFloat &other) const { return mpf_cmp(_xCount, other._xCount) >= 0; }
    bool operator >= (const double num) const { return mpf_cmp_d(_xCount, num) >= 0; }
    bool operator == (const BigFloat &other) const { return mpf_cmp(_xCount, other._xCount) == 0; }
    bool operator == (const double num) const { return mpf_cmp_d(_xCount, num) == 0; }
    bool operator != (const BigFloat &other) const { return mpf_cmp(_xCount, other._xCount) != 0; }
    bool operator != (const double num) const { return mpf_cmp_d(_xCount, num) != 0; }
	double TransformDouble() { return mpf_get_d( _xCount ); }
	void Assign_2exp( const int e ) { mpf_set_d(_xCount, 1);  mpf_mul_2exp(_xCount, _xCount, e); }
	void Mul_2exp( const int e ) { mpf_mul_2exp(_xCount, _xCount, e); }
	void Div_2exp( const int e ) { mpf_div_2exp(_xCount, _xCount, e); }
	bool LE_2exp( const int e )
	{
	    mpf_t tmp;
		mpf_init( tmp );
	    mpf_set_ui( tmp, 1 );
	    mpf_mul_2exp(tmp, tmp, e);
	    bool result = mpf_cmp(_xCount, tmp) <= 0;
	    mpf_clear( tmp );
	    return result;
	}
	void Mean( const BigFloat & left, const BigFloat & right, double ratio )
	{
		mpf_t r, tmp;
		mpf_init( r ); mpf_init( tmp );
		mpf_set_d( r, ratio );
		mpf_mul( _xCount, left._xCount, r );
		mpf_set_d( r, 1 - ratio );
		mpf_mul( tmp, right._xCount, r );
		mpf_add( _xCount, _xCount, tmp );
		mpf_clear( r ); mpf_clear( tmp );
	}
	void Weighted_Sum( double w1, const BigFloat & f1, double w2, const BigFloat & f2 )
	{
		mpf_t tmp1, tmp2;
		mpf_init_set_d( tmp1, w1 ); mpf_init_set_d( tmp2, w2 );
		mpf_mul( tmp1, tmp1, f1._xCount );
		mpf_mul( tmp2, tmp2, f2._xCount );
		mpf_add( _xCount, tmp1, tmp2 );
		mpf_clear( tmp1 ); mpf_clear( tmp2 );
	}
protected:
    mpf_t _xCount;
};


}


#endif //BigNum_h
