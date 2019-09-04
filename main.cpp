#include "mandelbrot.h"

#include <string>;

#define NOT_FOUND std::string::npos

int main(int argc, char *argv[]) {

  int width = 1024;
  int height = 1024;
  std::string fileName("mandelbrotSetImage");

  for(int i = 0; i < argc; i++){

    std::string arg(argv[i]);
   
    if(arg.find("--help") != NOT_FOUND) {
    
      puts("Mandelbrot Creator");
      puts("  --help");
      puts("  -w, --width [int] default: 1024");
      puts("  -h, --height [int] default: 1024");
      puts("  -f, --file [string] default: mandelbrotSetImage");
      
      return 0;
    
    } else if(arg.find("-w") != NOT_FOUND || arg.find("--width") != NOT_FOUND) {
   
      if(++i < argc) {
        width = std::stoi(argv[i]);
      }
   
    } else if(arg.find("-h") != NOT_FOUND || arg.find("--height") != NOT_FOUND) {
   
      if(++i < argc) {
        height = std::stoi(argv[i]);
      }
   
    } else if(arg.find("-f") != NOT_FOUND || arg.find("--filename") != NOT_FOUND) {
   
      if(++i < argc) {
        fileName.clear();
        fileName.append(argv[i]);
   
      }
   
    }

  }
  
  try {
  
    MandelbrotCreator::saveImage(fileName, width, height);
  
  } catch (MandelbrotCreator::FileOpenError& err) {
  
    puts(err.what());
    return 1;
  
  }

  std::string msg("Success! Mandelbrot image saved: ");
  msg.append(fileName);
  
  return 0;

}