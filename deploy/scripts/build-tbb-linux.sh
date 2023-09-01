set -x

# Following Arch package:
# https://git.archlinux.org/svntogit/packages.git/tree/trunk/PKGBUILD?h=packages/intel-tbb
tbb_pkgname="onetbb"
tbb_pkgver="2021.10.0"
tbb_url="https://github.com/oneapi-src/oneTBB/archive/v$tbb_pkgver/$tbb_pkgname-$tbb_pkgver.tar.gz"
tbb_tar_file="onetbb.tar.gz"
work_directory="/tmp"
source_folder="oneTBB-$tbb_pkgver"
make_j="12"
install_folder="/tmp/tbb-install"
if [ "$#" = "1" ]; then
  install_folder="$1"
fi
echo "Installing tbb to $install_folder"

function download_package() {
pushd $work_directory
curl -L $tbb_url > $tbb_tar_file
tar -xf $tbb_tar_file
popd
};

function build_package() {
pushd $work_directory
cd $source_folder
make -j $make_j
popd
}

function install_package() {
pushd $work_directory
cd oneTBB-$tbb_pkgver
install -d $install_folder/lib
install -m755 build/linux_*/*.so* $install_folder/lib
install -d $install_folder/include
cp -a include/tbb $install_folder/include
cmake \
  -DINSTALL_DIR=$install_folder/lib/cmake/TBB \
  -DSYSTEM_NAME=Linux -DTBB_VERSION_FILE=$install_folder/include/tbb/tbb_stddef.h \
  -P cmake/tbb_config_installer.cmake
popd
}

clean_package() {
rm $work_directory/$tbb_tar_file
rm -r $work_directory/$source_folder
}

download_package
build_package
install_package
clean_package
