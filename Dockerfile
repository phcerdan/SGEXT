FROM sgext/base as sgext-scripts
MAINTAINER Pablo Hernandez Cerdan <pablo.hernandez.cerdan@outlook.com>

###################### SGEXT #############
ENV SGEXT_SCRIPTS_BUILD_DIR ${BUILD_PATH}/SGEXT_SCRIPTS-build
ENV SGEXT_SCRIPTS_SRC_FOLDER_NAME SGEXT_SCRIPTS-src
ENV SGEXT_SCRIPTS_SRC_DIR ${BUILD_PATH}/${SGEXT_SCRIPTS_SRC_FOLDER_NAME}

# Copy files from current repo into the docker container
WORKDIR ${SGEXT_SCRIPTS_BUILD_DIR}
COPY . ${SGEXT_SCRIPTS_SRC_DIR}

RUN cmake \
        -G Ninja \
        -DCMAKE_BUILD_TYPE:STRING=Release \
        -DSG_BUILD_TESTING:BOOL=ON \
        -DSG_MODULE_ANALYZE:BOOL=ON \
        -DSG_MODULE_COMPARE:BOOL=ON \
        -DSG_MODULE_LOCATE:BOOL=ON \
        -DSG_MODULE_GENERATE:BOOL=ON \
        -DSG_MODULE_VISUALIZE:BOOL=OFF \
        -DSG_MODULE_VISUALIZE_ENABLE_QT:BOOL=OFF \
        -DVISUALIZE:BOOL=OFF \
        -DITK_DIR:STRING=${ITK_BUILD_DIR} \
        -DDGtal_DIR:STRING=${DGtal_BUILD_DIR} \
        ${SGEXT_SCRIPTS_SRC_DIR} && \
    ninja && \
    find . -name "*.o" -delete && \
    find ../SGEXT_SCRIPTS* -depth -name .git -exec rm -rf '{}' \;
ENV PATH="${SGEXT_SCRIPTS_BUILD_DIR}/cpp-scripts:${PATH}"
