#!/usr/bin/env python3
#
# OXYGEN-SDK Setup helper for Windows
#
# Functionality:
#  - Downloads and installs necessary 3rdparty libraries
#


import argparse
import os
import platform
import subprocess
import sys

import json
import zipfile

from urllib import request

#
# List of OXYGEN-SDK dependencies
dependencies_json = \
'''
{
    "Windows" : [
        {
            "name" : "Boost 1.70.0",
            "url" : "https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/boost_1_70_0.zip",
            "check" : {
                "exists" : "3rdparty/boost_1_70_0"
            },
            "after" : {
                "extract_to": "3rdparty"
            }
        },
        {
            "name" : "Qt resource compiler (rcc)",
            "url" : "https://github.com/DEWETRON/OXYGEN-SDK/releases/download/OXYGEN-SDK-6.0.0/qt_resource_compiler.zip",
            "check" : {
                "exists" : "3rdparty/bt/bin/rcc.exe"
            },
            "after" : {
                "extract_to": "."
            }
        },
        {
            "name" : "CMake 3.23.1",
            "url" : "https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1-windows-x86_64.msi",
            "check" : {
                "cmd" : "cmake --version"    
            },
            "after" : {
                "exec": "cmake-3.23.1-windows-x86_64.msi"
            }
        }
    ],
    "Linux" : [
    ]
}
'''


VERBOSE = 0

def verbose(msg):
    if VERBOSE:
        print('dwbuild:', msg, flush=True)





def do_check(check):
    """
    do_check
    """
    cmd = check.get('cmd', None)
    exists = check.get('exists', None)

    if cmd:
        try:
            cmd_list = cmd.split()
            out = subprocess.check_output(cmd_list)
        except OSError as error:
            print("Check error %s" % (error), flush=True)
            return False
        except:
            return False
    elif exists:
        return os.path.exists(exists)

    return True


def do_download(url, filename):
    """
    download
    """
    verbose("downloading %s" % (filename))
    request.urlretrieve(url, filename)


def do_afterprocessing(dep, after):
    """
    do_afterprocessing
    """
    exec = after.get('exec', None)
    extract_to = after.get('extract_to', None)
    
    url = dep.get('url', None)

    if exec:
        verbose("Running executable %s" % (exec))
        try:
            exec_list = exec.split()
            subprocess.check_output(exec_list)
        except OSError as error:
            print("Check error %s" % (error), flush=True)
            return False
        except:
            return False

    elif extract_to:
        archive_name = os.path.basename(url)
        verbose("Extracting %s to %s" % (archive_name, extract_to))
        archive = zipfile.ZipFile(archive_name)
        archive.extractall(path=extract_to)

    return True


def process_dependency(cmd_cfg, dep):
    """
    process_dependency
    """
    name = dep.get('name', None)
    url = dep.get('url', None)
    check = dep.get('check', {})
    after = dep.get('after', {})

    skip_download = cmd_cfg.get('skip_download', False)

    verbose("processing dependency %s %s" % (name, url))

    if name == None or url == None:
        return

    # Install only if check fails
    if not do_check(check):
        print("%s dependency processing ..." % (name), flush=True)
        if not skip_download:
            do_download(url, os.path.basename(url))
        do_afterprocessing(dep, after)
    else:
        print("%s dependency is fullfilled!" % (name), flush=True)


def setup(argv):
    """
    setup main
    """
    parser = argparse.ArgumentParser(
        description='%s is a script file for initial setup of an OXYGEN-SDK workspace' % sys.argv[0])

    parser.add_argument('-v', '--verbose', dest='verbose',
                        action='store_true',
                        default=False,
                        help='be more verbose')
    parser.add_argument('--skip-download', dest='skip_download',
                        action='store_true',
                        default=False,
                        help='Omit download steps')


    args = parser.parse_args(argv)


    global VERBOSE
    VERBOSE = args.verbose

    cmd_cfg = {
        "skip_download" : args.skip_download
    }


    dep_json = json.loads(dependencies_json)
    dep_list = dep_json[platform.system()]


    for dep in dep_list:
        process_dependency(cmd_cfg, dep)

    return 0


if __name__ == "__main__":
    sys.exit(setup(sys.argv[1:]))
