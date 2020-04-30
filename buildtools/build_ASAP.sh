#!/bin/bash
if [ "$1" != "" ]; then
    python_ver="$1"
else
    python_ver="3.8"
fi
if [ "$2" != "" ]; then
    build_gui="$2"
else
    build_gui="false"
fi
echo "Building ASAP with Python ${python_ver}; building GUI = ${build_gui}"
if [ "${build_gui}" = "false" ] ; then \
        cmake ../src -DPugiXML_INCLUDE_DIR=/root/pugixml-1.9/src/ -DOPENSLIDE_INCLUDE_DIR=/usr/include/openslide \
                    -DWRAP_MULTIRESOLUTIONIMAGEINTERFACE_PYTHON=TRUE -DCMAKE_INSTALL_PREFIX=/root/install \
                    -DBUILD_ASAP=TRUE -DBUILD_EXECUTABLES=TRUE -DBUILD_IMAGEPROCESSING=TRUE -DBUILD_MULTIRESOLUTIONIMAGEINTERFACE_VSI_SUPPORT=TRUE -DBUILD_TESTS=TRUE -DCMAKE_BUILD_TYPE=Release \
                    -DDCMTKJPEG_INCLUDE_DIR=/root -DDCMTKJPEG_LIBRARY=/usr/lib/libijg8.so -DUNITTEST_INCLUDE_DIR=/usr/include/UnitTest++ \
                    -DUNITTEST_LIBRARY=/usr/lib/x86_64-linux-gnu/libUnitTest++.so -DUNITTEST_LIBRARY_DEBUG=/usr/lib/x86_64-linux-gnu/libUnitTest++.so -DPACKAGE_ON_INSTALL=TRUE \
                    -DPYTHON_DEBUG_LIBRARY=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                    -DPYTHON_LIBRARY_DEBUG=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                    -DPYTHON_LIBRARY=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                    -DPYTHON_INCLUDE_DIR=/root/miniconda3/envs/build_python${python_ver}/include/python${python_ver} \
                    -DPYTHON_EXECUTABLE=/root/miniconda3/envs/build_python${python_ver}/bin/python \
                    -DPYTHON_NUMPY_INCLUDE_DIR=/root/miniconda3/envs/build_python${python_ver}/lib/python${python_ver}/site-packages/numpy/core/include \
    ; else \
        echo "Skipping GUI..."
        cmake ../src -DPugiXML_INCLUDE_DIR=/root/pugixml-1.9/src/ -DOPENSLIDE_INCLUDE_DIR=/usr/include/openslide \
                 -DWRAP_MULTIRESOLUTIONIMAGEINTERFACE_PYTHON=TRUE -DCMAKE_INSTALL_PREFIX=/root/install \
                 -DBUILD_MULTIRESOLUTIONIMAGEINTERFACE_VSI_SUPPORT=TRUE -DCMAKE_BUILD_TYPE=Release \
                 -DDCMTKJPEG_INCLUDE_DIR=/root -DDCMTKJPEG_LIBRARY=/usr/lib/libijg8.so -DPACKAGE_ON_INSTALL=TRUE \
                 -DPYTHON_DEBUG_LIBRARY=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                 -DPYTHON_LIBRARY_DEBUG=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                 -DPYTHON_LIBRARY=/root/miniconda3/envs/build_python${python_ver}/lib/libpython${python_ver}.so \
                 -DPYTHON_INCLUDE_DIR=/root/miniconda3/envs/build_python${python_ver}/include/python${python_ver} \
                 -DPYTHON_EXECUTABLE=/root/miniconda3/envs/build_python${python_ver}/bin/python \
                 -DPYTHON_NUMPY_INCLUDE_DIR=/root/miniconda3/envs/build_python${python_ver}/lib/python${python_ver}/site-packages/numpy/core/include \
    ; fi
export LD_LIBRARY_PATH=/root/miniconda3/envs/build_python${python_ver}/lib
make package
if [ "${build_gui}" = "false" ] ; then \
        for file in *.deb; do mv $file /artifacts/$(basename $file .deb)-py${python_ver}.deb; done \
; else \
        for file in *.deb; do mv $file /artifacts/$(basename $file .deb)-nogui-py${python_ver}.deb; done \
; fi