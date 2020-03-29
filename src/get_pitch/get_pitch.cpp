/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt> [<pow_min>] [<r1norm_min>] [<rmaxnorm_min>]
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
    pow_min       Minimum power (in dB) of a frame to be considered as voiced
                  Introduce a positive value, that will be converted to a negative one
    r1norm_min    Minimum r1norm of a frame to be considered as voiced
    rmaxnorm_min  Minimum rmaxnorm of a frame to be considered as voiced
    
)";

int main(int argc, const char *argv[]) {
	/// \HECHO
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.

  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav  = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();

  float pow_min, r1norm_min, rmaxnorm_min;

  if(argc == 6){
    std::string pmin       = args["<pow_min>"].asString();
    std::string r1min      = args["<r1norm_min>"].asString();
    std::string rmax       = args["<rmaxnorm_min>"].asString();

    pow_min      = stof(pmin)*-1.0;
    r1norm_min   = stof(r1min);
    rmaxnorm_min = stof(rmax);
  }
  else{
    pow_min      = -52.0F;
    r1norm_min   = 0.945;
    rmaxnorm_min = 0.47;
  }

  

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500, pow_min, r1norm_min, rmaxnorm_min);

  /// \HECHO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  float maxvalue=0.0F;
  for(unsigned int k = 0; k< x.size(); k++){
    if(x[k]> maxvalue) maxvalue = x[k];
  }
  float llindar_CC=0.01*maxvalue;
  for(unsigned int k = 0; k < x.size(); k++){
    if (abs(x[k]) < llindar_CC) x[k]=0;
    else if (x[k]> llindar_CC) x[k]-=llindar_CC;
    else if (x[k] < -llindar_CC) x[k]+=llindar_CC;
  }
  

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len, r1norm_min, rmaxnorm_min, pow_min);
    f0.push_back(f);
  }

  /// \HECHO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  int medianLength = 3;
   //   Move window through all elements of the signal
  for (unsigned int i = (medianLength-1)/2; i < f0.size() - (medianLength-1)/2; ++i)
  {
    //   Pick up window elements
    float window[medianLength];
    for (int j = 0; j < medianLength; ++j)
        window[j] = f0[i - (medianLength-1)/2 + j];
    //   Order elements (only half of them)
    for (int j = 0; j < medianLength - (medianLength-1)/2; ++j)
    {
        //   Find position of minimum element
        int min = j;
        for (int k = j + 1; k < medianLength; ++k)
          if (window[k] < window[min])
              min = k;
        //   Put found minimum element in its place
        const float temp = window[j];
        window[j] = window[min];
        window[min] = temp;
    }
    //   Get result - the middle element
    f0[i] = window[(medianLength-1)/2];
  }


  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
