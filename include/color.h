// color.h - extension of base.h...
// but for COLOR!!!!
// public domain'd
// https://tryh4rd.dev/
// also btw all "Color*" structs are assumed to be CIE versions unless
// explicitly stated otherwise

#pragma once

#include "base.h" // depends on base.h, https://github.com/shinmorisawa/base for more info

#define MAX_HANDLERS 4

typedef struct ColorRGB {
    double r, g, b; 
} ColorRGB; // linear sRGB, D65 (yes i did my research)

typedef struct ColorLAB {
    double l, a, b;
} ColorLAB;

typedef struct ColorLCH {
    double l, c, h;
} ColorLCH;

typedef struct ColorOKLAB {
    double l, a, b;
} ColorOKLAB;

typedef struct ColorOKLCH {
    double l, c, h;
} ColorOKLCH;

typedef struct ColorXYZ {
    double x, y, z;
} ColorXYZ;

typedef enum ColorSpaces {
    RGB = (1 << 0),
    LAB = (1 << 1),
    LCH = (1 << 2),
    OKLAB = (1 << 3),
    OKLCH = (1 << 4),
    XYZ = (1 << 5)
} ColorSpaces;

typedef struct ColorHandler {
    void (*color_rgb)(ColorRGB);
    void (*color_lab)(ColorLAB);
    void (*color_lch)(ColorLCH);
    void (*color_oklab)(ColorOKLAB);
    void (*color_oklch)(ColorOKLCH);
    void (*color_xyz)(ColorXYZ);
    ColorSpaces supported_spaces;
} ColorHandler;

typedef struct Color {
    union {
        ColorRGB color_rgb;
        ColorLAB color_lab;
        ColorLCH color_lch;
        ColorOKLAB color_oklab;
        ColorOKLCH color_oklch;
        ColorXYZ color_xyz;
    };

    ColorSpaces type;
} Color;

// beware:
// t is assumed to be between 0,1
// t is normalized
// same goes with all the colors
// so 0,1 instead of 0,255 for ColorRGB for instance
// here be dragons!

// ALL -> RGB   [x]
// ALL -> LAB   [x]
// ALL -> LCH   [x]
// ALL -> OKLab [x]
// ALL -> OKLch [x]
// ALL -> XYZ   [x]

// CIE XYZ to linear sRGB D65
ColorRGB color_xyz_to_rgb(ColorXYZ xyz);
// CIE LAB to linear sRGB D65
ColorRGB color_lab_to_rgb(ColorLAB lab);
// CIE LCH to linear sRGB D65
ColorRGB color_lch_to_rgb(ColorLCH lch);
// OKLab to linear sRGB D65
ColorRGB color_oklab_to_rgb(ColorOKLAB oklab);
// OKLch to linear sRGB D65
ColorRGB color_oklch_to_rgb(ColorOKLCH oklch);

// CIE XYZ to CIE LAB
ColorLAB color_xyz_to_lab(ColorXYZ xyz);
// CIE LCH to CIE LAB
ColorLAB color_lch_to_lab(ColorLCH lch);
// OKLab to CIE LAB
ColorLAB color_oklab_to_lab(ColorOKLAB oklab);
// OKLch to CIE LAB
ColorLAB color_oklch_to_lab(ColorOKLCH oklch);

// CIE XYZ to CIE LCH
ColorLCH color_xyz_to_lch(ColorXYZ xyz);
// CIE LAB to CIE LCH
ColorLCH color_lab_to_lch(ColorLAB lab);
// OKLab to CIE LCH
ColorLCH color_oklab_to_lch(ColorOKLAB oklab);
// OKLch to CIE LCH
ColorLCH color_oklch_to_lch(ColorOKLCH oklch);

// CIE XYZ to OKLab
ColorOKLAB color_xyz_to_oklab(ColorXYZ xyz);
// CIE LAB to OKLab
ColorOKLAB color_lab_to_oklab(ColorLAB lab);
// CIE LCH to OKLab
ColorOKLAB color_lch_to_oklab(ColorLCH lch);
// OKLch to OKLab
ColorOKLAB color_oklch_to_oklab(ColorOKLCH oklch);

