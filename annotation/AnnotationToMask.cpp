#include "AnnotationToMask.h"
#include "AnnotationList.h"
#include "Annotation.h"
#include "AnnotationGroup.h"
#include "MultiResolutionImageWriter.h"
#include "core/Box.h"
#include "core/ProgressMonitor.h"

void AnnotationToMask::setProgressMonitor(ProgressMonitor* monitor) {
  _monitor = monitor;
}

void AnnotationToMask::convert(const AnnotationList* const annotationList, const std::string& maskFile, const std::vector<unsigned long long>& dimensions, const std::vector<double>& spacing, const std::map<std::string, int> nameToLabel) const {
	std::vector<Annotation*> annotions = annotationList->getAnnotations();
	MultiResolutionImageWriter writer;
  if (_monitor) {
    writer.setProgressMonitor(_monitor);
  }
	if (writer.openFile(maskFile) == 0) {
		writer.setColorType(pathology::ColorType::Monochrome);
		writer.setCompression(pathology::Compression::LZW);
		writer.setTileSize(512);
		writer.setDataType(pathology::DataType::UChar);
		writer.setInterpolation(pathology::Interpolation::NearestNeighbor);
		writer.setOverrideSpacing(spacing);
		writer.writeImageInformation(dimensions[0], dimensions[1]);
		unsigned char* buffer = new unsigned char[512 * 512];
		for (unsigned long long ty = 0; ty < dimensions[1]; ty += 512) {
			for (unsigned long long tx = 0; tx < dimensions[0]; tx += 512) {
				std::fill(buffer, buffer + 512 * 512, 0);
        for (std::vector<Annotation*>::const_iterator annotation = annotions.begin(); annotation != annotions.end(); ++annotation) {
          if (!nameToLabel.empty() && !(*annotation)->getGroup()) {
            continue;
          }
          std::string nm = (*annotation)->getName();
          std::vector<Point> coords = (*annotation)->getCoordinates();
          std::vector<Point> bbox = (*annotation)->getImageBoundingBox();
          if (!coords.empty()) {
            coords.push_back(coords[0]);
          }
          int label = 1;
          if (!nameToLabel.empty()) {
            std::map<std::string, int>::const_iterator it = nameToLabel.find((*annotation)->getGroup()->getName());
            if (it != nameToLabel.end()) {
              label = it->second;
            }
            else {
              label = 0;
            }
          }
          for (unsigned int y = 0; y < 512; ++y) {
            if (ty + y > bbox[0].getY() && ty + y < bbox[1].getY()) {
              for (unsigned int x = 0; x < 512; ++x) {
                if (tx + x > bbox[0].getX() && tx + x < bbox[1].getX()) {                  
                  if (buffer[y * 512 + x] == 0) {
                    int in_poly = wn_PnPoly(Point(tx + x, ty + y), coords) != 0 ? 1 : 0;
                    buffer[y * 512 + x] = in_poly * label;
                  }
                }
              }
            }
          }
        }
			  writer.writeBaseImagePart((void*)buffer);
			}
		}
		writer.finishImage();
		delete[] buffer;
	}
}

int AnnotationToMask::cn_PnPoly(const Point& P, const std::vector<Point>& V) const {
  int    cn = 0;    // the  crossing number counter

  // loop through all edges of the polygon
  for (int i = 0; i < V.size() - 1; i++) {    // edge from V[i]  to V[i+1]
    if (((V[i].getY() <= P.getY()) && (V[i + 1].getY() > P.getY()))     // an upward crossing
      || ((V[i].getY() > P.getY()) && (V[i + 1].getY() <= P.getY()))) { // a downward crossing
      // compute  the actual edge-ray intersect x-coordinate
      float vt = (float)(P.getY() - V[i].getY()) / (V[i + 1].getY() - V[i].getY());
      if (P.getX() <  V[i].getX() + vt * (V[i + 1].getX() - V[i].getX())) // P.x < intersect
        ++cn;   // a valid crossing of y=P.y right of P.x
    }
  }
  return (cn & 1);
}

int AnnotationToMask::wn_PnPoly(const Point& P, const std::vector<Point>& V) const {
  int    wn = 0;    // the  winding number counter

  // loop through all edges of the polygon
  for (int i = 0; i<V.size() - 1; i++) {   // edge from V[i] to  V[i+1]
    if (V[i].getY() <= P.getY()) {          // start y <= P.y
      if (V[i + 1].getY()  > P.getY())      // an upward crossing
        if (isLeft(V[i], V[i + 1], P) > 0)  // P left of  edge
          ++wn;            // have  a valid up intersect
    }
    else {                        // start y > P.y (no test needed)
      if (V[i + 1].getY() <= P.getY())     // a downward crossing
        if (isLeft(V[i], V[i + 1], P) < 0)  // P right of  edge
          --wn;            // have  a valid down intersect
    }
  }
  return wn;
}