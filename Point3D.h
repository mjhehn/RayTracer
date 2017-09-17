#ifndef POINT3D_H_INCLUDED
#define POINT3D_H_INCLUDED

#include<iostream>
using std::ostream;

//! Real-valued points in 3 space, i.e. (x, y, z)
/*! 
  Point3D is the base class for storing points in 3 space.
 */
class Point3D {
public:
  /// Constructor with up to 3 coordinate arguments. All arguments default to 0.
  Point3D(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}

  inline double X() const {return x;}          //!< Return the X coordinate of the point
  inline double Y() const {return y;}          //!< Return the Y coordinate of the point
  inline double Z() const {return z;}          //!< Return the Z coordindate of the point

protected:
  double x;                                     //!< x coordinate (real value)
  double y;                                     //!< y coordinate (real value)
  double z;                                     //!< z coordinate (real value)
};

/// Overload of << operator for Point3D. Prints out as three real values, separated by spaces with a trailing space.
inline ostream& operator << (ostream& ostr, const Point3D& pt) {ostr << pt.X() << " " << pt.Y() << " " << pt.Z() << " "; return ostr;}

#endif // POINT3D_H_INCLUDED
