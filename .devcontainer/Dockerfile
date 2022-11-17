# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.185.0/containers/cpp/.devcontainer/base.Dockerfile
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# See here for Node.ja: https://github.com/microsoft/vscode-dev-containers/blob/main/containers/python-3/.devcontainer/base.Dockerfile
# Copy library scripts to execute
#COPY .devcontainer/library-scripts/*.sh .devcontainer/library-scripts/*.env /tmp/library-scripts/
COPY library-scripts/*.sh library-scripts/*.env /tmp/library-scripts/

# [Choice] Node.js version: none, lts, 16, 14, 12, 10
#ARG NODE_VERSION="none"
ARG NODE_VERSION="lts/*"
ENV NVM_DIR=/usr/local/share/nvm
ENV NVM_SYMLINK_CURRENT=true \
    PATH=${NVM_DIR}/current/bin:${PATH}
RUN bash /tmp/library-scripts/node-debian.sh "${NVM_DIR}" "${NODE_VERSION}" "${USERNAME}" \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# Remove library scripts for final image
RUN rm -rf /tmp/library-scripts

# Install PXT
RUN npm install -g pxt

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
