//-----------------------------------------------------------------------------
// Copyright (c) 2016 @hirodotexe. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef STRATEGO_POINT_H_
#define STRATEGO_POINT_H_

// 2d point, vector.
struct Point {
  Point Inverse() const {
    Point result;
    result.x = -x;
    result.y = -y;
    return result;
  }
  Point Add(const Point &point) const {
    Point result;
    result.x = x + point.x;
    result.y = y + point.y;
    return result;
  }
  Point Subtract(const Point &point) const {
    return Add(point.Inverse());
  }
  Point Average(const Point &point) const {
    Point result = Add(point);
    result.x /= 2;
    result.y /= 2;
    return result;
  }
  bool Equals(const Point &point) const {
    return (point.x == x && point.y == y);
  }

  int y;
  int x;
};

struct Move {
  Point src;
  Point dest;
};

#endif  // STRATEGO_POINT_H_