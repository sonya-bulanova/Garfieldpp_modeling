#include <TRandom.h>

void random_check(){
  const unsigned int nTracks = 2000;
  TRandom * ran = new TRandom();
  for (unsigned int j = 0; j < nTracks; ++j) {
    double rTrack = ran->Uniform(-4.99, 4.99); //cm
    std::cout << rTrack << std::endl;
  }
}
