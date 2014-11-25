#pragma once
#include "Color.h"
#include "Constants.h"

//Radiance is between 0 and 1

/// <summary>
/// Class that encapsualtes an image that can be drawn
/// </summary>
class Image {
private:
	int m_width;
	int m_height;
	std::vector<Radiance3> m_data;

	int PPMGammaEncode(float radiance, float displayConstant) const;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="Image"/> class.
	/// </summary>
	/// <param name="width">The width.</param>
	/// <param name="height">The height.</param>
	/// <param name="defaultRadiance">The default radiance.</param>
	Image(int width = 100, int height = 100, Radiance3 defaultRadiance = Radiance3(0.0f, 0.0f, 0.0f)) :
		m_width(width), m_height(height), m_data(width * height, defaultRadiance)
	{}

	~Image(){}

	int width() const { return m_width; }
	int height() const { return m_height; }


	/// <summary>
	/// Sets a pixel.
	/// </summary>
	/// <param name="x">The x.</param>
	/// <param name="y">The y.</param>
	/// <param name="value">The new Radiance3 value.</param>
	void setPixel(int x, int y, Radiance3 value) {
		m_data[x + y * m_width] = value;
	}


	/// <summary>
	/// Gets a pixel.
	/// </summary>
	/// <param name="x">The x.</param>
	/// <param name="y">The y.</param>
	/// <returns>The Radiance3 at that pixel</returns>
	const Radiance3& getPixel(int x, int y) const {
		return m_data[x + y * m_width];
	}

	/// <summary>
	/// Saves the specified image to a file.
	/// </summary>
	/// <param name="filename">The filename.</param>
	/// <param name="displayConstant">The display constant that brightens the image.</param>
	void save(const std::string& filename, float displayConstant = DISPLAY_BRIGHTNESS) const;
};