// OKLab to OKLch
ColorOKLCH color_oklab_to_oklch(ColorOKLAB oklab);
// CIE XYZ to OKLch
ColorOKLCH color_xyz_to_oklch(ColorXYZ xyz);
// CIE LAB to OKLch
ColorOKLCH color_lab_to_oklch(ColorLAB lab);
// CIE LCH to OKLch
ColorOKLCH color_lch_to_oklch(ColorLCH lch);

// CIE LAB to CIE XYZ
ColorXYZ color_lab_to_xyz(ColorLAB lab);
// CIE LCH to CIE XYZ
ColorXYZ color_lch_to_xyz(ColorLCH lch);
// OKLab to CIE XYZ
ColorXYZ color_oklab_to_xyz(ColorOKLAB oklab);
// OKLch to CIE XYZ
ColorXYZ color_oklch_to_xyz(ColorOKLCH oklch);

// LERP!!!!
// Linear interpolate between 2 CIE XYZ colors
ColorXYZ color_xyz_lerp(ColorXYZ start, ColorXYZ end, double t);
// Linear interpolate between 2 CIE LAB colors
ColorLAB color_lab_lerp(ColorLAB start, ColorLAB end, double t);
// Linear interpolate between 2 CIE LCH colors
ColorLCH color_lch_lerp(ColorLCH start, ColorLCH end, double t);
// Linear interpolate between 2 OKLab colors
ColorOKLAB color_oklab_lerp(ColorOKLAB start, ColorOKLAB end, double t);
// Linear interpolate between 2 OKLch colors
ColorOKLCH color_oklch_lerp(ColorOKLCH start, ColorOKLCH end, double t);

void color_install_handler(ColorHandler handler);
void color_run_handler(u64 index, Color color);

ColorRGB linear_to_srgb(ColorRGB rgb);

#ifndef NO_COLOR_IMPLEMENTATION
#include <stdio.h>
#include <math.h>

// matrices and stuff
static double xyz_rgb_mat[3][3] = {
    { 3.2406255, -1.5372080, -0.4986286 },
    { -0.9689307, 1.8757561, 0.0415175 },
    { 0.0557101, -0.2040211, 1.0569959 }
};

static double xyz_oklab_mat1[3][3] = {
    { 0.8189330101, 0.3618667424, -0.1288597137 },
    { 0.0329845436, 0.9293118715, 0.0361456387 },
    { 0.0482003018, 0.2643662691, 0.6338517070 }
};

static double xyz_oklab_mat2[3][3] = {
    { 0.2104542553, 0.7836177850, -0.0040720468 },
    { 1.9779984951, -2.4285922050, 0.4505937099 },
    { 0.0259040371, 0.7827717662, -0.8086757660 }
};

static double xyz_oklab_mat1_i[3][3] = {
    { 1.2270138511035211, -0.5577999806518222, 0.2812561489664678 },
    { -0.0405801784232806, 1.11225686961683, -0.0716766786656012 },
    { -0.0763812845057069, -0.4214819784180126, 1.5861632204407947 }
};

static double xyz_oklab_mat2_i[3][3] = {
    { 1.0101010086264997, 0.39527151599023685, 0.21515876639733963 },
    { 1.0101010191631066, -0.10662761851830986, -0.06449916644185308 },
    { 1.0101010654162885, -0.09055045833844474, -1.2921305295637735 }
};

static double lab_delta = 6.0 / 29.0;
static double lab_d65[3] = {
    95.0489 / 100.0,
    100.0 / 100.0,
    108.8840 / 100.0
};

// handlers
static ColorHandler handlers[MAX_HANDLERS] = {0};
static u64 handler_count = 0;

