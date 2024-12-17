load("//third_party/rules_python:init.bzl", apollo_neo_3rd_rules_python_repo = "init")
load("//third_party/py:init.bzl", apollo_neo_3rd_py_repo = "init")
load("//third_party/centerpoint_infer_op:init.bzl", apollo_neo_3rd_centerpoint_infer_op_repo = "init")
load("//third_party/grpc:init.bzl", apollo_neo_3rd_grpc_repo = "init")
load("//third_party/tensorrt:init.bzl", apollo_neo_3rd_tensorrt_repo = "init")
load("//third_party/paddleinference:init.bzl", apollo_neo_3rd_paddleinference_repo = "init")
load("//third_party/vtk:init.bzl", apollo_neo_3rd_vtk_repo = "init")
load("//third_party/npp:init.bzl", apollo_neo_3rd_npp_repo = "init")
load("//third_party/caddn_infer_op:init.bzl", apollo_neo_3rd_caddn_infer_op_repo = "init")
load("//third_party/gtest:init.bzl", apollo_neo_3rd_gtest_repo = "init")
load("//third_party/cpplint:init.bzl", apollo_neo_3rd_cpplint_repo = "init")
load("//third_party/civetweb:init.bzl", apollo_neo_3rd_civetweb_repo = "init")
load("//third_party/yaml_cpp:init.bzl", apollo_neo_3rd_yaml_cpp_repo = "init")
load("//third_party/bazel_skylib:init.bzl", apollo_neo_3rd_bazel_skylib_repo = "init")
load("//third_party/gpus:init.bzl", apollo_neo_3rd_gpus_repo = "init")
load("//third_party/nlohmann_json:init.bzl", apollo_neo_3rd_nlohmann_json_repo = "init")
load("//third_party/rules_proto:init.bzl", apollo_neo_3rd_rules_proto_repo = "init")
load("//third_party/eigen3:init.bzl", apollo_neo_3rd_eigen3_repo = "init")
load("//third_party/pcl:init.bzl", apollo_neo_3rd_pcl_repo = "init")
load("//third_party/localization_msf:init.bzl", apollo_neo_3rd_localization_msf_repo = "init")
load("//third_party/qt5:init.bzl", apollo_neo_3rd_qt5_repo = "init")
def clean_dep(dep):
    return str(Label(dep))
def system_python3_tk_repo():
    native.new_local_repository(
        name = "python3-tk",
        build_file = clean_dep("//dev/bazel:python3-tk.BUILD"),
        path = "/usr/include",
    )
def system_bvar_repo():
    native.new_local_repository(
        name = "bvar",
        build_file = clean_dep("//dev/bazel:bvar.BUILD"),
        path = "/usr/include",
    )
def system_libhdf5_dev_repo():
    native.new_local_repository(
        name = "libhdf5-dev",
        build_file = clean_dep("//dev/bazel:libhdf5-dev.BUILD"),
        path = "/usr/include",
    )
def system_libxt_dev_repo():
    native.new_local_repository(
        name = "libxt-dev",
        build_file = clean_dep("//dev/bazel:libxt-dev.BUILD"),
        path = "/usr/include",
    )
def system_libjsoncpp_dev_repo():
    native.new_local_repository(
        name = "libjsoncpp-dev",
        build_file = clean_dep("//dev/bazel:libjsoncpp-dev.BUILD"),
        path = "/usr/include",
    )
def system_libxml2_dev_repo():
    native.new_local_repository(
        name = "libxml2-dev",
        build_file = clean_dep("//dev/bazel:libxml2-dev.BUILD"),
        path = "/usr/include",
    )
def system_sysstat_repo():
    native.new_local_repository(
        name = "sysstat",
        build_file = clean_dep("//dev/bazel:sysstat.BUILD"),
        path = "/usr/include",
    )
def system_nethogs_repo():
    native.new_local_repository(
        name = "nethogs",
        build_file = clean_dep("//dev/bazel:nethogs.BUILD"),
        path = "/usr/include",
    )
def system_libglew_dev_repo():
    native.new_local_repository(
        name = "libglew-dev",
        build_file = clean_dep("//dev/bazel:libglew-dev.BUILD"),
        path = "/usr/include",
    )
def system_libgl1_mesa_dev_repo():
    native.new_local_repository(
        name = "libgl1-mesa-dev",
        build_file = clean_dep("//dev/bazel:libgl1-mesa-dev.BUILD"),
        path = "/usr/include",
    )
