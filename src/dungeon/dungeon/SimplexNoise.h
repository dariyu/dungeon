#ifndef __simplex_noise
#define __simplex_noise

namespace engine{
	class SimplexNoise{
	public:
		static double noise(double xin, double yin);
		static double noise(double xin, double yin, double zin);
		static void init();
	private:
		static int grad3[12][3];
		static int p[256];
		// To remove the need for index wrapping, double the permutation table length
		static int perm[512];
		// A lookup table to traverse the simplex around a given point in 4D.
		// Details can be found where this table is used, in the 4D noise method.
		static int simplex[256][4];
		// This method is a *lot* faster than using (int)Math.floor(x)
		// Editor remark: it's true for java, but I don't know how cmath's floor works.
		inline static int fastfloor(double x){
			return x > 0 ? (int)x : (int)x - 1;
		}
		inline static double dot(int g[2], double x, double y){
			return g[0] * x + g[1] * y;
		}
		inline static double dot(int g[3], double x, double y, double z){
			return g[0] * x + g[1] * y + g[2] * z;
		}
	};
}

#endif