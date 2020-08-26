#!/usr/bin/env python3

from PIL import Image, ImageDraw 
from colour import Color
import argparse
from math import log10

def colorDecimalTo254(tup):
    """Converts an RGB tuple from a 0-1 scale to a 0-254 scale."""
    
    # PIL works with the 254 scale, while the colour package works with a 0-1 scale.
    return (int(tup[0] * 254), int(tup[1] * 254), int(tup[2] * 254))

parser = argparse.ArgumentParser(
    description="Generates the fractal image from the data.",
    formatter_class=argparse.RawTextHelpFormatter
)
parser.add_argument(
    "input_file",
    help="The input file to read the data from.",
    type=str
)
parser.add_argument(
    "output_file",
    help="The file to save the image into.",
    type=str
)
parser.add_argument(
    "--separator",
    help="The separator used by the input file.\nDefaults to \\t.",
    type=str,
    nargs="?",
    default="\t",
    const="\t"
)
parser.add_argument(
    "--basesteps",
    help="The unit number of steps to base the scale coloration on (only affects the multilog scale).\nDefaults to 1.",
    type=int,
    nargs="?",
    default=1,
    const=1
)
parser.add_argument(
    "--scale",
    help="The scale used to assign colors to the values.\nDefaults to multilog.",
    type=str,
    nargs="?",
    choices=["multilog", "log", "linear"],
    default="multilog",
    const="multilog"
)
parser.add_argument(
    "--ncolors",
    help="Limit the number of colors used in the scale.",
    type=int,
    nargs="?",
    default=None,
    const=None
)
args = parser.parse_args()

# If the color gradient is applied on the multilog scale this number of steps
# acts as a base unit of time to define the absolute scale.
# The other two scales are relative and do not need this.
base_steps = args.basesteps
separator = args.separator

# Color gradient
color_grad_subdivisions = 1000
all_scale_colors = [
    [Color("black"), Color("black")],
    [Color("#040085"), Color("#47a9ff")], # Blue
    [Color("#00631e"), Color("#47d171")], # Green
    [Color("#8f0000"), Color("#ff8080")], # Red
    [Color("#4b0066"), Color("#e18fff")]  # Purple
]
color_outofscale = Color("white")
if args.ncolors is None or args.ncolors < 1 or args.ncolors > len(all_scale_colors):
    scale_colors = all_scale_colors
else:
    scale_colors = all_scale_colors[0:args.ncolors]

color_gradient = [color for colors in scale_colors for color in colors[0].range_to(colors[1], color_grad_subdivisions)]

x_coords = []
y_coords = []
l_dim = []
values = []

with open(args.input_file, "r") as f:
    for line in f.readlines():
        if line[0] == "#":
            continue
        [x, y, l, val] = line.split(separator)
        x_coords.append(float(x))
        y_coords.append(float(y))
        l_dim.append(float(l))
        values.append(float(val))

value_max = max(values)

resolution = min(l_dim)
orig_space = [[-3, +3], [-3, +3]]
img_dim = (round((orig_space[0][1] - orig_space[0][0]) / resolution), round((orig_space[1][1] - orig_space[1][0]) / resolution))
img_space = [[0, img_dim[0] - 1], [0, img_dim[1] - 1]]

print("Image resolution:", img_dim)

img = Image.new("RGB", img_dim, color="white")

for x, y, l, val in zip(x_coords, y_coords, l_dim, values):
    # Map xy coordinates to image pixels.
    rectangle_dim = [
        (x - orig_space[0][0]) / ((orig_space[0][1] - orig_space[0][0]) / img_dim[0]) - round((l / resolution - 1) / 2),
        (y - orig_space[1][0]) / ((orig_space[1][1] - orig_space[1][0]) / img_dim[1]) - round((l / resolution - 1) / 2),
        (x - orig_space[0][0]) / ((orig_space[0][1] - orig_space[0][0]) / img_dim[0]) + round((l / resolution - 1) / 2),
        (y - orig_space[1][0]) / ((orig_space[1][1] - orig_space[1][0]) / img_dim[1]) + round((l / resolution - 1) / 2)
    ]
    # Flip the coordinate system around the y axis:
    # PIL has x positive from left to right and y positive top to bottom, while
    # the image data refers to a system with x positive from left to right and 
    # y positive from bottom to top.
    rectangle_dim[1] = img_dim[1] - rectangle_dim[1] - 1
    rectangle_dim[3] = img_dim[1] - rectangle_dim[3] - 1

    color = None
    if val == value_max:
        # Without this line values which are out of scale (but not left out from the data)
        # will result in the top color of the scale instead of color_outofscale.
        color = color_outofscale
    elif args.scale == "linear":
        # Linear relative scale (minimum = first color, maximum = last color)
        color_i = int(val / value_max * (len(color_gradient) - 1))
        color = color_gradient[color_i]
    elif args.scale == "log":
        # Linear logarithmic scale (minimum = first color, maximum = last color)
        color_i = int(log10(val) / log10(value_max) * (len(color_gradient) - 1))
        color = color_gradient[color_i]
    elif args.scale == "multilog":
        # Absolute logarithmic scale: base color is decided by the integer part of
        # log10(v / base_steps), tint by the decimal part.
        color_i = log10(val / base_steps) + 1
        if color_i < 0:
            # No negative values.
            color_i = 0
        color_i = int(color_i * color_grad_subdivisions)

        if color_i >= len(color_gradient):
            color = color_outofscale
        else:
            color = color_gradient[color_i]
    new_pixel = colorDecimalTo254(color.get_rgb())
    ImageDraw.Draw(img).rectangle(rectangle_dim, fill=new_pixel)

img.save(args.output_file)



