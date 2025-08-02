#ifndef __AUTILITY_AUMATH_H__
#define __AUTILITY_AUMATH_H__

#include "au_common.h"

#ifdef USE_AU_MATH

#ifndef USE_AU_TYPES
#error Must define USE_AU_TYPES if using math define.
#endif

#include "au_types.h"

#include <math.h> 
#include <cfloat>

namespace au {
	namespace math {

		using namespace types;

		static const double pi = 3.1415926535897932384626433832795;

		AU_INLINE static double deg2rad(double deg)
		{
			return deg * (pi / 180);
		}

		AU_INLINE static double rad2deg(double rad)
		{
			return rad * (180 / pi);
		}

		AU_INLINE static bool get_2d_intersection_point(double2 p1, double2 p2, double2 p3, double2 p4, double2* o_corss_point)
		{
			// f(x) = ax + by = c = 0
			double a1, b1, c1;
			double a2, b2, c2;

			a1 = p1.y - p2.y;
			b1 = p2.x - p1.x;
			c1 = (p1.x * p2.y) - (p2.x * p1.y);

			a2 = p3.y - p4.y;
			b2 = p4.x - p3.x;
			c2 = (p3.x * p4.y) - (p4.x * p3.y);

			// get cross point
			double D = (a1 * b2) - (a2 * b1);
			// parallel
			if (D == 0)
			{
				return false;
			}
			else
			{
				double x = ((b1 * c2) - (b2 * c1)) / D;
				double y = ((a2 * c1) - (a1 * c2)) / D;
				*o_corss_point = double2(x, y);
				return true;
			}
		}

		AU_INLINE static double2 cramer_formula_binary_linear_equation(double paraA1, double paraB1, double paraC1, double paraA2, double paraB2, double paraC2)
		{
			// a1 * x + b1 * y = c1
			// a2 * x + b2 * y = c2
			//
			// delta = | a1 b1 | = a1b2 - b1a2
			//         | a2 b2 |
			//
			// delta(x) = | c1 b1 | = c1b2 - b1c2
			//			  | c2 b2 |
			//
			// delta(y) = | a1 c1 | = a1c2 - c1a2
			//			  | a2 c2 |

			// x = delta(x) / delta = (c1b2 - b1c2) / (a1b2 - b1a2)
			// y = delta(y) / delta = (a1c2 - c1a2) / (a1b2 - b1a2)


			double divid = (paraA1 * paraB2) - (paraB1 * paraA2);
			if (fabs(divid) <= DBL_EPSILON)
			{
				divid = 0.0001;
			}

			double x = ((paraC1 * paraB2) - (paraB1 * paraC2)) / (divid);
			double y = ((paraA1 * paraC2) - (paraC1 * paraA2)) / (divid);
			return double2(x, y);
		}

		AU_INLINE static bool check_if_2_line_same_plane_intersection(double3 _point1, double3 _vec1, double3 _point2, double3 _vec2, double3* o_interset_31)
		{
			// point 1
			double x1 = _point1.x;
			double y1 = _point1.y;
			double z1 = _point1.z;
			// vector 1
			double a1 = _vec1.x;
			double b1 = _vec1.y;
			double c1 = _vec1.z;
			// point 2
			double x2 = _point2.x;
			double y2 = _point2.y;
			double z2 = _point2.z;
			// vector 2
			double a2 = _vec2.x;
			double b2 = _vec2.y;
			double c2 = _vec2.z;

			double cramer_x_para_al = a1;
			double cramer_x_para_bl = -a2;
			double cramer_x_para_c1 = x2 - x1;
			bool x_all_para_is_zero = (cramer_x_para_al == 0) && (cramer_x_para_bl == 0) && (cramer_x_para_c1 == 0);

			double cramer_y_para_al = b1;
			double cramer_y_para_bl = -b2;
			double cramer_y_para_c1 = y2 - y1;
			bool y_all_para_is_zero = (cramer_y_para_al == 0) && (cramer_y_para_bl == 0) && (cramer_y_para_c1 == 0);

			double cramer_z_para_al = c1;
			double cramer_z_para_bl = -c2;
			double cramer_z_para_c1 = z2 - z1;
			bool z_all_para_is_zero = (cramer_z_para_al == 0) && (cramer_z_para_bl == 0) && (cramer_z_para_c1 == 0);

			double paraA1, paraB1, paraC1, paraA2, paraB2, paraC2;

			if (!x_all_para_is_zero && !y_all_para_is_zero) {
				paraA1 = cramer_x_para_al;
				paraB1 = cramer_x_para_bl;
				paraC1 = cramer_x_para_c1;

				paraA2 = cramer_y_para_al;
				paraB2 = cramer_y_para_bl;
				paraC2 = cramer_y_para_c1;
			}
			else if (!x_all_para_is_zero && !z_all_para_is_zero) {
				paraA1 = cramer_x_para_al;
				paraB1 = cramer_x_para_bl;
				paraC1 = cramer_x_para_c1;

				paraA2 = cramer_z_para_al;
				paraB2 = cramer_z_para_bl;
				paraC2 = cramer_z_para_c1;
			}
			else if (!y_all_para_is_zero && !z_all_para_is_zero) {
				paraA1 = cramer_y_para_al;
				paraB1 = cramer_y_para_bl;
				paraC1 = cramer_y_para_c1;

				paraA2 = cramer_z_para_al;
				paraB2 = cramer_z_para_bl;
				paraC2 = cramer_z_para_c1;
			}
			else {
				paraA1 = paraB1 = paraC1 = paraA2 = paraB2 = paraC2 = 0;
			}


			double2 ts = cramer_formula_binary_linear_equation(paraA1, paraB1, paraC1, paraA2, paraB2, paraC2);
			double t = ts.x;
			double s = ts.y;

			//linear equation L1
			double rx1 = x1 + a1 * t;
			double ry1 = y1 + b1 * t;
			double rz1 = z1 + c1 * t;

			//linear equation L2
			double rx2 = x2 + a2 * s;
			double ry2 = y2 + b2 * s;
			double rz2 = z2 + c2 * s;

			if (rx1 == rx2 && ry1 == ry2 && rz1 == rz2)
			{
				*o_interset_31 = double3(rx1, ry1, rz1);
				return true;
			}
			else
			{
				return false;
			}
		}

