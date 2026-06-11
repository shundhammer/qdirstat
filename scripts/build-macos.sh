#!/usr/bin/env bash
# Build a self-contained qdirstat.app (and optional .dmg) on macOS.
# Requires Homebrew: brew install qt qt5compat
set -euo pipefail

clean=0
make_dmg=1
for arg in "$@"; do
    case "$arg" in
        --clean)    clean=1 ;;
        --no-dmg)   make_dmg=0 ;;
        --dmg)      make_dmg=1 ;;
        -h|--help)
            sed -n '2,5p; /^for arg/,/^done/p' "$0"
            exit 0 ;;
        *) echo "unknown option: $arg" >&2; exit 2 ;;
    esac
done

repo="$(cd "$(dirname "$0")/.." && pwd)"
build="$repo/build"

command -v qmake6      >/dev/null || { echo "qmake6 not found (brew install qt)" >&2; exit 1; }
command -v macdeployqt6 >/dev/null || { echo "macdeployqt6 not found (brew install qt)" >&2; exit 1; }

gettext_prefix="$(brew --prefix gettext 2>/dev/null || echo /opt/homebrew/opt/gettext)"
[[ -f "$gettext_prefix/include/libintl.h" ]] || {
    echo "libintl.h not found under $gettext_prefix (brew install gettext)" >&2
    exit 1
}

if (( clean )) || [[ ! -f "$build/Makefile" ]]; then
    rm -rf "$build"
    mkdir -p "$build"
    (cd "$build" && qmake6 "$repo/src/src.pro" \
        "QT+=svg svgwidgets" \
        "INCLUDEPATH+=$gettext_prefix/include" \
        "LIBS+=-L$gettext_prefix/lib -lintl")
fi

(cd "$build" && make -j"$(sysctl -n hw.ncpu)")

# Strip the qmake-baked /opt/homebrew/lib rpath BEFORE macdeployqt runs so the
# bundled binary doesn't dual-load Qt on a developer machine. On a friend's Mac
# this rpath would dead-end harmlessly, but on this machine it causes
# duplicate-class warnings and possible spurious crashes.
exe="$build/qdirstat.app/Contents/MacOS/qdirstat"
if otool -l "$exe" | grep -q '/opt/homebrew/lib'; then
    install_name_tool -delete_rpath /opt/homebrew/lib "$exe"
fi

deploy_args=( "$build/qdirstat.app" -verbose=2 )
(( make_dmg )) && deploy_args+=( -dmg )
macdeployqt6 "${deploy_args[@]}"

echo
echo "Built:  $build/qdirstat.app"
(( make_dmg )) && echo "DMG:    $build/qdirstat.dmg"
