/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc {

  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \HECHO Compute the autocorrelation r[l]
      for (unsigned int n = 0; n <= x.size()-1-l; ++n){
        r[l]=r[l] + x[n]*x[n+l];
      }
      r[l]=(1.0/x.size())*r[l];
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      for (unsigned int m = 0; m<frameLen; ++m){
        window[m] = 0.54 -0.46*cos((2*M_PI*m)/(frameLen-1));
      }
      
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float pow_min, float r1norm_min, float rmaxnorm_min) const {
    /// \HECHO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    
    //Si una de les dues aproximacions de correlació i la potència superen el llindar, considerem voiced
    if ((r1norm>=r1norm_min || rmaxnorm>=rmaxnorm_min) && pot > pow_min) return false;
    return true;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x, float r1norm_min, float rmaxnorm_min, float pow_min) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);
  
    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \HECHO
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
  ///	
	/// In either case, the lag should not exceed that of the minimum value of the pitch.
    while (*iR > 0 ){
      iR++;
    }

    if(iR < r.begin() + npitch_min) iR += npitch_min;
    
    iRMax = iR;
    while(iR!=r.end()){
      if(*iR>*iRMax){
        iRMax=iR;
      }
      ++iR;
    }

    unsigned int lag = iRMax - r.begin();

    float pot = 10 * log10(r[0]);

#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << '\t' << r[1]/r[0] << '\t' << '\t' << r[lag]/r[0] << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0], pow_min, r1norm_min, rmaxnorm_min))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
