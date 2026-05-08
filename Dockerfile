FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest AS builder

WORKDIR /app
COPY . .

# Сборка в отдельной папке
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Финальный образ
FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest

COPY --from=builder /app/build/inventory-api /usr/local/bin/inventory-api
RUN chmod +x /usr/local/bin/inventory-api

EXPOSE 8080

ENTRYPOINT ["/usr/local/bin/inventory-api"]
CMD ["--config", "/etc/inventory/config.yaml"]