ColorRGB color_xyz_to_rgb(ColorXYZ xyz) {
    ColorRGB rgb = {0};
    rgb.r = xyz_rgb_mat[0][0] * xyz.x + xyz_rgb_mat[0][1] * xyz.y + xyz_rgb_mat[0][2] * xyz.z;
    rgb.g = xyz_rgb_mat[1][0] * xyz.x + xyz_rgb_mat[1][1] * xyz.y + xyz_rgb_mat[1][2] * xyz.z;
    rgb.b = xyz_rgb_mat[2][0] * xyz.x + xyz_rgb_mat[2][1] * xyz.y + xyz_rgb_mat[2][2] * xyz.z;

    if (rgb.r > 1)
        rgb.r = 1;
    if (rgb.g > 1)
        rgb.g = 1;
    if (rgb.b > 1)
        rgb.b = 1;

    if (rgb.r < 0)
        rgb.r = 0;
    if (rgb.g < 0)
        rgb.g = 0;
    if (rgb.b < 0)
        rgb.b = 0;

    return rgb;
}

ColorRGB color_lab_to_rgb(ColorLAB lab) {
    ColorXYZ xyz = color_lab_to_xyz(lab);
    ColorRGB rgb = color_xyz_to_rgb(xyz);
    return rgb;
}

ColorRGB color_lch_to_rgb(ColorLCH lch) {
    ColorLAB lab = color_lch_to_lab(lch);
    ColorXYZ xyz = color_lab_to_xyz(lab);
    ColorRGB rgb = color_xyz_to_rgb(xyz);
    return rgb;
}

ColorRGB color_oklab_to_rgb(ColorOKLAB oklab) {
    ColorXYZ xyz = color_oklab_to_xyz(oklab);
    ColorRGB rgb = color_xyz_to_rgb(xyz);
    return rgb;
}

ColorRGB color_oklch_to_rgb(ColorOKLCH oklch) {
    ColorOKLAB oklab = color_oklch_to_oklab(oklch);
    ColorXYZ xyz = color_oklab_to_xyz(oklab);
    ColorRGB rgb = color_xyz_to_rgb(xyz);
    return rgb;
}

static double lab_xyz_conv_helper(double t) {
    if (t > (lab_delta * lab_delta * lab_delta)) {
        return cbrt(t);
    } else {
        return (1.0/3.0 * t * pow(lab_delta, -2)) + (4.0/29.0);
    }
}

ColorLAB color_xyz_to_lab(ColorXYZ xyz) {
    ColorLAB lab = {0};
    lab.l = 116.0 * lab_xyz_conv_helper(xyz.y / lab_d65[1]) - 16.0;
    lab.a = 500.0 * (lab_xyz_conv_helper(xyz.x / lab_d65[0]) - lab_xyz_conv_helper(xyz.y / lab_d65[1]));
    lab.b = 200.0 * (lab_xyz_conv_helper(xyz.y / lab_d65[1]) - lab_xyz_conv_helper(xyz.z / lab_d65[2]));
    lab.l /= 100.0;
    lab.a = (lab.a + 128.0) / 255.0;
    lab.b = (lab.b + 128.0) / 255.0;
    return lab;
}

ColorLAB color_lch_to_lab(ColorLCH lch) {
    ColorLAB lab = {0};
    lch.c = lch.c / (1.0 - lch.c);

    lab.l = lch.l;
    lab.a = lch.c * cos(lch.h * M_PI * 2.0);
    lab.b = lch.c * sin(lch.h * M_PI * 2.0);

    lab.a = (lab.a + 128.0) / 255.0;
    lab.b = (lab.b + 128.0) / 255.0;
    return lab;
}

ColorLAB color_oklab_to_lab(ColorOKLAB oklab) {
    ColorXYZ xyz = color_oklab_to_xyz(oklab);
    ColorLAB lab = color_xyz_to_lab(xyz);
    return lab;
}

