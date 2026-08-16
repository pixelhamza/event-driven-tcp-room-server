# Build Stage
FROM alpine:latest AS builder
RUN apk add --no-cache g++ cmake make
WORKDIR /app
COPY . .
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# Runtime Stage
FROM alpine:latest
RUN apk add --no-cache libstdc++
WORKDIR /app
COPY --from=builder /app/build/chat_server .
EXPOSE 8080
CMD ["./chat_server"]
