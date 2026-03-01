FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ make python3 python3-pip iverilog \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /xbasic
COPY . .

RUN make && pip3 install --break-system-packages .

ENTRYPOINT ["xbasic"]
CMD ["--help"]