		//https://www.itread01.com/content/1545016334.html
		inline static double3 get_3d_closer_point(double3 _p1, double3 _p2, double3 _p3, double3 _p4)
		{
			double v1[3];
			double v2[3];

			for (size_t i = 0; i < 3; ++i)
			{
				v1[i] = _p2.v[i] - _p1.v[i];
				v2[i] = _p4.v[i] - _p3.v[i];

				//v1[i] = dData[1][i] - dData[0][i];
				//v2[i] = dData[3][i] - dData[2][i];
			}


			double x1 = _p1.v[0];
			double y1 = _p1.v[1];
			double z1 = _p1.v[2];

			double x3 = _p3.v[0];
			double y3 = _p3.v[1];
			double z3 = _p3.v[2];

			double a1 = v1[0];
			double b1 = v1[1];
			double c1 = v1[2];

			double a2 = v2[0];
			double b2 = v2[1];
			double c2 = v2[2];

			double3 out;
			bool check = check_if_2_line_same_plane_intersection(double3(x1, y1, z1), double3(a1, b1, c1), double3(x3, y3, z3), double3(a2, b2, c2), &out);
			if (check)
			{
				return out;
			}

			double a3 = b1 * c2 - b2 * c1;
			double b3 = c1 * a2 - c2 * a1;
			double c3 = a1 * b2 - a2 * b1;

			if (fabs(a3) <= DBL_EPSILON)
			{
				a3 = 0.0001;
			}
			if (fabs(a1) <= DBL_EPSILON)
			{
				a1 = 0.0001;
			}

			if (fabs(a2) <= DBL_EPSILON)
			{
				a2 = 0.0001;
			}

			double x5h = a3 * x3 + b3 * b3 * x1 / a3 - b3 * (y1 - y3) + c3 * c3 * x1 / a3 - c3 * (z1 - z3);
			double x51 = a3 + b3 * b3 / a3 + c3 * c3 / a3;
			if (fabs(x51) <= DBL_EPSILON)
			{
				x51 = 0.0001;
			}
			double x5 = x5h / x51;

			double y5 = b3 * (x5 - x1) / a3 + y1;
			double z5 = c3 * (x5 - x1) / a3 + z1;

			double x6h = b1 * x5 / a1 - y5 - b2 * x3 / a2 + y3;
			double x61 = b1 / a1 - b2 / a2;
			if (fabs(x61) <= DBL_EPSILON)
			{
				x61 = 0.0001;
			}
			double x6 = x6h / x61;

			double y6 = b1 * (x6 - x5) / a1 + y5;
			double z6 = c1 * (x6 - x5) / a1 + z5;

			double x7h = b3 * x6 / a3 - y6 - b1 * x1 / a1 + y1;
			double x71 = b3 / a3 - b1 / a1;
			if (fabs(x71) <= DBL_EPSILON)
			{
				x71 = 0.0001;
			}
			double x7 = x7h / x71;

			double y7 = b3 * (x7 - x6) / a3 + y6;
			double z7 = c3 * (x7 - x6) / a3 + z6;

			double xn = (x6 + x7) / 2;
			double yn = (y6 + y7) / 2;
			double zn = (z6 + z7) / 2;



			double3 closer_point(xn, yn, zn);

			return closer_point;
		}
	}
}


#endif //USE_AU_MATH

#endif // __AUTILITY_MATH_H__