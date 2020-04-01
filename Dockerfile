FROM sgext/base as sgext-sgext
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
        -DSG_MODULE_VISUALIZE:BOOL=OFF \
        -DSG_MODULE_VISUALIZE_WITH_QT:BOOL=OFF \
        -DITK_DIR:STRING=${ITK_BUILD_DIR} \
        -DDGtal_DIR:STRING=${DGtal_BUILD_DIR} \
        ${SGEXT_SRC_DIR} && \
    ninja && \
    find . -name "*.o" -delete && \
    find ../SGEXT* -depth -name .git -exec rm -rf '{}' \;
ENV PATH="${SGEXT_BUILD_DIR}/cpp-scripts:${PATH}"