ColorLAB color_oklch_to_lab(ColorOKLCH oklch) {
    ColorOKLAB oklab = color_oklch_to_oklab(oklch);
    ColorXYZ xyz = color_oklab_to_xyz(oklab);
    ColorLAB lab = color_xyz_to_lab(xyz);
    return lab;
}

ColorLCH color_xyz_to_lch(ColorXYZ xyz) {
    ColorLAB lab = color_xyz_to_lab(xyz);
    ColorLCH lch = color_lab_to_lch(lab);
    return lch; 
}

ColorLCH color_lab_to_lch(ColorLAB lab) {
    ColorLCH lch = {0};
    lch.l = lab.l;

    lab.a = 255.0 * lab.a - 128.0;
    lab.b = 255.0 * lab.b - 128.0;

    lch.c = sqrt((lab.a * lab.a) + (lab.b * lab.b));
    lch.h = atan2(lab.b, lab.a) / M_PI / 2.0;

    lch.c = lch.c / (1.0 + lch.c);

    return lch;
}

ColorLCH color_oklab_to_lch(ColorOKLAB oklab) {
    ColorLAB lab = color_oklab_to_lab(oklab);
    ColorLCH lch = color_lab_to_lch(lab);
    return lch; 
}

ColorLCH color_oklch_to_lch(ColorOKLCH oklch) {
    ColorLAB lab = color_oklch_to_lab(oklch);
    ColorLCH lch = color_lab_to_lch(lab);
    return lch; 
}

ColorOKLAB color_xyz_to_oklab(ColorXYZ xyz) {
    ColorOKLAB oklab = {0};
    ColorOKLAB temp = {0};
    // apply the linear map into an LMS-like color space
    temp.l = xyz_oklab_mat1[0][0] * xyz.x + xyz_oklab_mat1[0][1] * xyz.y + xyz_oklab_mat1[0][2] * xyz.z;
    temp.a = xyz_oklab_mat1[1][0] * xyz.x + xyz_oklab_mat1[1][1] * xyz.y + xyz_oklab_mat1[1][2] * xyz.z;
    temp.b = xyz_oklab_mat1[2][0] * xyz.x + xyz_oklab_mat1[2][1] * xyz.y + xyz_oklab_mat1[2][2] * xyz.z;
    // cuberoot non-linearity? idk man
    temp.l = cbrt(temp.l);
    temp.a = cbrt(temp.a);
    temp.b = cbrt(temp.b);
    // now apply ANOTHER LINEAR MAP???
    oklab.l = xyz_oklab_mat2[0][0] * temp.l + xyz_oklab_mat2[0][1] * temp.a + xyz_oklab_mat2[0][2] * temp.b;
    oklab.a = xyz_oklab_mat2[1][0] * temp.l + xyz_oklab_mat2[1][1] * temp.a + xyz_oklab_mat2[1][2] * temp.b;
    oklab.b = xyz_oklab_mat2[2][0] * temp.l + xyz_oklab_mat2[2][1] * temp.a + xyz_oklab_mat2[2][2] * temp.b;
    return oklab;
}

ColorOKLAB color_lab_to_oklab(ColorLAB lab) {
    ColorXYZ xyz = color_lab_to_xyz(lab);
    ColorOKLAB oklab = color_xyz_to_oklab(xyz);
    return oklab;
}

ColorOKLAB color_lch_to_oklab(ColorLCH lch) {
    ColorXYZ xyz = color_lch_to_xyz(lch);
    ColorOKLAB oklab = color_xyz_to_oklab(xyz);
    return oklab;
}

ColorOKLAB color_oklch_to_oklab(ColorOKLCH oklch) {
    ColorOKLAB oklab = {0};

    // undo the chroma compression
    oklch.c = oklch.c / (1.0 - oklch.c);

    // ez polar conversions :D
    oklab.l = oklch.l;
    oklab.a = oklch.c * cos(oklch.h * M_PI * 2.0);
    oklab.b = oklch.c * sin(oklch.h * M_PI * 2.0);

    return oklab;
}

