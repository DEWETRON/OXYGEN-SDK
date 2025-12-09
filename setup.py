#!/usr/bin/env python3
#
# OXYGEN-SDK Setup helper for Windows
#
# Functionality:
#  - Downloads and installs necessary 3rdparty libraries
#

import argparse
import platform
import subprocess
import sys
import json
import zipfile
from pathlib import Path
import logging
import requests

# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("Oxygen-SDK-Setup")

# List of OXYGEN-SDK dependencies
dependencies_json = '''
{
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

def verbose(msg):
    logger.info(msg)

def do_check(check):
    """
    Checks if the given setup step has to be executed or
    if the dependency is already fulfilled.
    """
    cmd = check.get('cmd')
    exists = check.get('exists')

    if cmd:
        try:
            subprocess.run(cmd.split(), check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            return False
    elif exists:
        return Path(exists).exists()

    return False

def do_download(url, filename):
    """
    Download the file from the given URL to the specified filename.
    """
    verbose(f"Downloading {filename} from {url}")
    try:
        response = requests.get(url)
        response.raise_for_status()
        with open(filename, 'wb') as f:
            f.write(response.content)
    except requests.RequestException as e:
        logger.error(f"Failed to download {url}: {e}")
        sys.exit(1)

def do_afterprocessing(dep, after):
    """
    Perform after-processing steps such as extracting or executing files.
    """
    exec_command = after.get('exec')
    extract_to = after.get('extract_to')

    name = dep.get('name')
    url = dep.get('url')

    if exec_command:
        logger.info(f"Executing {name} ...")
        try:
            subprocess.run(exec_command.split(), check=True)
        except subprocess.CalledProcessError as e:
            logger.error(f"Error executing {exec_command}: {e}")
            sys.exit(1)

    elif extract_to:
        logger.info(f"Unpacking {name} ...")
        archive_name = Path(url).name
        try:
            with zipfile.ZipFile(archive_name, 'r') as archive:
                archive.extractall(Path(extract_to))
        except zipfile.BadZipFile as e:
            logger.error(f"Error extracting {archive_name}: {e}")
            sys.exit(1)

def process_dependency(cmd_cfg, dep):
    """
    Process each dependency by checking, downloading, and after-processing.
    """
    name = dep.get('name')
    url = dep.get('url')
    check = dep.get('check', {})
    after = dep.get('after', {})

    skip_download = cmd_cfg.get('skip_download', False)

    verbose(f"Processing dependency {name} ({url})")

    if not name or not url:
        return

    if not do_check(check):
        if not skip_download:
            logger.info(f"Downloading {name} ...")
            do_download(url, Path(url).name)
        do_afterprocessing(dep, after)
    else:
        logger.info(f"{name} dependency is fulfilled!")

def setup(argv):
    """
    Main setup function.
    """
    parser = argparse.ArgumentParser(
        description='Setup script for OXYGEN-SDK workspace')

    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose logging')
    parser.add_argument('--skip-download', action='store_true', help='Skip download steps')

    args = parser.parse_args(argv)

    if args.verbose:
        logger.setLevel(logging.DEBUG)

    cmd_cfg = {
        "skip_download": args.skip_download
    }

    dep_json = json.loads(dependencies_json)
    system = platform.system()
    dep_list = dep_json.get(system, [])

    if not dep_list:
        logger.error(f"No dependencies found for {system}. Exiting.")
        return 1

    for dep in dep_list:
        process_dependency(cmd_cfg, dep)

    return 0

if __name__ == "__main__":
    sys.exit(setup(sys.argv[1:]))
