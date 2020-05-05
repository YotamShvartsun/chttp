FROM alpine
COPY . /server
WORKDIR server
RUN ls
RUN apk add cmake make g++ nodejs npm
ENV LD_LIBRARY_PATH=/usr/local/lib64
RUN mkdir build && cd build && cmake .. && make && make install
RUN mkdir examples/build && cd examples/build && cmake .. && make
RUN cd examples/client && npm install && npm run-script build
WORKDIR examples/build
CMD ["./examples"]
EXPOSE 80
