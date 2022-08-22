## Adafruit GFX Library original text:

This is the core graphics library for all our displays, providing a common set of graphics primitives (points, lines, circles, etc.). It needs to be paired with a hardware-specific library for each display device we carry (to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information.
All text above must be included in any redistribution.

This library carries exactly the same licensing terms as the original, any changes made in this repo are assumed to be under the same terms as the original too.

## Library fork supporting SSD1306/SH1106 displays on mbed 5/6 RTOS

Although many newer mbed RTOS chipsets have inbuilt hardware LCD drivers, there's sometimes a need to use a more basic display, such as an OLED or LCD unit. This is a fork of the Adafruit_GFX library with changes to run on mbed 5/6 RTOS. It is derived from a few sources:

* Most of the hard work was done by [AdaFruit in the Adafruit_GFX repo](https://github.com/adafruit/Adafruit-GFX-Library)
* I then combined the latest Adafruit_GFX master with the [work originally done to support SSD1306 on mbed 2](https://os.mbed.com/users/nkhorman/code/Adafruit_GFX/) 
* On top of this I added support for SH1106 displays (after doing a bit of digging and datasheet analysis). 
* You can use the regular [Adafruit_GFX introductory guide](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview) as this fork has high compatibility. 
* It also integrates well with tcMenu, IoAbstraction and TaskManagerIO.

## Issues

Unless you are absolutely sure that the issue is a core Adafruit_GFX issue, please raise the issue here first for us to triage it.

## Usage

This library can work with SSD1306 and SH1106 displays over I2C or SPI. There's a couple of really simple examples packaged that show basic usage. However, you can look at almost any Adafruit example, as this library is completely compatible, especially wit the IoAbstraction extensions.

Although not required, this library has the ability to integrate with IoAbstraction by defining USE_IOABSTRACTION_TCMENU, this provides a complete Print interface that is nearly compatible with Arduino's print functions. I'll be honest, we all but assume this flag is set because all our development uses it. To completely remove IoAbstraction, remove the flag in the config header. 

Performance, at the moment the SSD1306 class could be significantly optimised. However, our first aim is to get it stable on mbed 5/6, we'll look at performance later.

## What is the purpose of this fork:

Every fork should state what it's purpose is. This fork is purely to provide mbed support for a few popular displays, we do not intend to change the core libraries in any way. It allows mbed V6 users to take advantage of the excellent Adafruit_GFX library. We have set it up to work with platformIO, but it should also work with any other build system.

Important note about bug fixes: do not put in PRs for the core graphics class here, only mbed and display specific fixes will be applied. Instead apply any changes to the core and then we'll re-pull the change here.

# Useful Resources

- Image2Code: This is a handy Java GUI utility to convert a BMP file into the array code necessary to display the image with the drawBitmap function. Check out the code at ehubin's GitHub repository: https://github.com/ehubin/Adafruit-GFX-Library/tree/master/Img2Code

- drawXBitmap function: You can use the GIMP photo editor to save a .xbm file and use the array saved in the file to draw a bitmap with the drawXBitmap function. See the pull request here for more details: https://github.com/adafruit/Adafruit-GFX-Library/pull/31

- 'Fonts' folder contains bitmap fonts for use with recent (1.1 and later) Adafruit_GFX. To use a font in your Arduino sketch, \#include the corresponding .h file and pass address of GFXfont struct to setFont(). Pass NULL to revert to 'classic' fixed-space bitmap font.

- 'fontconvert' folder contains a command-line tool for converting TTF fonts to Adafruit_GFX header format.

---

### Roadmap

The PRIME DIRECTIVE is to maintain backward compatibility with existing Arduino sketches -- many are hosted elsewhere and don't track changes here, some are in print and can never be changed! This "little" library has grown organically over time and sometimes we paint ourselves into a design corner and just have to live with it or add ungainly workarounds.

Highly unlikely to merge any changes for additional or incompatible font formats (see Prime Directive above). There are already two formats and the code is quite bloaty there as it is (this also creates liabilities for tools and documentation). If you *must* have a more sophisticated font format, consider creating a fork with the features required for your project. For similar reasons, also unlikely to add any more bitmap formats, it's getting messy.

Please don't reformat code for the sake of reformatting code. The resulting large "visual diff" makes it impossible to untangle actual bug fixes from merely rearranged lines.
