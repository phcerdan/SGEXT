FROM sgext/base
MAINTAINER Pablo Hernandez Cerdan <pablo.hernandez.cerdan@outlook.com>

###################### SGEXT #############
ENV SGEXT_BUILD_DIR ${BUILD_PATH}/SGEXT-build
ENV SGEXT_SRC_FOLDER_NAME SGEXT-src
ENV SGEXT_SRC_DIR ${BUILD_PATH}/${SGEXT_SRC_FOLDER_NAME}

# Copy files from current repo into the docker container
WORKDIR ${SGEXT_BUILD_DIR}
COPY . ${SGEXT_SRC_DIR}

# intel-tbb (installed via apt) is installed in non-default location
ENV TBB_ROOT /opt/intel/tbb
RUN cmake \
        -G Ninja \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DSG_BUILD_TESTING:BOOL=ON \
        -DSG_MODULE_ANALYZE:BOOL=ON \
        -DSG_MODULE_COMPARE:BOOL=ON \
        -DSG_MODULE_LOCATE:BOOL=ON \
        -DSG_MODULE_GENERATE:BOOL=ON \
        -DSG_MODULE_DYNAMICS:BOOL=ON \
        -DSG_MODULE_VISUALIZE:BOOL=ON \
        -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
        -DSG_MODULE_TREE:BOOL=ON \
        -DITK_DIR:STRING=${ITK_BUILD_DIR} \
        -DDGtal_DIR:STRING=${DGtal_BUILD_DIR} \
        -DVTK_DIR:STRING=${VTK_BUILD_DIR} \
        ${SGEXT_SRC_DIR} && \
    ninja && \
    find . -name "*.o" -delete && \
    find ../SGEXT* -depth -name .git -exec rm -rf '{}' \;
ENV PATH="${SGEXT_BUILD_DIR}/cpp-scripts:${SGEXT_BUILD_DIR}/modules/tree/cli:${SGEXT_BUILD_DIR}/modules/generate/bin:${PATH}"

# Build-time metadata as defined at http://label-schema.org
ARG BUILD_DATE
ARG IMAGE=sgext/linux
ARG VERSION=latest
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name=$IMAGE \
      org.label-schema.version=$VERSION \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="1.0"
