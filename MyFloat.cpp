//Authors: Gabriella Quattrone and Matthew Czajkowski
#include "MyFloat.h"

MyFloat::MyFloat(){
  sign = 0;
  exponent = 0;
  mantissa = 0;
}

MyFloat::MyFloat(float f){
  unpackFloat(f);
}

MyFloat::MyFloat(const MyFloat & rhs){
	sign = rhs.sign;
	exponent = rhs.exponent;
	mantissa = rhs.mantissa;
}

ostream& operator<<(std::ostream &strm, const MyFloat &f){
	//this function is complete. No need to modify it.
	strm << f.packFloat();
	return strm;
}

MyFloat MyFloat::operator+(const MyFloat& rhs) const{
	int exp = 0;
	int extra = 0;
	
	MyFloat copy_this(*this);
	MyFloat copy_rhs(rhs);
	
	copy_this.mantissa |= 0x800000;
	copy_rhs.mantissa |= 0x800000;
	
	if(copy_this.mantissa == copy_rhs.mantissa){//if all components of both numbers 
		if(copy_this.exponent == copy_rhs.exponent){//except the sign are the same,
			if(copy_this.sign != copy_rhs.sign){//we can conclude the result is 0
				return 0;
			}
		}
	}
	if(copy_this.exponent > copy_rhs.exponent){
		exp = copy_this.exponent - copy_rhs.exponent;
		copy_rhs.mantissa = (copy_rhs.mantissa >> (exp - 1));
		extra = copy_rhs.mantissa & 1;
		copy_rhs.mantissa = copy_rhs.mantissa >> 1;
		copy_rhs.exponent = copy_rhs.exponent + exp;
	}
	
	else if(copy_this.exponent < copy_rhs.exponent){
		exp = copy_rhs.exponent - copy_this.exponent;
		copy_this.mantissa = (copy_this.mantissa >> (exp - 1));
		extra = copy_this.mantissa & 1;
		copy_this.mantissa = copy_this.mantissa >> 1;
		copy_this.exponent = copy_this.exponent + exp;
		
	}
	
	if(copy_this.sign != copy_rhs.sign){//If one (-) and one is (+)
                if(copy_this.exponent < copy_rhs.exponent || (copy_this.exponent == copy_rhs.exponent && copy_rhs.mantissa > copy_this.mantissa)){
			copy_this.sign = copy_rhs.sign;
		}
		if(copy_this.mantissa > copy_rhs.mantissa){
			copy_this.mantissa = copy_this.mantissa - copy_rhs.mantissa;
			copy_this.mantissa = copy_this.mantissa - extra;
		}
		else{
			copy_this.mantissa = copy_rhs.mantissa - copy_this.mantissa;
			copy_this.mantissa = copy_this.mantissa - extra;
		}
	}
	else
	   copy_this.mantissa += copy_rhs.mantissa;//then both numbers are either both (+) or both (-)
		
        int i = 0;
	for(i = 31; i >= 0; i--){
		if(((copy_this.mantissa >> i) & 1) == 1)
			break;
	}
	
	int shift = i - 23;
	
	if(shift > 0){
		copy_this.mantissa >>= shift;
		copy_this.exponent += shift;
	}
	else{
		copy_this.mantissa <<= (-1*shift);
		copy_this.exponent -= (-1*shift);
	}
	
	copy_this.mantissa &= (0x7fffff); //bit string === 011111111111111111111111
	
	return copy_this;  //you don't have to return *this. it's just here right now so it will compile
}

MyFloat MyFloat::operator-(const MyFloat& rhs) const{
	
	MyFloat cop(rhs);
	if(cop.sign == 1)
		cop.sign = 0;
	else
		cop.sign = 1;
	return *this + cop; //you don't have to return *this. it's just here right now so it will compile
}

bool MyFloat::operator==(const float rhs) const{
	MyFloat copy_t(*this);
	MyFloat copy_r(rhs);
	if(copy_t.mantissa == copy_r.mantissa){
		if(copy_t.sign == copy_r.sign){
			if(copy_t.exponent == copy_r.exponent){
				return true;
			}
		}
	}
	return false; //this is just a stub so your code will compile
}


void MyFloat::unpackFloat(float f) {
	//sign = (f >> (31)) & 0b1
	//exponent = (f >> (23)) & 0b11111111
	//mantissa = (f >> (0)) & 0b11111111111111111111111
	__asm__(
	"movl %[f], %[sign];"
        "shrl $31, %[sign];"
	"andl $0b1, %[sign];"
        "movl %[f], %[exponent];"
        "shrl $23, %[exponent];"
	"andl $0b11111111, %[exponent];"
	"movl %[f], %[mantissa];"
        "andl $0x7fffff, %[mantissa];":
	[sign] "=r" (sign), [exponent] "=r" (exponent), [mantissa] "=r" (mantissa):
        [f] "r" (f):
        "cc");
	//this function must be written in inline assembly
	//extracts the fields of f into sign, exponent, and mantissa
}//unpackFloat

float MyFloat::packFloat() const{
  float f = 0;
  
  __asm__( 
    "shll $23, %[exponent];" //move the exponent back
    "shll $31, %[sign];" //move the sign back
    "orl  %[exponent], %[sign];" //add on exp field to sign field
    "orl  %[sign], %[mantissa];" //add back those two fields to mantissa field
    "movl %[mantissa], %[f]": //now move all fields back to float
    [f] "=r" (f):
    [sign] "r" (sign), [exponent] "r" (exponent), [mantissa] "r" (mantissa):);

  //this function must be written in inline assembly
  //returns the floating point number represented by this
  
  return f;
}//packFloat

