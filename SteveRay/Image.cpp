#include "stdafx.h"
#include "Image.h"

int Image::PPMGammaEncode(float radiance, float displayConstant) const {
	//clamp radiance to 0 and 1.0, then gamma correct it.  
	return int(pow(std::min(1.0f, std::max(0.0f, radiance * displayConstant)), GAMMA_CORRECTION) * 255.0f);
}

void Image::save(const std::string& filename, float displayConstant) const {
	FILE* file = fopen(filename.c_str(), "wt");

	fprintf(file, "P3 %d %d 255\n", m_width, m_height);

	for (int y = 0; y < m_height; ++y) {
		fprintf(file, "\n# y = %d\n", y);

		for (int x = 0; x < m_width; ++x) {
			const Radiance3& c(getPixel(x, y));
			fprintf(file, "%d %d %d\n",
				PPMGammaEncode(c.x(), displayConstant),
				PPMGammaEncode(c.y(), displayConstant),
				PPMGammaEncode(c.z(), displayConstant));
		}
	}

	fclose(file);
}
