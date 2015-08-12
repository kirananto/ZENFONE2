 #
 # Copyright © 2014, KiranAnto
 #
 # Custom build script
 #
 # This software is licensed under the terms of the GNU General Public
 # License version 2, as published by the Free Software Foundation, and
 # may be copied, distributed, and modified under those terms.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # Please maintain this if you use this script or any part of it
 #
KERNEL_DIR=$PWD
KERN_IMG=$KERNEL_DIR/arch/x86/boot/bzImage
DTBTOOL=$KERNEL_DIR/tools/dtbToolCM
BUILD_START=$(date +"%s")
blue='\033[0;34m'
cyan='\033[0;36m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'
# Modify the following variable if you want to build
export CROSS_COMPILE="/media/kiran/cfa944e1-2a51-4fa1-9334-894a93e41cb9/Android-Development/Kernel/Toolchains/x86_64-linux-4.9/bin/x86_64-linux-"
export USE_CCACHE=1
export ARCH=x86
export KBUILD_BUILD_USER="Kiran.Anto"
export KBUILD_BUILD_HOST="RaZor-Machine"
STRIP="/media/kiran/cfa944e1-2a51-4fa1-9334-894a93e41cb9/Android-Development/Kernel/Toolchains/x86_64-linux-4.9/bin/x86_64-linux-strip"
MODULES_DIR=$KERNEL_DIR/../RaZORBUILDOUTPUT/Common

compile_kernel ()
{
echo -e "**********************************************************************************************"
echo "                    "
echo "                                        Compiling RaZorReborn for Zenfone 2                   "
echo "                    "
echo -e "**********************************************************************************************"
make hd_defconfig
make -j2
if ! [ -a $KERN_IMG ];
then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol"
exit 1
fi
}
case $1 in
clean)
make ARCH=x86 -j8 clean mrproper
rm -rf $KERNEL_DIR/arch/arm/boot/dt.img
;;
*)
compile_kernel
;;
esac
rm $MODULES_DIR/../ZF2OUTPUT/tools/bzImage
cp $KERNEL_DIR/arch/x86/boot/bzImage  $MODULES_DIR/../ZF2OUTPUT/
cd $MODULES_DIR/../ZF2OUTPUT
zipfile="RRV1.0ZF2-$(date +"%Y-%m-%d(%I.%M%p)").zip"
zip -r $zipfile * -x *kernel/.gitignore*
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"
echo -e "$green Uploading "
dropbox_uploader -p upload $MODULES_DIR/../ZF2OUTPUT/$zipfile /
dropbox_uploader share /$zipfile
echo "Enjoy RazorKernel for Zenfone 2"
