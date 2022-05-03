SDK manual with Sphinx
======================


Setup for Windows
=================
 * GitHub Desktop       https://desktop.github.com/
    - Version control system
 * Python3              https://www.python.org/downloads/
    - Sphinx is written in Python3
    - Download and install latest Python (Python 3.9.4) at the time of writing
 * MikTeX               https://miktex.org/
    - LaTeX distribution for Windows
    - Install for all users
 * TexStudio            https://www.texstudio.org/
    - LaTeX Editor
 * Visual Studio Code   https://code.visualstudio.com/download
    - IDE with Sphinx Preview

Please reboot if any installer requests it.

After this please open a cmd shell "Eingabeauforderung":
 * Install Sphinx
     - "pip install -U sphinx"
 * Install Sphinx material theme
     - "pip install -U sphinx-theme-material"


Setup for Linux (Ubuntu 20.04)
==============================

Note: Python3 will already be installed in Linux.

 * TexStudio            https://www.texstudio.org/
    - LaTeX Editor
    - "sudo apt install texstudio"
 * Visual Studio Code   https://code.visualstudio.com/download
    - IDE with Sphinx Preview
    - get the .deb package
    - "sudo apt install ~/Dowloads/code_*_amd64.deb"
 * Latex tools
    - "sudo apt install dvipng"
    - "sudo apt install texlive-luatex"
    - "sudo apt install texlive-latex-extra"
    - "sudo apt install fonts-freefont-otf texlive-fonts-extra"
    - "sudo apt install latexmk"

After this please open a terminal:
 * Install Sphinx
     - "pip3 install -U sphinx"
 * Install Sphinx material theme
     - "pip3 install -U sphinx-material"
 * Install Sphinx read-the-docs theme
     - "pip3 install -U sphinx_rtd_theme"


If sphinx-build cannot be found, at "$HOME/.local/bin" to PATH.


Info and help - reStructuredText Primer:
https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html



Create html
===========

In any commandshell enter:

make.bat html

The latex output is created in the sub directory _build/html


Create pdf
==========

In TeXstudio "lualatex" should be used as standard compiler.

In any commandshell enter:

make.bat latex

The latex output is created in the sub directory _build/latex

Open _build/latex/oxygenmanual.tex in TexStudio

and press F5


make.bat latexpdf

Directly create pdf cocument.



Documentation Standard
======================
1. Chapter              =============
1.1 Subchapter          -------------
1.1.1 Section           ~~~~~~~~~~~~~
1.1.1.1 Subsection      ^^^^^^^^^^^^^


rst files should use UNIX file format or validators report
problems in every line.