ColorOKLCH color_oklab_to_oklch(ColorOKLAB oklab) {
    ColorOKLCH oklch = {0};

    oklch.l = oklab.l;
    oklch.c = sqrt((oklab.a * oklab.a) + (oklab.b * oklab.b));
    oklch.h = atan2(oklab.b, oklab.a) / M_PI / 2.0;

    // chroma compression :)
    oklch.c = oklch.c / (1.0 + oklch.c);

    return oklch;
}

ColorOKLCH color_xyz_to_oklch(ColorXYZ xyz) {
    ColorOKLAB oklab = color_xyz_to_oklab(xyz);
    ColorOKLCH oklch = color_oklab_to_oklch(oklab);
    return oklch;
}

ColorOKLCH color_lab_to_oklch(ColorLAB lab) {
    ColorXYZ xyz = color_lab_to_xyz(lab);
    ColorOKLAB oklab = color_xyz_to_oklab(xyz);
    ColorOKLCH oklch = color_oklab_to_oklch(oklab);
    return oklch;
}

ColorOKLCH color_lch_to_oklch(ColorLCH lch) {
    ColorXYZ xyz = color_lch_to_xyz(lch);
    ColorOKLAB oklab = color_xyz_to_oklab(xyz);
    ColorOKLCH oklch = color_oklab_to_oklch(oklab);
    return oklch;
}

static double xyz_lab_conv_helper(double t) {
    if (t > lab_delta) {
        return t * t * t;
    } else {
        return 3 * (lab_delta * lab_delta) * (t - (4.0/29.0));
    }
}

ColorXYZ color_lab_to_xyz(ColorLAB lab) {
    ColorXYZ xyz = {0};
    lab.l *= 100.0;
    lab.a = lab.a * 255.0 - 128.0;
    lab.b = lab.b * 255.0 - 128.0;
    xyz.x = lab_d65[0] * xyz_lab_conv_helper((lab.l + 16.0) / 116.0 + (lab.a / 500.0));
    xyz.y = lab_d65[1] * xyz_lab_conv_helper((lab.l + 16.0) / 116.0);
    xyz.z = lab_d65[2] * xyz_lab_conv_helper((lab.l + 16.0) / 116.0 - (lab.b / 200.0));
    return xyz;
}

ColorXYZ color_lch_to_xyz(ColorLCH lch) {
    ColorLAB lab = color_lch_to_lab(lch);
    ColorXYZ xyz = color_lab_to_xyz(lab);
    return xyz;
}

ColorXYZ color_oklab_to_xyz(ColorOKLAB oklab) {
    ColorXYZ xyz = {0};
    ColorOKLAB temp = {0};

    // multiply by inverse mat2
    temp.l = xyz_oklab_mat2_i[0][0] * oklab.l + xyz_oklab_mat2_i[0][1] * oklab.a + xyz_oklab_mat2_i[0][2] * oklab.b;
    temp.a = xyz_oklab_mat2_i[1][0] * oklab.l + xyz_oklab_mat2_i[1][1] * oklab.a + xyz_oklab_mat2_i[1][2] * oklab.b;
    temp.b = xyz_oklab_mat2_i[2][0] * oklab.l + xyz_oklab_mat2_i[2][1] * oklab.a + xyz_oklab_mat2_i[2][2] * oklab.b;

    // cube to undo the cuberoot
    temp.l = pow(temp.l, 3.0);
    temp.a = pow(temp.a, 3.0);
    temp.b = pow(temp.b, 3.0);

    xyz.x = xyz_oklab_mat1_i[0][0] * temp.l + xyz_oklab_mat1_i[0][1] * temp.a + xyz_oklab_mat1_i[0][2] * temp.b;
    xyz.y = xyz_oklab_mat1_i[1][0] * temp.l + xyz_oklab_mat1_i[1][1] * temp.a + xyz_oklab_mat1_i[1][2] * temp.b;
    xyz.z = xyz_oklab_mat1_i[2][0] * temp.l + xyz_oklab_mat1_i[2][1] * temp.a + xyz_oklab_mat1_i[2][2] * temp.b;

    return xyz;
}

