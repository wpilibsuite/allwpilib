#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
mkdir -p $DIR/generated
cd $DIR/generated

# Versions
IMGUI_COMMIT=74f7ac04a166c77ef1cbbbebff51e5bfc4fcfa5d
ICONFONTCPPHEADERS_COMMIT=acd3728de3ee4e2461f8958154bb2dc46f958723
DROID_COMMIT=d3817c246c6e3da7531fa1fbb0b0fca271aae7fb
PROGGYFONTS_VERSION=1.1.5
FONTAWESOME_VERSION=6.2.0
FIRACODE_VERSION=6.2

mkdir download
pushd download
# Download compressor
wget https://github.com/ocornut/imgui/raw/${IMGUI_COMMIT}/misc/fonts/binary_to_compressed_c.cpp

# Download fonts
wget -O proggyfonts.zip https://github.com/bluescan/proggyfonts/archive/refs/tags/v${PROGGYFONTS_VERSION}.zip
wget https://github.com/FortAwesome/Font-Awesome/releases/download/${FONTAWESOME_VERSION}/fontawesome-free-${FONTAWESOME_VERSION}-web.zip
wget -O droid-fonts.zip https://github.com/grays/droid-fonts/archive/${DROID_COMMIT}.zip
wget -O fira-code.zip https://github.com/tonsky/FiraCode/releases/download/${FIRACODE_VERSION}/Fira_Code_v${FIRACODE_VERSION}.zip

# Download C++ icon font header
wget https://github.com/juliettef/IconFontCppHeaders/raw/${ICONFONTCPPHEADERS_COMMIT}/IconsFontAwesome6.h

popd

# Extract fonts
rm -rf extract
mkdir extract
pushd extract
unzip ../download/proggyfonts.zip
unzip ../download/fontawesome-free-${FONTAWESOME_VERSION}-web.zip
unzip ../download/droid-fonts.zip
mkdir fira-code
pushd fira-code
unzip ../../download/fira-code.zip
popd
popd

rm -rf fonts
mkdir -p fonts/src
mkdir -p fonts/include

# Copy C++ icon font header
cp -p download/IconsFontAwesome6.h fonts/include/

# Copy license files
cp -p extract/proggyfonts-${PROGGYFONTS_VERSION}/LICENSE fonts/LICENSE-proggyfonts.txt
cp -p extract/fontawesome-free-${FONTAWESOME_VERSION}-web/LICENSE.txt fonts/LICENSE-fontawesome.txt
cp -p extract/droid-fonts-${DROID_COMMIT}/droid/NOTICE fonts/LICENSE-droid.txt

# Build C versions
g++ -o imgui_font_bin2c download/binary_to_compressed_c.cpp
./imgui_font_bin2c "extract/proggyfonts-${PROGGYFONTS_VERSION}/ProggyDotted/ProggyDotted Regular.ttf" ProggyDotted > fonts/src/ProggyDotted.inc
./imgui_font_bin2c "extract/fontawesome-free-${FONTAWESOME_VERSION}-web/webfonts/fa-regular-400.ttf" FontAwesomeRegular > fonts/src/FontAwesomeRegular.inc
./imgui_font_bin2c "extract/fontawesome-free-${FONTAWESOME_VERSION}-web/webfonts/fa-solid-900.ttf" FontAwesomeSolid > fonts/src/FontAwesomeSolid.inc
./imgui_font_bin2c "extract/droid-fonts-${DROID_COMMIT}/droid/DroidSans.ttf" DroidSans > fonts/src/DroidSans.inc
./imgui_font_bin2c "extract/fira-code/ttf/FiraCode-Retina.ttf" FiraCodeRetina > fonts/src/FiraCodeRetina.inc

# Generate C wrapper source/headers
for font in ProggyDotted FontAwesomeRegular FontAwesomeSolid DroidSans FiraCodeRetina
do
cat >fonts/src/imgui_${font}.cpp <<END
#include "imgui_${font}.h"
#include "${font}.inc"
ImFont* ImGui::AddFont${font}(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {
  return io.Fonts->AddFontFromMemoryCompressedTTF(${font}_compressed_data, ${font}_compressed_size, size_pixels, font_cfg, glyph_ranges);
}
END

cat >fonts/include/imgui_${font}.h <<END
#pragma once
#include "imgui.h"
namespace ImGui {
ImFont* AddFont${font}(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg = nullptr, const ImWchar* glyph_ranges = nullptr);
}
END
done

rm -r download extract imgui_font_bin2c