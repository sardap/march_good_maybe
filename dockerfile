FROM psarda/devkitproubuntu:2210-gba as GBA-builder

COPY --from=golang:1.19.2-buster /usr/local/go /usr/local/go

ENV PATH "/usr/local/go/bin:${PATH}"
ENV GOROOT /usr/local/go
ENV GOBIN ${GOROOT}/bin

RUN apt-get update -y \
    && apt-get install -y wget libx11-dev libxcursor-dev libgl1-mesa-dev libfontconfig1-dev libbrotli1\
    && go install github.com/sardap/build-good-maybe@b649c2345a9630d80a5415a290653755f964966b

RUN mkdir /app


COPY --from=psarda/asepritecliubuntu:2210 /aseprite /aseprite

ENV PATH "/aseprite/aseprite:${PATH}"
ENV ASEPRITE_PATH /aseprite/aseprite

WORKDIR /app

ENTRYPOINT [ "build-good-maybe" ]
CMD [ "/app/build/gfx", "/app/build.toml", "/app/assets", "/app", "build" ]
