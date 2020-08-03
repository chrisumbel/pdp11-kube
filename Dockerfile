FROM ubuntu

ADD pdp11 /pdp11/
ADD pdp11.ini /pdp11/
ADD unix_v7_rl.dsk /pdp11/
ADD run.sh /pdp11/
ADD pdp11.expect /pdp11/
ADD proxy /pdp11/

RUN apt-get update
RUN apt-get install expect -y

WORKDIR /pdp11

EXPOSE 8023
ENTRYPOINT ["/bin/bash", "/pdp11/run.sh"]
