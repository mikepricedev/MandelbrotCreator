#ifndef MANDELBROT_SET_H
#define MANDELBROT_SET_H

#include <utility>
#include <complex>
#include <future>
#include <vector>
#include <string>
#include <exception>

namespace MandelbrotCreator {
  
  struct RGB {
    int r = 0;
    int g = 0;
    int b = 0;
  };

  typedef std::vector<RGB> FractalPixels;


  class MandelbrotPixels {
    public:

      MandelbrotPixels(int width, int height, int iterationMax = 1024);

      FractalPixels pixels(void);


    protected:

      int _width;
      int _height;
      long double _xStep;
      long double _yStep;

      // When c is NOT in the Mandelbrot set, `iterations` is the iteration
      // number when |z| >= 2
      static bool _inMandelbrotSet(const std::complex<long double>& c,
        int& iterations, const int& iterationMax);

      static void _createRGB(RGB& rgb, const int& iterations,
        const int& iterationMax);

    private:
      
      MandelbrotPixels(void);
      
      int _iterationMax_;

      void _pixels_(FractalPixels& pixels, const int& start, const int& stop);
      
      static long double _Z_ABS_BOUND_;
      static unsigned int _THREADS_;
  
  };

  class FileOpenError : std::exception {
    public:
      FileOpenError(std::string what);
      const char* what(void);
    private:
      FileOpenError(void);
      std::string _what_;
  };

  void saveImage(std::string fileName, int width, int height,
    int iterationMax = 1024);

}; // ns MandelbrotCreator


#endif