#include "Misc.h"

#include <string>
#include <vector>
#include <map>

namespace misc
{
  std::string explode (std::string original, std::string exploder) {
    std::string tmp;
    tmp=original;
    int num, loc;
    num=1;
    while (tmp.find(exploder)!=std::string::npos) {
      loc=tmp.find(exploder);
      tmp=tmp.substr(loc+exploder.length());
      num++;
    }
    std::string *result;
    result = new std::string[num];
    num=0;
    tmp=original;
    while (tmp.find(exploder)!=std::string::npos) {
      loc=tmp.find(exploder);
      result[num]=tmp.substr(0,loc);
      tmp=tmp.substr(loc+exploder.length());
      num++;
    }
    result[num]=tmp;
    return result[num];
  }

  void find_and_replace( std::string& source, const std::string& find, const std::string& replace )
  {
  size_t found = source.rfind( find );
  while(found!=std::string::npos) //fixed unknown letters replace. Now it works correctly and replace all found symbold instead of only one at previous versions
  {
    source.replace( found, find.length(), replace );
    found = source.rfind( find );
  }
  }

  //dirty hack
  int FtoIround(float d)
  {
    return d<0 ? d-.5f : d+.5f;
  }

  char roundc( float a )
  {
    if( a < 0 )
      a -= 0.5f;
    if( a > 0 )
      a += 0.5f;
    if( a < -127 )
      a = -127;
    else if( a > 127 )
      a = 127;
    return static_cast<char>( a );
  }

  float frand()
  {
    return rand() / static_cast<float>( RAND_MAX );
  }

  float randfloat(float lower, float upper)
  {
    return lower + ( upper - lower ) * frand();
  }

  int randint(int lower, int upper)
  {
    return lower + static_cast<int>( ( upper + 1 - lower ) * frand() );
  }

  int getADTCord(float cord)
  {
    return cord / 533.33333f;
  }
}