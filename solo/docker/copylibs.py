#!/usr/bin/env python3

import os
import shutil

def main():
    # The directory where we want to copy the files
    target_dir = "/usr/local/lib"

    for root, dirs, files in os.walk("/opt/apollo/neo/packages"):
        # Check if 'lib' is one of the subdirectories
        if "lib" in dirs:
            source_lib_path = os.path.join(root, "lib")
            print(f"Found lib directory at: {source_lib_path}")

            # Copy each item inside the found 'lib' to /usr/local/lib
            for item in os.listdir(source_lib_path):
                source_item_path = os.path.join(source_lib_path, item)
                target_item_path = os.path.join(target_dir, item)

                # If it's a directory, copy it (including its contents)
                if os.path.isdir(source_item_path):
                    # copytree with dirs_exist_ok=True (Python 3.8+)
                    # will merge the directory if it already exists
                    try:
                        shutil.copytree(
                            src=source_item_path, 
                            dst=target_item_path, 
                            dirs_exist_ok=True
                        )
                    except FileExistsError:
                        # In older Python versions, you can handle conflicts manually
                        pass
                else:
                    # If it's a file, copy2 preserves metadata
                    shutil.copy2(source_item_path, target_item_path)

if __name__ == "__main__":
    main()
