#pragma once


inline D2D_POINT_2F operator+(D2D_POINT_2F a, const D2D_POINT_2F b) { return { a.x + b.x, a.y + b.y }; }
inline D2D_POINT_2F operator-(D2D_POINT_2F a, const D2D_POINT_2F b) { return { a.x - b.x, a.y - b.y }; }
inline D2D_POINT_2F operator*(float b, D2D_POINT_2F a) { return { static_cast<FLOAT>(a.x *b), static_cast<FLOAT>(a.y *b) }; }
inline bool operator==(D2D_POINT_2F p1, D2D_POINT_2F p2) { return (p1.x == p2.x && p1.y == p2.y) ? true : false; }
inline bool operator!=(D2D_POINT_2F p1, D2D_POINT_2F p2) { return (p1.x == p2.x && p1.y == p2.y) ? false : true; }

inline D2D_RECT_F operator+(D2D_RECT_F rc, D2D_POINT_2F pt) { return { pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom }; }
inline D2D_RECT_F operator+(D2D_POINT_2F pt, D2D_RECT_F rc) { return { pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom }; }
inline D2D_RECT_F operator+(D2D_RECT_F rc_1, D2D_RECT_F rc_2) { return { rc_1.left + rc_2.left, rc_1.top + rc_2.top, rc_1.right + rc_2.right, rc_1.bottom + rc_2.bottom }; }
