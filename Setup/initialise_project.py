#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
from pathlib import Path

from VersionHints_template import VERSION_HINTS_TEMPLATE

def run_git(args, cwd):
    """Utility helper to run git commands safely."""
    try:
        subprocess.run(["git"] + args, cwd=cwd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Git warning/notice ({' '.join(args)}): {e.stderr.strip()}")

def main():
    repo_root = Path.cwd()
    template_dir = repo_root / "sjf_DummyPlugin"

    # 1. Ensure template directory exists
    if not template_dir.exists() or not template_dir.is_dir():
        print(f"Error: Template directory '{template_dir}' not found!")
        sys.exit(1)

    # 2. Prompt for name
    try:
        project_name = input("Enter new project name: ").strip()
    except (KeyboardInterrupt, EOFError):
        print("\nOperation cancelled.")
        sys.exit(0)

    if not project_name:
        print("Error: Project name cannot be empty.")
        sys.exit(1)

    # 3. String transformations
    underscores = project_name.replace(" ", "_")
    all_caps = underscores.upper()

    target_dir = repo_root / underscores

    if target_dir.exists():
        print(f"Error: Target directory '{target_dir.name}' already exists!")
        sys.exit(1)

    print(f"\nCreating new project: {project_name} ({underscores})...")

    # 4. Perform replacements in CMakeLists.txt
    cmakelists_path = template_dir / "CMakeLists.txt"
    if cmakelists_path.exists():
        content = cmakelists_path.read_text(encoding="utf-8")

        # Replacements (order-safe)
        content = content.replace("Dummy Plugin", project_name)
        content = content.replace("DummyPlugin", underscores)
        content = content.replace("DUMMY_PLUGIN", all_caps)

        cmakelists_path.write_text(content, encoding="utf-8")
    else:
        print(f"Warning: {cmakelists_path} not found. Skipping text replacement.")

    version_hints_path = template_dir / "Source" / "VersionHints.h"
    version_hints_path.parent.mkdir(parents=True, exist_ok=True)

    version_hints_path.write_text(VERSION_HINTS_TEMPLATE, encoding="utf-8")
    print("Reset Source/VersionHints.h for the new project.")

    # 5. Rename directory
    template_dir.rename(target_dir)

    # 6. Reset Git history
    print("Initializing fresh Git history...")
    run_git(["checkout", "--orphan", "fresh-start"], cwd=target_dir)
    run_git(["add", "-A"], cwd=target_dir)
    run_git(["commit", "-m", "Initial commit"], cwd=target_dir)
    run_git(["branch", "-M", "main"], cwd=target_dir)

    # Submodules check
    if (target_dir / ".gitmodules").exists():
        print("Updating submodules...")
        run_git(["submodule", "update", "--init", "--recursive"], cwd=target_dir)

    # Clean up remotes and history
    run_git(["remote", "remove", "origin"], cwd=target_dir)
    run_git(["reflog", "expire", "--expire=now", "--all"], cwd=target_dir)
    run_git(["gc", "--prune=now", "--aggressive"], cwd=target_dir)

    print(f"\nSuccess! Project created at: {target_dir}")

if __name__ == "__main__":
    main()