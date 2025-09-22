# First stage: build
FROM debian:bookworm-slim as build

WORKDIR /src

# Copy necessary files
COPY CMakeLists.txt /src/
COPY main.cpp /src/
COPY simulation.cpp /src/
COPY simulation.h /src/
COPY TestCoverage.cpp /src/
COPY a.out /src/
COPY include /src/include
COPY disease_in.ini /src/

# Install dependencies
RUN apt-get update && \
    apt-get install -y build-essential cmake && \
    apt-get clean

WORKDIR /src/build

# Run cmake and make to build the project
RUN cmake .. && make

# Second stage: runtime
FROM debian:bookworm-slim as run

WORKDIR /app

# Copy the built executable from the build stage
COPY --from=build /src/build/main /app
COPY --from=build /src/build/test /app

# Copy the necessary runtime files
COPY disease_in.ini /app/

# Run the tests just to be sure (optional step)
RUN /app/test

# Set the working directory
WORKDIR /scratch

# Execute the simulation on container start
ENTRYPOINT ["/app/main"]
