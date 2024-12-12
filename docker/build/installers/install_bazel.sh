# #!/usr/bin/env bash

# ###############################################################################
# # Copyright 2020 The Apollo Authors. All Rights Reserved.
# #
# # Licensed under the Apache License, Version 2.0 (the "License");
# # you may not use this file except in compliance with the License.
# # You may obtain a copy of the License at
# #
# # http://www.apache.org/licenses/LICENSE-2.0
# #
# # Unless required by applicable law or agreed to in writing, software
# # distributed under the License is distributed on an "AS IS" BASIS,
# # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# # See the License for the specific language governing permissions and
# # limitations under the License.
# ###############################################################################

# # Fail on first error.
# set -e

# CURR_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
# . ${CURR_DIR}/installer_base.sh

# TARGET_ARCH=$(uname -m)

# BAZEL_VERSION="3.7.1"
# BUILDTOOLS_VERSION="3.5.0"

# if [[ "$TARGET_ARCH" == "x86_64" ]]; then
#   # https://docs.bazel.build/versions/master/install-ubuntu.html
#   PKG_NAME="bazel_${BAZEL_VERSION}-linux-x86_64.deb"
#   DOWNLOAD_LINK="https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/${PKG_NAME}"
#   SHA256SUM="2c6c68c23618ac3f37c73ba111f79212b33968217e1a293aa9bf5a17cdd3212b"
#   download_if_not_cached $PKG_NAME $SHA256SUM $DOWNLOAD_LINK

#   apt_get_update_and_install \
#     zlib1g-dev

#   # https://docs.bazel.build/versions/master/install-ubuntu.html#step-3-install-a-jdk-optional
#   # openjdk-11-jdk

#   dpkg -i "${PKG_NAME}"

#   # Cleanup right after installation
#   rm -rf "${PKG_NAME}"

#   ## buildifier ##
#   PKG_NAME="buildifier-${BUILDTOOLS_VERSION}.${TARGET_ARCH}.bin"
#   CHECKSUM="f9a9c082b8190b9260fce2986aeba02a25d41c00178855a1425e1ce6f1169843"
#   DOWNLOAD_LINK="https://github.com/bazelbuild/buildtools/releases/download/${BUILDTOOLS_VERSION}/buildifier"
#   download_if_not_cached "${PKG_NAME}" "${CHECKSUM}" "${DOWNLOAD_LINK}"

#   cp -f ${PKG_NAME} "${SYSROOT_DIR}/bin/buildifier"
#   chmod a+x "${SYSROOT_DIR}/bin/buildifier"
#   rm -rf ${PKG_NAME}

#   info "Done installing bazel ${BAZEL_VERSION} with buildifier ${BUILDTOOLS_VERSION}"

# elif [[ "$TARGET_ARCH" == "aarch64" ]]; then
#   BAZEL_VERSION="5.2.0"
#   ARM64_BINARY="bazel-${BAZEL_VERSION}-linux-arm64"
#   CHECKSUM="af2b09fc30123af7aee992eba285c61758c343480116ba76d880268e40d081a5"
#   DOWNLOAD_LINK="https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/${ARM64_BINARY}"
#   # https://github.com/bazelbuild/bazel/releases/download/3.5.0/bazel-3.5.0-linux-arm64
#   download_if_not_cached "${ARM64_BINARY}" "${CHECKSUM}" "${DOWNLOAD_LINK}"
#   cp -f ${ARM64_BINARY} "${SYSROOT_DIR}/bin/bazel"
#   chmod a+x "${SYSROOT_DIR}/bin/bazel"
#   rm -rf "${ARM64_BINARY}"

#   cp /opt/apollo/rcfiles/bazel_completion.bash /etc/bash_completion.d/bazel

#   PKG_NAME="buildifier-${BUILDTOOLS_VERSION}-linux-arm64"
#   CHECKSUM="19d5b358cb099e264086b26091661fd7548df0a2400e47fd98238cfe0a3e67f9"
#   DOWNLOAD_LINK="https://apollo-system.cdn.bcebos.com/archive/6.0/${PKG_NAME}"
#   download_if_not_cached "${PKG_NAME}" "${CHECKSUM}" "${DOWNLOAD_LINK}"

#   cp -f ${PKG_NAME} "${SYSROOT_DIR}/bin/buildifier"
#   chmod a+x "${SYSROOT_DIR}/bin/buildifier"
#   rm -rf ${PKG_NAME}

#   info "Done installing bazel ${BAZEL_VERSION} with buildifier ${BUILDTOOLS_VERSION}"
# else
#   error "Target arch ${TARGET_ARCH} not supported yet"
#   exit 1
# fi

# # Note(storypku):
# # Used by `apollo.sh config` to determine native cuda compute capability.
# bash ${CURR_DIR}/install_deviceQuery.sh

# curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
# sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
# echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list

# sudo apt update

# sudo apt install --only-upgrade bazel=5.2.0

# rm -f /etc/apt/sources.list.d/bazel.list

# # Clean up cache to reduce layer size.
# apt-get clean && \
#     rm -rf /var/lib/apt/lists/*

#!/bin/bash
# Fail on the first error to prevent cascading errors and undefined states.
set -e

# This script installs Bazel using Bazelisk and Buildifier on a Linux machine.
# Bazelisk is a user-friendly launcher for Bazel. Buildifier helps format and lint Bazel BUILD and .bzl files.

# Navigate to the script's directory to ensure all operations are performed relative to this location.
cd "$(dirname "${BASH_SOURCE[0]}")"

# Update the package list to ensure access to the latest versions of packages and their dependencies.
apt-get update -y

# Install golang, a prerequisite for installing Bazelisk and Buildifier.
apt-get -y install golang

# Download Bazelisk, a Bazel launcher that automatically downloads and runs the correct version of Bazel.
wget https://github.com/bazelbuild/bazelisk/releases/download/v1.25.0/bazelisk-darwin-arm64

# Move the downloaded Bazelisk to /usr/local/bin and rename it to 'bazel',
# making it accessible as the 'bazel' command.
mv bazelisk-darwin-arm64 /usr/local/bin/bazel

# Grant executable permissions to Bazelisk to allow it to run as a program.
chmod +x /usr/local/bin/bazel

# Install Buildifier, a tool for formatting Bazel BUILD and .bzl files, ensuring they adhere to style guidelines.
go install github.com/bazelbuild/buildtools/buildifier@latest

# Move the installed Buildifier from the Go bin directory to /usr/local/bin for system-wide access.
mv ~/go/bin/buildifier /usr/local/bin