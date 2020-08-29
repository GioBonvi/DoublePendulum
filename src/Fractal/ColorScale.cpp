#include <cmath>
#include <sstream>
#include "ColorScale.hpp"

png::rgb_pixel ColorScale::hexCodeToRgbPixel(std::string hexCode) {
    std::string clenHexCode;
    int red, green, blue;

    if(hexCode.at(0) == '#') {
        clenHexCode = hexCode.erase(0, 1);
    } else {
        clenHexCode = hexCode;
    }

    std::istringstream(clenHexCode.substr(0,2)) >> std::hex >> red;
    std::istringstream(clenHexCode.substr(2,2)) >> std::hex >> green;
    std::istringstream(clenHexCode.substr(4,2)) >> std::hex >> blue;

    return png::rgb_pixel(red, green, blue);
}


ColorScale::ColorScale(std::vector<std::string> colorHexCodes, int shadesNum) {
    png::rgb_pixel startColor, endColor;

    this->shadesNum = shadesNum;
    this->outOfScaleColor = ColorScale::hexCodeToRgbPixel(*std::prev(colorHexCodes.end()));

    auto divideRange = [](uint start, uint end, int subdivisions, int index) {
        return (uint) (start + (end - start) / ((float) subdivisions - 1) * index);
    };

    for (uint i = 0; i < colorHexCodes.size() - 1; i+=2) {
        startColor = ColorScale::hexCodeToRgbPixel(colorHexCodes[i]);
        endColor = ColorScale::hexCodeToRgbPixel(colorHexCodes[i + 1]);

        for (int j = 0; j < shadesNum; j++) {
            this->colors.push_back(png::rgb_pixel(
                divideRange(startColor.red, endColor.red, shadesNum, j),
                divideRange(startColor.green, endColor.green, shadesNum, j),
                divideRange(startColor.blue, endColor.blue, shadesNum, j)
            ));
        }
    }
}

// Default color scale.
ColorScale::ColorScale() : ColorScale(
    std::vector<std::string> {
        "#000000", "#000000", // Black      x < 1
        "#040085", "#47a9ff", // Blue       x in (1; 10]
        "#00631e", "#47d171", // Green      x in (10; 100]
        "#8f0000", "#ff8080", // Red        x in (100; 1000]
        "#4b0066", "#e18fff", // Purple     x > 1000
        "#FFFFFF"             // White      x out of scale
    },
    100
) {};

png::rgb_pixel ColorScale::getColor(double value, double outOfScaleValue) {
    uint colorIndex;
    
    if (value == outOfScaleValue) {
        return this->outOfScaleColor;
    }

    colorIndex = this->shadesNum * (log10(value) + 1);
    if (colorIndex < 0) {
        // Flatten all values below the 0 to the minimum.
        colorIndex = 0;
    }
    if (colorIndex >= this->colors.size()) {
        // Flatten all values above the maximum to out of scale.
        return this->outOfScaleColor;
    }

    return this->colors[colorIndex];
}
