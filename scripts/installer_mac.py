#!/usr/bin/env python3
import argparse
import os
import stat
import subprocess
import sys
from pathlib import Path


def create_clean_script(scripts_dir: Path, vst3_name: str, au_name: str, stand_name: str, mfg_code: str):
    """Creates an executable script to remove previous installations."""
    scripts_dir.mkdir(parents=True, exist_ok=True)
    clean_old_path = scripts_dir / "preinstall"

    script_content = f"""#!/bin/sh
# remove previous installations
rm -rf "/Library/Audio/Plug-Ins/VST3/{vst3_name}"
pkgutil --forget "com.{mfg_code}.pkg.vst3" 2>/dev/null || true

rm -rf "/Library/Audio/Plug-Ins/Components/{au_name}"
pkgutil --forget "com.{mfg_code}.pkg.au" 2>/dev/null || true

rm -rf "/Applications/{stand_name}"
pkgutil --forget "com.{mfg_code}.pkg.standalone" 2>/dev/null || true

exit 0
"""
    clean_old_path.write_text(script_content, encoding="utf-8")
    clean_old_path.chmod(clean_old_path.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    return scripts_dir

def create_postinstall_script(scripts_dir: Path, vst3_name: str, au_name: str, stand_name: str):
    """Creates an executable postinstall script to strip quarantine flags."""
    scripts_dir.mkdir(parents=True, exist_ok=True)
    postinstall_path = scripts_dir / "postinstall"

    script_content = f"""#!/bin/sh
# Strip quarantine attributes from installed plugin targets
xattr -cr "/Library/Audio/Plug-Ins/VST3/{vst3_name}" 2>/dev/null || true
xattr -cr "/Library/Audio/Plug-Ins/Components/{au_name}" 2>/dev/null || true
xattr -cr "/Applications/{stand_name}" 2>/dev/null || true

exit 0
"""
    postinstall_path.write_text(script_content, encoding="utf-8")
    postinstall_path.chmod(postinstall_path.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    return scripts_dir

def run_clean_pkgbuild(identifier: str, version: str, output_pkg: Path, clean_scripts_dir: Path):
    """Builds a payload-free script package using --nopayload."""
    cmd = [
        "pkgbuild",
        "--nopayload",
        "--scripts", str(clean_scripts_dir),
        "--identifier", identifier,
        "--version", version,
        str(output_pkg)
    ]
    print(f"Building clean component package: {output_pkg.name}...", flush=True)
    subprocess.run(cmd, check=True, capture_output=True, text=True)

def create_distribution_xml(xml_path: Path, project_name: str, version: str, mfg_code: str):
    """Generates a Distribution.xml file for productbuild."""
    content = f"""<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>{project_name}</title>
    <options customize="always" require-scripts="false"/>
    
    <choices-outline>
        <line choice="choice_clean"/>
        <line choice="choice_vst3"/>
        <line choice="choice_au"/>
        <line choice="choice_standalone"/>
    </choices-outline>
    
    <choice id="choice_clean" 
            title="Remove previous Versions" 
            description="Deletes old installations and clears receipt logs before installing."
            selected="true"> 
        <pkg-ref id="com.{mfg_code}.pkg.clean"/>
    </choice>

    <choice id="choice_vst3" title="{project_name} VST3 Plugin">
        <pkg-ref id="com.{mfg_code}.pkg.vst3"/>
    </choice>

    <choice id="choice_au" title="{project_name} Audio Unit (AU)">
        <pkg-ref id="com.{mfg_code}.pkg.au"/>
    </choice>

    <choice id="choice_standalone" title="{project_name} Standalone Application">
        <pkg-ref id="com.{mfg_code}.pkg.standalone"/>
    </choice>

    <pkg-ref id="com.{mfg_code}.pkg.clean" version="1.0.0">{project_name}_clean.pkg</pkg-ref>
    <pkg-ref id="com.{mfg_code}.pkg.vst3" version="{version}">{project_name}_vst3.pkg</pkg-ref>
    <pkg-ref id="com.{mfg_code}.pkg.au" version="{version}">{project_name}_au.pkg</pkg-ref>
    <pkg-ref id="com.{mfg_code}.pkg.standalone" version="{version}">{project_name}_standalone.pkg</pkg-ref>
</installer-gui-script>
"""
    xml_path.write_text(content, encoding="utf-8")


def run_pkgbuild(root_path: Path, install_location: str, identifier: str, version: str, output_pkg: Path, scripts_dir: Path = None):
    """Generates an intermediate .pkg file for a specific plugin format."""
    if not root_path.exists():
        print(f"Error: Bundle missing at {root_path}")
        sys.exit(1)

    output_pkg.parent.mkdir(parents=True, exist_ok=True)

    cmd = [
        "pkgbuild",
        "--root", str(root_path),
        "--install-location", install_location,
        "--identifier", identifier,
        "--version", version
    ]

    if scripts_dir and scripts_dir.exists():
        cmd.extend(["--scripts", str(scripts_dir)])

    cmd.append(str(output_pkg))

    print(f"Building component package: {output_pkg.name}...", flush=True)
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"  Successfully created {output_pkg.name}", flush=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running pkgbuild for {identifier}:\n{e.stderr}", flush=True)
        sys.exit(1)


def run_productbuild(dist_xml: Path, package_dir: Path, output_installer: Path):
    """Combines intermediate .pkg files into a final installer wizard."""
    cmd = [
        "productbuild",
        "--distribution", str(dist_xml),
        "--package-path", str(package_dir),
        str(output_installer)
    ]

    print(f"\nBuilding final installer: {output_installer.name}...", flush=True)
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"Successfully created final installer at: {output_installer}", flush=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running productbuild:\n{e.stderr}", flush=True)
        sys.exit(1)

def main():
    print("\n******************\n******************\n******************\n******************\n")
    parser = argparse.ArgumentParser(description="Build macOS .pkg installer for Audio Plugins")
    parser.add_argument("--name", required=True, help="Plugin name")
    parser.add_argument("--manufacturer", required=True, help="Manufacturer Code")
    parser.add_argument("--plugin-code", required=True, help="Plugin Code")
    parser.add_argument("--version", required=True, help="Plugin version")
    parser.add_argument("--vst3-path", required=True, help="VST3 path")
    parser.add_argument("--au-path", required=True, help="AU path")
    parser.add_argument("--stand-path", required=True, help="Standalone path")
    parser.add_argument("--build-dir", type=Path, required=True, help="CMake build directory")

    args = parser.parse_args()

    mfg = args.manufacturer.lower()

    print(f"Building installer for: {args.name} v{args.version} ({args.manufacturer} / {args.plugin_code})", flush=True)
    print(f"Build directory: {args.build_dir}", flush=True)

    pkg_working_dir = args.build_dir / "installer_packages"
    pkg_working_dir.mkdir(parents=True, exist_ok=True)

    scripts_dir = pkg_working_dir / "pkg_scripts"
    vst3_name = Path(args.vst3_path).name
    au_name = Path(args.au_path).name
    stand_name = Path(args.stand_path).name if args.stand_path else ""

    create_postinstall_script(scripts_dir, vst3_name, au_name, stand_name)

    # 0. Build Payload-Free Clean Package
    clean_scripts_dir = pkg_working_dir / "clean_scripts"
    create_clean_script(clean_scripts_dir, vst3_name, au_name, stand_name, mfg)

    run_clean_pkgbuild(
        identifier=f"com.{mfg}.pkg.clean",
        version=args.version,
        output_pkg=pkg_working_dir / f"{args.name}_clean.pkg",
        clean_scripts_dir=clean_scripts_dir
    )

    # 1. Build VST3 Component Package
    run_pkgbuild(
        root_path=Path(args.vst3_path),
        install_location=f"/Library/Audio/Plug-Ins/VST3/{vst3_name}",
        identifier=f"com.{mfg}.pkg.vst3",
        version=args.version,
        output_pkg=pkg_working_dir / f"{args.name}_vst3.pkg",
        scripts_dir=scripts_dir
    )

    # 2. Build AU Component Package
    run_pkgbuild(
        root_path=Path(args.au_path),
        install_location=f"/Library/Audio/Plug-Ins/Components/{au_name}",
        identifier=f"com.{mfg}.pkg.au",
        version=args.version,
        output_pkg=pkg_working_dir / f"{args.name}_au.pkg",
        scripts_dir=scripts_dir
    )

    # 3. Build Standalone Component Package
    run_pkgbuild(
        root_path=Path(args.stand_path),
        install_location=f"/Applications/{stand_name}",
        identifier=f"com.{mfg}.pkg.standalone",
        version=args.version,
        output_pkg=pkg_working_dir / f"{args.name}_standalone.pkg",
        scripts_dir=scripts_dir
    )

    # 4. Generate Distribution XML
    xml_path = pkg_working_dir / "Distribution.xml"
    create_distribution_xml(xml_path, args.name, args.version, mfg)

    # 5. Create Final Combined Installer
    final_installer_path = args.build_dir / f"{args.name}_{args.version}_macOS.pkg"
    run_productbuild(
        dist_xml=xml_path,
        package_dir=pkg_working_dir,
        output_installer=final_installer_path
    )

    print("\n******************\n******************\n******************\n******************\n")


if __name__ == "__main__":
    main()