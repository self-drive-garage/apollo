FROM nvidia/cuda:11.8.0-cudnn8-devel-ubuntu22.04

RUN apt update && apt install -y python3 \
    python3-pip \
    python3-apt \
    wget \
    rsync \
    figlet \
    nethogs \
    sysstat \
    libboost-all-dev \
    libgflags-dev \
    libgoogle-glog-dev \
    libproj-dev \
    libxtst6 \
    libxtst-dev

RUN pip3 install --default-timeout 30 ansible watchdog yattag bokeh flask

COPY aem/galaxy-requirements.yaml /galaxy-requirements.yaml

COPY aem/ansible /ansible

RUN mkdir -pv /opt/apollo/neo/bin /opt/apollo/neo/src && chmod -R 777 /opt/apollo/neo

ENV CUDNN_VERSION 8.9.6.50

ENV TENSORRT_VERSION 8.6.1.6

ENV PATH /opt/apollo/sysroot/bin:$PATH

ENV PATH /opt/apollo/neo/bin:$PATH

RUN ansible-galaxy collection install -f -r galaxy-requirements.yaml && \
    ansible-playbook ansible/playbook.yaml && \
    rm -rf /opt/apollo/sysroot/Qt* /usr/lib/x86_64-linux-gnu/libnvinfer_static.a && \
    apt clean && \
    rm -rf /var/cache/apollo/distfiles/* /var/cache/apt/archives/*

COPY docker/build/installers /opt/installers

RUN /opt/installers/install_osqp.sh

RUN echo 'fs.inotify.max_user_watches=524288' | tee -a /etc/sysctl.conf && sysctl -p

RUN touch .installed && chmod 777 .installed

RUN chmod 777 /etc/profile.d/debuginfod.sh

RUN pip3 install --upgrade --force-reinstall protobuf==3.19.6 grpcio-tools==1.48.0 grpcio==1.48.0

RUN for i in 71 72 73 74 75;do sed  -i "${i}s/^[[:space:]]*#define/#define/" "/usr/local/cuda/include/cusolver_common.h";done

RUN apt clean

RUN rm -rf galaxy-requirements.yaml ansible
