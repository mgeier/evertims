# build libevert and evertims for Linux / MacOS
# ---------------------------------------------

OUTPUTDIR=linux
BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# ------------------------------------------------------------
# Build libevert
# ------------------------------------------------------------
$BUILDDIR/../libevert/build/build_linux.sh

# ------------------------------------------------------------
# Build evertims
# ------------------------------------------------------------
$BUILDDIR/../evertims/build/build_linux.sh
