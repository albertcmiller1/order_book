########################################################################################################################
# simplehttpserver build stage
########################################################################################################################

FROM alpine:3.17.0 AS build

RUN apk update && \
    apk add --no-cache \
    build-base \
    cmake \
    boost1.80-dev=1.80.0-r3 

WORKDIR /simplehttpserver

COPY  . .
# COPY CMakeLists.txt .

WORKDIR /simplehttpserver/build

# RUN cmake .. 

RUN cmake .. && \
    cmake --build .

########################################################################################################################
# simplehttpserver image
########################################################################################################################

FROM alpine:3.17.0

RUN apk update && \
    apk add --no-cache \
    libstdc++ \
    boost1.80-program_options=1.80.0-r3

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

# COPY ./simplehttpserver/build/simplehttpserver .

# COPY --chown=shs:shs --from=build \
#     ./simplehttpserver/build/simplehttpserver \
#     .    

ENTRYPOINT [ "/simplehttpserver/build/socket" ]

# http://0.0.0.0:40080 