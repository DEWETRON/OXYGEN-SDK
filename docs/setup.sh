#!/bin/env bash

set -e

sudo apt install texstudio
sudo apt install dvipng
sudo apt install texlive-luatex
sudo apt install texlive-latex-extra
sudo apt install fonts-freefont-otf texlive-fonts-extra
sudo apt install latexmk

pip3 install -U sphinx
pip3 install -U sphinx-material
pip3 install -U sphinx_rtd_theme
