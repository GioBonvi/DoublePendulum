#ifndef COLOR_SCALE
#define COLOR_SCALE

#include <vector>
#include <string>
#include <png++/rgb_pixel.hpp>
#include "../DoublePendulum/DoublePendulum.hpp"

/*
 * A multi-color logarithmic scale.
 * 
 * The scale is composed by varoius "legs", each with a starting and an ending
 * color. Each leg is divided in multiple shades, smoothly transitioning from
 * the starting to the ending color.
 * 
 * For any given value the color is assigned based on the log10 of the value:
 * the integer part determines the leg and the decimal part the shade.
 * 
 * A color is stored as a png::rgb_pixel object.
 * 
 * Example:
 *  value = 316 => log10(value) = 2.5
 *  integer = 2 => third leg (starting from 0)
 *  decimal = 0.5 => 50% shade (in the middle between starting and ending color)
 */
class ColorScale {
    private:
        // The list of colors from lowest value to highest.
        std::vector<png::rgb_pixel> colors;
        // The color used to mark "out of scale" values.
        png::rgb_pixel outOfScaleColor;
        // How many colors each leg of the scale is composed of.
        int shadesNum;

        // Convert a hex color code in the corresponding color.
        static png::rgb_pixel hexCodeToRgbPixel(std::string hexCode);
    
    public:
        ColorScale();
        ColorScale(std::vector<std::string> colorHexCodes, int shadesNum = 100);

        // Assign a color to the value.
        png::rgb_pixel getColor(double value, double outOfScaleValue);
};

#endif