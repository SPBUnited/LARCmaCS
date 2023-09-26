FROM mathworks/matlab:r2020b

FROM robocupssl/ubuntu-vnc:latest

USER root

RUN apt-get update && \
    apt-get install -y qt5-default && \
    apt-get install -y g++ && \
    apt-get install -y protobuf-compiler && \
    apt-get install -y make && \
    apt-get install -y iputils-ping && \
    apt-get install -y telnet && \
    apt-get install -y nano && \
    apt-get install -y csh


COPY --from=0 /opt/matlab /opt/matlab

RUN chmod 777 /opt/matlab/R2020b && \
    mkdir -p /opt/matlab/R2020b/licenses && chmod 777 /opt/matlab/R2020b/licenses

RUN ln -s /opt/matlab/R2020b/bin/glnxa64/libicudata.so.64 /opt/matlab/R2020b/bin/glnxa64/libicudata.so && \
    mkdir /opt/matlab/R2020b/bin/glnxa64/qt && \
    mv /opt/matlab/R2020b/bin/glnxa64/libQt5* /opt/matlab/R2020b/bin/glnxa64/qt


USER default

RUN git clone https://github.com/petr-konovalov/MLscripts.git && \
    cd MLscripts && git checkout new_algo

RUN git clone https://github.com/petr-konovalov/LARCmaCS && \
    cd LARCmaCS && git checkout RoboCup2021 && \
    mkdir build && cd build && \
    qmake ../LARCmaCS.pro -spec linux-g++ && \
    make


