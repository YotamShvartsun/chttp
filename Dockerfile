FROM alpine

COPY . /server
WORKDIR server
# Change some enviorment vars to make sure the build will be sucssesful
ENV LD_LIBRARY_PATH=/usr/local/lib64
ENV NODE_ENV=production
# install the tools required for compilation
RUN apk add cmake make g++ nodejs npm
# build CHttp
RUN mkdir build && cd build && cmake .. && make && make install
# Build the example site - the server
RUN mkdir examples/build && cd examples/build && cmake .. && make
# install the front-end deps and build the front end
RUN cd examples/client && npm install && npm run-script build
# Run the example server
WORKDIR examples/build
CMD ["./examples"]
