FROM nvidia/cuda:11.8.0-cudnn8-devel-ubuntu22.04

ENV CUDA_LITE 11.4

RUN apt-get update && apt-get install -y --no-install-recommends sudo gnupg2 curl ca-certificates \
    && rm -rf /var/lib/apt/lists/*

ENV CUDA_VERSION 11.4.1

ENV PATH /usr/local/cuda/bin:${PATH}

# nvidia-container-runtime
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
ENV NVIDIA_REQUIRE_CUDA "cuda>=${CUDA_LITE}"

ENV LIBRARY_PATH /usr/local/cuda/lib64/stubs

ENV CUDNN_VERSION 8.6.0.1665

ENV TENSORRT_VERSION 8.5.2
 
COPY rcfiles /opt/apollo/rcfiles
RUN mkdir -p /opt/apollo/installers
COPY installers/installer_base.sh /opt/apollo/installers/installer_base.sh

RUN apt update && DEBIAN_FRONTEND=noninteractive TZ="America/New_York" apt-get -y install tzdata

RUN apt update && apt-get -y install apt-utils \
                                    bc      \
                                    curl    \
                                    file    \
                                    gawk    \
                                    git     \
                                    less    \
                                    lsof    \
                                    python3     \
                                    python3-pip \
                                    python3-distutils \
                                    python3-apt \
                                    python3-dev     \
                                    sed         \
                                    software-properties-common \
                                    sudo    \
                                    unzip   \
                                    vim     \
                                    wget    \
                                    zip     \
                                    xz-utils \
                                    ncurses-dev \
                                    libuuid1 \
                                    uuid-dev \
                                    rsync \
                                    figlet \
                                    nethogs \
                                    sysstat \
                                    build-essential \
                                    autoconf    \
                                    automake    \
                                    gcc       \
                                    g++       \
                                    gdb         \
                                    libtool     \
                                    patch       \
                                    pkg-config      \
                                    libexpat1-dev   \
                                    linux-libc-dev \
                                    lcov \
                                    mesa-common-dev \
                                    libglvnd-dev \
                                    libxcb1-dev \
                                    libtinyxml2-dev \
                                    libsqlite3-dev

RUN pip3 install pycodestyle \
                 flake8 \
                 yapf


############################
# Setup sudo and bash
###########################
COPY installers/install_minimal_environment.sh /opt/apollo/installers/install_minimal_environment.sh
RUN bash /opt/apollo/installers/install_minimal_environment.sh us 22.04

####################
# INSTALL BUILD TOOLS
####################
COPY installers/install_bazel.sh /opt/apollo/installers/install_bazel.sh
RUN bash /opt/apollo/installers/install_bazel.sh

COPY installers/install_cmake.sh /opt/apollo/installers/install_cmake.sh
RUN bash /opt/apollo/installers/install_cmake.sh

COPY installers/install_llvm_clang.sh /opt/apollo/installers/install_llvm_clang.sh
RUN bash /opt/apollo/installers/install_llvm_clang.sh

####################
# INSTALL CYBER DEPS
####################
COPY installers/install_protobuf.sh /opt/apollo/installers/install_protobuf.sh
RUN bash /opt/apollo/installers/install_protobuf.sh

COPY installers/install_fast-rtps.sh /opt/apollo/installers/install_fast-rtps.sh
RUN bash /opt/apollo/installers/install_fast-rtps.sh

COPY installers/install_abseil.sh /opt/apollo/installers/install_abseil.sh
RUN bash /opt/apollo/installers/install_abseil.sh

COPY installers/install_gflags_glog.sh /opt/apollo/installers/install_gflags_glog.sh
RUN bash /opt/apollo/installers/install_gflags_glog.sh


####################
# INSTALL QA TOOLS
####################
COPY installers/install_shellcheck.sh /opt/apollo/installers/install_shellcheck.sh
RUN bash /opt/apollo/installers/install_shellcheck.sh

COPY installers/install_gperftools.sh /opt/apollo/installers/install_gperftools.sh
RUN bash /opt/apollo/installers/install_gperftools.sh

COPY installers/install_shfmt.sh /opt/apollo/installers/install_shfmt.sh
RUN bash /opt/apollo/installers/install_shfmt.sh


####################
# INSTALL VIZ TOOLS
####################
COPY installers/install_qt.sh /opt/apollo/installers/install_qt.sh
RUN bash /opt/apollo/installers/install_qt.sh build 22.04

COPY installers/install_pcl.sh /opt/apollo/installers/install_pcl.sh
RUN bash /opt/apollo/installers/install_pcl.sh

########################
# INSTALL AV MODULE DEPS
########################
COPY installers/install_boost.sh /opt/apollo/installers/install_boost.sh
RUN bash /opt/apollo/installers/install_boost.sh

COPY installers/install_ffmpeg.sh /opt/apollo/installers/install_ffmpeg.sh
RUN bash /opt/apollo/installers/install_ffmpeg.sh

COPY installers/install_proj.sh /opt/apollo/installers/install_proj.sh
RUN bash /opt/apollo/installers/install_proj.sh

COPY installers/install_vtk.sh /opt/apollo/installers/install_vtk.sh
RUN bash /opt/apollo/installers/install_vtk.sh

######################################################
RUN info "Install support for [modules/common] ..."

COPY installers/install_osqp.sh /opt/apollo/installers/install_osqp.sh
RUN bash /opt/apollo/installers/install_osqp.sh

######################################################
RUN info "Install support for [modules/perception] ..."

COPY installers/install_paddle_deps.sh /opt/apollo/installers/install_paddle_deps.sh
RUN bash /opt/apollo/installers/install_paddle_deps.sh

######################################################
RUN info "Install support for [modules/perception] ..."

COPY installers/install_opencv.sh /opt/apollo/installers/install_opencv.sh
RUN bash /opt/apollo/installers/install_opencv.sh

#######################################################
RUN info "Install support for [modules/planning] ..."

COPY installers/install_adolc.sh /opt/apollo/installers/install_adolc.sh
RUN bash /opt/apollo/installers/install_adolc.sh

COPY installers/install_ipopt.sh /opt/apollo/installers/install_ipopt.sh
RUN bash /opt/apollo/installers/install_ipopt.sh

#######################################################

#######################################################
RUN info "Install support for [modules/tools] ..."

COPY installers/install_python_modules.sh /opt/apollo/installers/install_python_modules.sh
RUN bash /opt/apollo/installers/install_python_modules.sh

######################################################
RUN info "Install support for [modules/teleop] ..."

COPY installers/install_openh264.sh /opt/apollo/installers/install_openh264.sh
RUN bash /opt/apollo/installers/install_openh264.sh

######################################################
RUN info "Install support for [modules/audio] ..."
COPY installers/install_fftw3.sh /opt/apollo/installers/install_fftw3.sh
RUN bash /opt/apollo/installers/install_fftw3.sh

# RUN bash /opt/apollo/installers/install_drivers_deps.sh build
# RUN bash /opt/apollo/installers/install_dreamview_deps.sh us
# RUN bash /opt/apollo/installers/install_contrib_deps.sh build
# RUN bash /opt/apollo/installers/install_gpu_support.sh
# RUN bash /opt/apollo/installers/install_release_deps.sh
# RUN bash /opt/apollo/installers/install_tkinter.sh
# RUN bash /opt/apollo/installers/post_install.sh dev

# RUN bash /opt/apollo/installers/install_pkg_repo.sh

# COPY rcfiles/setup.sh /opt/apollo/neo/

# RUN bash /opt/apollo/installers/install_rsdriver.sh
# RUN bash /opt/apollo/installers/install_livox_driver.sh
# RUN bash /opt/apollo/installers/install_hesai2_driver.sh
# RUN bash /opt/apollo/installers/install_vanjee_driver.sh

# RUN pip3 install tensorflow==2.10.0

# CLEAN UP
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*