def system_libqhull_dev_repo():
    native.new_local_repository(
        name = "libqhull-dev",
        build_file = clean_dep("//dev/bazel:libqhull-dev.BUILD"),
        path = "/usr/include",
    )
def system_gperftools_repo():
    native.new_local_repository(
        name = "gperftools",
        build_file = clean_dep("//dev/bazel:gperftools.BUILD"),
        path = "/usr/include",
    )
def system_libtinyxml2_dev_repo():
    native.new_local_repository(
        name = "tinyxml2",
        build_file = clean_dep("//dev/bazel:libtinyxml2-dev.BUILD"),
        path = "/usr/include",
    )
def system_libpcap08_repo():
    native.new_local_repository(
        name = "libpcap0.8",
        build_file = clean_dep("//dev/bazel:libpcap0.8.BUILD"),
        path = "/usr/include",
    )
def system_libdouble_conversion_dev_repo():
    native.new_local_repository(
        name = "libdouble-conversion-dev",
        build_file = clean_dep("//dev/bazel:libdouble-conversion-dev.BUILD"),
        path = "/usr/include",
    )
def system_libunwind_dev_repo():
    native.new_local_repository(
        name = "libunwind-dev",
        build_file = clean_dep("//dev/bazel:libunwind-dev.BUILD"),
        path = "/usr/include",
    )
def system_libflann_dev_repo():
    native.new_local_repository(
        name = "libflann-dev",
        build_file = clean_dep("//dev/bazel:libflann-dev.BUILD"),
        path = "/usr/include",
    )
def system_libfreetype6_repo():
    native.new_local_repository(
        name = "libfreetype6",
        build_file = clean_dep("//dev/bazel:libfreetype6.BUILD"),
        path = "/usr/include",
    )
def system_libleveldb_dev_repo():
    native.new_local_repository(
        name = "libleveldb-dev",
        build_file = clean_dep("//dev/bazel:libleveldb-dev.BUILD"),
        path = "/usr/include",
    )
def system_libopenblas_dev_repo():
    native.new_local_repository(
        name = "libopenblas-dev",
        build_file = clean_dep("//dev/bazel:libopenblas-dev.BUILD"),
        path = "/usr/include",
    )
def system_libopenni0_repo():
    native.new_local_repository(
        name = "libopenni0",
        build_file = clean_dep("//dev/bazel:libopenni0.BUILD"),
        path = "/usr/include",
    )
def system_libusb_10_0_repo():
    native.new_local_repository(
        name = "libusb-1.0-0",
        build_file = clean_dep("//dev/bazel:libusb-1.0-0.BUILD"),
        path = "/usr/include",
    )
def system_uuid_dev_repo():
    native.new_local_repository(
        name = "uuid",
        build_file = clean_dep("//dev/bazel:uuid-dev.BUILD"),
        path = "/usr/include",
    )
def system_libtiff5_repo():
    native.new_local_repository(
        name = "libtiff5",
        build_file = clean_dep("//dev/bazel:libtiff5.BUILD"),
        path = "/usr/include",
    )
def system_libncurses5_dev_repo():
    native.new_local_repository(
        name = "ncurses5",
        build_file = clean_dep("//dev/bazel:libncurses5-dev.BUILD"),
        path = "/usr/include",
    )
def system_libwebp_dev_repo():
    native.new_local_repository(
        name = "libwebp-dev",
        build_file = clean_dep("//dev/bazel:libwebp-dev.BUILD"),
        path = "/usr/include",
    )
