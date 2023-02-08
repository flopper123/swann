#pragma once
#include <iostream>
#include <fstream>
#include <exception>

std::istream& file2istream(std::string path) {
  std::filebuf fb;
  
  if (!fb.open(path, std::ios::in)) {
    throw "Could not open data file: " + path;
  }

  std::istream is(&fb);
  return is;
}
