FROM cern/cc7-base
LABEL maintainer="project-lcg-spi-internal@cern.ch"
ENV container docker

# Prepare centos to use systemd
RUN yum update -y \
    && yum install -y systemd \
    && yum clean all \
    && (cd /lib/systemd/system/sysinit.target.wants/; for i in *; do [ $i == systemd-tmpfiles-setup.service ] || rm -f $i; done); \
        rm -fv /lib/systemd/system/multi-user.target.wants/*; \
        rm -fv /etc/systemd/system/*.wants/*; \
        rm -fv /lib/systemd/system/local-fs.target.wants/*; \
        rm -fv /lib/systemd/system/sockets.target.wants/*udev*; \
        rm -fv /lib/systemd/system/sockets.target.wants/*initctl*; \
        rm -fv /lib/systemd/system/basic.target.wants/*; \
        rm -fv /lib/systemd/system/anaconda.target.wants/*; \
        rm -fv /usr/lib/tmpfiles.d/systemd-nologin.conf;

# Install base packages
COPY CI/packages.txt /tmp/packages
RUN yum install -y yum-plugin-ovl \
    && yum update -y \
    && yum install -y $(cat /tmp/packages) \
    && rm -fv /tmp/packages

# Install devtoolset-7
RUN yum install -y centos-release-scl \
    && yum-config-manager --enable rhel-server-rhscl-7-rpms \
    && yum install -y devtoolset-7

# Set the correct timezone
RUN ln -sf /usr/share/zoneinfo/Europe/Zurich /etc/localtime

# Install DIM DNS
RUN cd / \
    && mkdir DIM \
    && cd DIM \
    && wget http://dim.web.cern.ch/dim_v20r26.zip \
    && unzip dim_v20r26.zip \
    && cd dim_v20r26 \
    && cd linux \
    && export LD_LIBRARY_PATH=. \
    && chmod 777 dns 

CMD ["/bin/bash"]