def system_libgtk20_0_repo():
    native.new_local_repository(
        name = "libgtk2.0-0",
        build_file = clean_dep("//dev/bazel:libgtk2.0-0.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_opencv_repo():
    native.new_local_repository(
        name = "opencv",
        build_file = clean_dep("//dev/bazel:3rd-opencv.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-opencv/latest",
    )
def system_vanjee_driver_repo():
    native.new_local_repository(
        name = "vanjee-driver",
        build_file = clean_dep("//dev/bazel:vanjee-driver.BUILD"),
        path = "/usr/include",
    )
def system_hesai2_driver_repo():
    native.new_local_repository(
        name = "hesai2-driver",
        build_file = clean_dep("//dev/bazel:hesai2-driver.BUILD"),
        path = "/usr/include",
    )
def system_seyond_driver_repo():
    native.new_local_repository(
        name = "seyond-driver",
        build_file = clean_dep("//dev/bazel:seyond-driver.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_boost_repo():
    native.new_local_repository(
        name = "boost",
        build_file = clean_dep("//dev/bazel:3rd-boost.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-boost/latest",
    )
def system_rsdriver_repo():
    native.new_local_repository(
        name = "rsdriver",
        build_file = clean_dep("//dev/bazel:rsdriver.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_gflags_repo():
    native.new_local_repository(
        name = "com_github_gflags_gflags",
        build_file = clean_dep("//dev/bazel:3rd-gflags.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-gflags/latest",
    )
def apollo_neo_3rd_ad_rss_lib_repo():
    native.new_local_repository(
        name = "ad_rss_lib",
        build_file = clean_dep("//dev/bazel:3rd-ad-rss-lib.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-ad-rss-lib/latest",
    )
def system_coinor_libipopt_dev_repo():
    native.new_local_repository(
        name = "coinor-libipopt-dev",
        build_file = clean_dep("//dev/bazel:coinor-libipopt-dev.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_ipopt_repo():
    native.new_local_repository(
        name = "ipopt",
        build_file = clean_dep("//dev/bazel:3rd-ipopt.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-ipopt/latest",
    )
def system_libadolc_dev_repo():
    native.new_local_repository(
        name = "adolc",
        build_file = clean_dep("//dev/bazel:libadolc-dev.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_glog_repo():
    native.new_local_repository(
        name = "com_github_google_glog",
        build_file = clean_dep("//dev/bazel:3rd-glog.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-glog/latest",
    )
def apollo_neo_3rd_fftw3_repo():
    native.new_local_repository(
        name = "fftw3",
        build_file = clean_dep("//dev/bazel:3rd-fftw3.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-fftw3/latest",
    )
def system_libpcap_dev_repo():
    native.new_local_repository(
        name = "libpcap-dev",
        build_file = clean_dep("//dev/bazel:libpcap-dev.BUILD"),
        path = "/usr/include",
    )
def system_libnuma_dev_repo():
    native.new_local_repository(
        name = "libnuma-dev",
        build_file = clean_dep("//dev/bazel:libnuma-dev.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_libtorch_gpu_repo():
    native.new_local_repository(
        name = "libtorch_gpu",
        build_file = clean_dep("//dev/bazel:3rd-libtorch-gpu.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-libtorch-gpu/latest",
    )
def apollo_neo_3rd_osqp_repo():
    native.new_local_repository(
        name = "osqp",
        build_file = clean_dep("//dev/bazel:3rd-osqp.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-osqp/latest",
    )
def apollo_neo_3rd_libtorch_cpu_repo():
    native.new_local_repository(
        name = "libtorch_cpu",
        build_file = clean_dep("//dev/bazel:3rd-libtorch-cpu.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-libtorch-cpu/latest",
    )
def system_libx264_dev_repo():
    native.new_local_repository(
        name = "libx264-dev",
        build_file = clean_dep("//dev/bazel:libx264-dev.BUILD"),
        path = "/usr/include",
    )
def system_libsqlite3_dev_repo():
    native.new_local_repository(
        name = "sqlite3",
        build_file = clean_dep("//dev/bazel:libsqlite3-dev.BUILD"),
        path = "/usr/include",
    )
def system_libvpx_dev_repo():
    native.new_local_repository(
        name = "libvpx-dev",
        build_file = clean_dep("//dev/bazel:libvpx-dev.BUILD"),
        path = "/usr/include",
    )
def system_libvorbisenc2_repo():
    native.new_local_repository(
        name = "libvorbisenc2",
        build_file = clean_dep("//dev/bazel:libvorbisenc2.BUILD"),
        path = "/usr/include",
    )
def system_libvorbis0a_repo():
    native.new_local_repository(
        name = "libvorbis0a",
        build_file = clean_dep("//dev/bazel:libvorbis0a.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_absl_repo():
    native.new_local_repository(
        name = "com_google_absl",
        build_file = clean_dep("//dev/bazel:3rd-absl.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-absl/latest",
    )
def system_livox_driver_repo():
    native.new_local_repository(
        name = "livox-driver",
        build_file = clean_dep("//dev/bazel:livox-driver.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_proj_repo():
    native.new_local_repository(
        name = "proj",
        build_file = clean_dep("//dev/bazel:3rd-proj.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-proj/latest",
    )
def apollo_neo_3rd_adv_plat_repo():
    native.new_local_repository(
        name = "adv_plat",
        build_file = clean_dep("//dev/bazel:3rd-adv-plat.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-adv-plat/latest",
    )
def system_libfdk_aac_dev_repo():
    native.new_local_repository(
        name = "libfdk-aac-dev",
        build_file = clean_dep("//dev/bazel:libfdk-aac-dev.BUILD"),
        path = "/usr/include",
    )
def system_libmp3lame0_repo():
    native.new_local_repository(
        name = "libmp3lame0",
        build_file = clean_dep("//dev/bazel:libmp3lame0.BUILD"),
        path = "/usr/include",
    )
def system_libass9_repo():
    native.new_local_repository(
        name = "libass9",
        build_file = clean_dep("//dev/bazel:libass9.BUILD"),
        path = "/usr/include",
    )
def apollo_neo_3rd_portaudio_repo():
    native.new_local_repository(
        name = "portaudio",
        build_file = clean_dep("//dev/bazel:3rd-portaudio.BUILD"),
        path = "/opt/apollo/neo/packages/3rd-portaudio/latest",
    )
def init_deps():
    apollo_neo_3rd_rules_python_repo()
    apollo_neo_3rd_py_repo()
    apollo_neo_3rd_centerpoint_infer_op_repo()
    apollo_neo_3rd_grpc_repo()
    apollo_neo_3rd_tensorrt_repo()
    apollo_neo_3rd_paddleinference_repo()
    apollo_neo_3rd_vtk_repo()
    apollo_neo_3rd_npp_repo()
    apollo_neo_3rd_caddn_infer_op_repo()
    apollo_neo_3rd_gtest_repo()
    apollo_neo_3rd_cpplint_repo()
    apollo_neo_3rd_civetweb_repo()
    apollo_neo_3rd_yaml_cpp_repo()
    apollo_neo_3rd_bazel_skylib_repo()
    apollo_neo_3rd_gpus_repo()
    apollo_neo_3rd_nlohmann_json_repo()
    apollo_neo_3rd_rules_proto_repo()
    apollo_neo_3rd_eigen3_repo()
    apollo_neo_3rd_pcl_repo()
    apollo_neo_3rd_localization_msf_repo()
    apollo_neo_3rd_qt5_repo()
    system_python3_tk_repo()
    system_bvar_repo()
    system_libhdf5_dev_repo()
    system_libxt_dev_repo()
    system_libjsoncpp_dev_repo()
    system_libxml2_dev_repo()
    system_sysstat_repo()
    system_nethogs_repo()
    system_libglew_dev_repo()
    system_libgl1_mesa_dev_repo()
    system_libqhull_dev_repo()
    system_gperftools_repo()
    system_libtinyxml2_dev_repo()
    system_libpcap08_repo()
    system_libdouble_conversion_dev_repo()
    system_libunwind_dev_repo()
    system_libflann_dev_repo()
    system_libfreetype6_repo()
    system_libleveldb_dev_repo()
    system_libopenblas_dev_repo()
    system_libopenni0_repo()
    system_libusb_10_0_repo()
    system_uuid_dev_repo()
    system_libtiff5_repo()
    system_libncurses5_dev_repo()
    system_libwebp_dev_repo()
    system_libgtk20_0_repo()
    apollo_neo_3rd_opencv_repo()
    system_vanjee_driver_repo()
    system_hesai2_driver_repo()
    system_seyond_driver_repo()
    apollo_neo_3rd_boost_repo()
    system_rsdriver_repo()
    apollo_neo_3rd_gflags_repo()
    apollo_neo_3rd_ad_rss_lib_repo()
    system_coinor_libipopt_dev_repo()
    apollo_neo_3rd_ipopt_repo()
    system_libadolc_dev_repo()
    apollo_neo_3rd_glog_repo()
    apollo_neo_3rd_fftw3_repo()
    system_libpcap_dev_repo()
    system_libnuma_dev_repo()
    apollo_neo_3rd_libtorch_gpu_repo()
    apollo_neo_3rd_osqp_repo()
    apollo_neo_3rd_libtorch_cpu_repo()
    system_libx264_dev_repo()
    system_libsqlite3_dev_repo()
    system_libvpx_dev_repo()
    system_libvorbisenc2_repo()
    system_libvorbis0a_repo()
    apollo_neo_3rd_absl_repo()
    system_livox_driver_repo()
    apollo_neo_3rd_proj_repo()
    apollo_neo_3rd_adv_plat_repo()
    system_libfdk_aac_dev_repo()
    system_libmp3lame0_repo()
    system_libass9_repo()
    apollo_neo_3rd_portaudio_repo()