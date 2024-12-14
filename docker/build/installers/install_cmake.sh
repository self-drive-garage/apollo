#!/usr/bin/env bash
###############################################################################
# Copyright 2020 The Apollo Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################
# Fail on first error.
set -e

cd "$(dirname "${BASH_SOURCE[0]}")"
. ./installer_base.sh

VERSION="3.31.2"
TARGET_ARCH="$(uname -m)"

function symlink_if_not_exist() {
    local dest="/usr/local/bin/cmake"
    if [[ ! -e "${dest}" ]]; then
        info "Created symlink ${dest} for convenience."
        ln -s ${SYSROOT_DIR}/bin/cmake /usr/local/bin/cmake
    fi
}

CMAKE_SH=
CHECKSUM=
if [[ "${TARGET_ARCH}" == "x86_64" ]]; then
    CMAKE_SH="cmake-${VERSION}-linux-x86_64.sh"
    CHECKSUM="b81cf3f4892683133f330cd7c016c28049b5725617db24ca8763360883545d34"
elif [[ "${TARGET_ARCH}" == "aarch64" ]]; then
    CMAKE_SH="cmake-${VERSION}-linux-aarch64.sh"
    CHECKSUM="85cc81f782cd8b5ac346e570ad5cfba3bdbe5aa01f27f7ce6266c4cef9334255"
fi

DOWNLOAD_LINK="https://github.com/Kitware/CMake/releases/download/v${VERSION}/${CMAKE_SH}"
download_if_not_cached "${CMAKE_SH}" "${CHECKSUM}" "${DOWNLOAD_LINK}"

chmod a+x ${CMAKE_SH}
./${CMAKE_SH} --skip-license --prefix="${SYSROOT_DIR}"
symlink_if_not_exist
rm -fr ${CMAKE_SH}
