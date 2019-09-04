#include "mandelbrot.h"

#include <thread>
#include <memory>
#include <utility>
#include <algorithm>
#include <fstream>

namespace MandelbrotCreator {

  MandelbrotPixels::MandelbrotPixels(int width, int height, int iterationMax):
    _width(width), _height(height), _iterationMax_(iterationMax)
  {
    _xStep = (long double)4.0 / ((long double)_width - (long double)1.0);
    _yStep = (long double)4.0 / ((long double)_height - (long double)1.0);
  }
  
  // static members
  long double MandelbrotPixels::_Z_ABS_BOUND_ = (long double)2.0;
  unsigned int MandelbrotPixels::_THREADS_ = std::thread::hardware_concurrency();

  // static methods
  bool MandelbrotPixels::_inMandelbrotSet(const std::complex<long double>& c,
        int& iterations, const int& iterationMax)
  {
    
    std::complex<long double> z(0);

    do {
      
      z = z * z + c;

      if(std::abs(z) >= _Z_ABS_BOUND_){

        return false;

      }

    } while(++iterations < iterationMax);

    return true;

  }

  void MandelbrotPixels::_createRGB(RGB& rgb, const int& iterations,
        const int& iterationMax)
  {

    // Bernstein polynomials method take from:
    // https://solarianprogrammer.com/2013/02/28/mandelbrot-set-cpp-11/

    // map n on the 0..1 interval
  	long double t = (long double)iterations/(long double)iterationMax;
  
  	// Use smooth polynomials for r, g, b
  	rgb.r = std::clamp<int>((int)(9*(1-t)*t*t*t*255), 0, 255);
  	rgb.g = std::clamp<int>((int)(15*(1-t)*(1-t)*t*t*255), 0, 255);
  	rgb.b = std::clamp<int>((int)(8.5*(1-t)*(1-t)*(1-t)*t*255), 0, 255);

  }
  
  void MandelbrotPixels::_pixels_(FractalPixels& pixels, 
    const int& start, const int& stop)
  {

    div_t quotRem;
    std::complex<long double> c;
    int iterations;
    
    for(int i = start; i < stop; i++){
      
      quotRem = std::div(i, _width);

      c.real(_xStep * ((long double)quotRem.quot) - _Z_ABS_BOUND_);
      c.imag(_Z_ABS_BOUND_ - _yStep * (long double)quotRem.rem);

      iterations = 0;
      
      if(!_inMandelbrotSet(c, iterations, _iterationMax_)) {
        
        auto& rgb = pixels.at(i);
        _createRGB(rgb, iterations, _iterationMax_);
        
      }

    }

  }

  // instance methods
  FractalPixels MandelbrotPixels::pixels(void)
  {

    int numPixels = _width * _height;
    
    // Create all RGB pixels upfront 
    FractalPixels fractalPixels(numPixels, RGB{});
    
    int start = 0;
    auto quotRem = std::div(numPixels, (_THREADS_ == 0 ? 1 : _THREADS_));
    int pixelChunk = quotRem.quot;
    int stop = pixelChunk;
    
    std::vector<std::future<void>> futures;

    //Launch threads minus 1 to account for thread this method is running in
    for(int i = 1; i < _THREADS_; i++){

      futures.push_back(std::async(std::launch::async,[
        start, // capture by value
        stop, // capture by value,
        &fractalPixels, // capture by refernce
        this
      ](){

        _pixels_(fractalPixels, start, stop);

      }));

      start = stop;
      stop += pixelChunk;

    }

    //Last loop adds pixelChunk
    stop += quotRem.rem;
    _pixels_(fractalPixels, start, stop);

    //Join threads
    for(const auto& future : futures){
      future.wait();
    }

    return fractalPixels;

  }

  FileOpenError::FileOpenError(std::string what): _what_(what) {}
  const char* FileOpenError::what(void){ return _what_.c_str(); }

  void saveImage(std::string fileName, int width, int height,
    int iterationMax)
  {
    MandelbrotPixels mandelbrotPixels(width, height, iterationMax);

    auto pixels = mandelbrotPixels.pixels();

    fileName.append(".ppm");

    std::ofstream image(fileName);

    if(!image){
      std::string msg("Could not open file: ");
      msg.append(fileName);
      throw FileOpenError(msg);
    }

    image << "P3\n" << width << " " << height << " 255\n";

    for(const auto& rgb : pixels){

      image << rgb.r << ' ' << rgb.g << ' ' << rgb.b << "\n";

    }

    image.close();

  }

}; // ns MandelbrotCreator