ColorXYZ color_oklch_to_xyz(ColorOKLCH oklch) {
    ColorOKLAB oklab = color_oklch_to_oklab(oklch);
    ColorXYZ xyz = color_oklab_to_xyz(oklab);
    return xyz;
}

ColorXYZ color_xyz_lerp(ColorXYZ start, ColorXYZ end, double t) {
    ColorXYZ out = {0};
    out.x = start.x + (end.x - start.x) * t;
    out.y = start.y + (end.y - start.y) * t;
    out.z = start.z + (end.z - start.z) * t;
    return out;
}

ColorLAB color_lab_lerp(ColorLAB start, ColorLAB end, double t) {
    ColorLAB out = {0};
    out.l = start.l + (end.l - start.l) * t;
    out.a = start.a + (end.a - start.a) * t;
    out.b = start.b + (end.b - start.b) * t;
    return out;
}

ColorLCH color_lch_lerp(ColorLCH start, ColorLCH end, double t) {
    ColorLCH out = {0};
    out.l = start.l + (end.l - start.l) * t;
    out.c = start.c + (end.c - start.c) * t;
    out.h = start.h + (end.h - start.h) * t;
    return out;
}

ColorOKLAB color_oklab_lerp(ColorOKLAB start, ColorOKLAB end, double t) {
    ColorOKLAB out = {0};
    out.l = start.l + (end.l - start.l) * t;
    out.a = start.a + (end.a - start.a) * t;
    out.b = start.b + (end.b - start.b) * t;
    return out;
}

ColorOKLCH color_oklch_lerp(ColorOKLCH start, ColorOKLCH end, double t) {
    ColorOKLCH out = {0};
    out.l = start.l + (end.l - start.l) * t;
    out.c = start.c + (end.c - start.c) * t;
    out.h = start.h + (end.h - start.h) * t;
    return out;
}

void color_install_handler(ColorHandler handler) {
    handlers[handler_count] = handler;
    handler_count++;
}

void color_run_handler(u64 index, Color color) {
    if (index > MAX_HANDLERS - 1) {
        printf("[color.h] handler index too high, specify lower index\n");
    }

    ColorHandler handler = handlers[index];

    if ((handler.supported_spaces & RGB) != 0) {
        if (color.type == RGB) {
            handler.color_rgb(color.color_rgb);
        }
    }

    if ((handler.supported_spaces & LAB) != 0) {
        if (color.type == LAB) {
            handler.color_lab(color.color_lab);
        }
    }

    if ((handler.supported_spaces & LCH) != 0) {
        if (color.type == LCH) {
            handler.color_lch(color.color_lch);
        }
    }

    if ((handler.supported_spaces & OKLAB) != 0) {
        if (color.type == OKLAB) {
            handler.color_oklab(color.color_oklab);
        }
    }

    if ((handler.supported_spaces & OKLCH) != 0) {
        if (color.type == OKLCH) {
            handler.color_oklch(color.color_oklch);
        }
    }

    if ((handler.supported_spaces & XYZ) != 0) {
        if (color.type == XYZ) {
            handler.color_xyz(color.color_xyz);
        }
    }
}

ColorRGB linear_to_srgb(ColorRGB rgb) {
    if (rgb.r <= 0.0031308) {
        rgb.r *= 12.92;
    } else {
        rgb.r = 1.055 * pow(rgb.r, 1.0/2.4) - 0.055;
    }

    if (rgb.g <= 0.0031308) {
        rgb.g *= 12.92;
    } else {
        rgb.g = 1.055 * pow(rgb.g, 1.0/2.4) - 0.055;
    }

    if (rgb.b <= 0.0031308) {
        rgb.b *= 12.92;
    } else {
        rgb.b = 1.055 * pow(rgb.b, 1.0/2.4) - 0.055;
    }

    return rgb;
}
#endif
