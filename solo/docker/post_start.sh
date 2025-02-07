sudo apt-get -y update && sudo apt-get -y install libboost-all-dev \
    libgflags-dev \
    libgoogle-glog-dev \
    libproj-dev \
    libxtst6 \
    libxtst-dev

sudo python3 copylibs.py

echo "chowning /usr/local/lib"
sudo chown -R samehm:domain-users /usr/local/lib