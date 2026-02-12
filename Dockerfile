FROM --platform=linux/amd64 python:3.12-slim

RUN pip install --no-cache-dir cia-to-cci

RUN mkdir -p /root/.3ds /data

WORKDIR /data

ENTRYPOINT ["cia-to-cci"]
